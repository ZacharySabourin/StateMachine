#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/neutrino.h>
#include <sys/netmgr.h>

#include "./des_messaging.h"
#include "./des_state/des_state.h"

void run_client_thread(state_message_t *shared_msg, int server_pid);
void loop_and_send_messages(int client_coid, state_message_t *shared_msg);
void handle_display_reply(server_response_t *response);
void toggle_thread_access(int value);

void* run_server_thread(void *shared_message);
Input parse_input(person_t *client_msg);
void update_shared_message(state_message_t *shared_msg, State next_state, person_t *client_msg);
void handle_command_error(server_response_t *response);
void setMsgToIdle(state_message_t *shared_msg, State next_state, person_t *client_msg);
void set_response_ok(server_response_t *response);

const char* const INPUTS_STR[NUM_INPUTS] = { "idle", "ls", "glu", "lo", "ws", "lc", "gll", "rs", "gru", "ro",
		"rc", "grl", "exit" };

const char* const OUTPUT_MESSAGES[NUM_STATES] = { "> PROMPTING FOR EVENT TYPE", "> LEFT SENSOR SCANNED",
		"> GUARD LEFT DOOR UNLOCKED", "> LEFT DOOR OPENED", "> WEIGHT SCANNED", "> LEFT DOOR CLOSED",
		"> GUARD LEFT DOOR LOCKED", "> RIGHT SENSOR SCANNED", "> GUARD RIGHT DOOR UNLOCKED",
		"> RIGHT DOOR OPENED", "> RIGHT DOOR CLOSED", "> GUARD RIGHT DOOR LOCKED", "> EXITING" };

int shared_message_ready = READY;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Must have 2 command-line arguments\n");
		exit(EXIT_FAILURE);
	}

	printf("des_controller pid: %d\n", getpid());

	state_message_t shared_message;
	shared_message.state = IDLE;
	strcpy(shared_message.status_message, OUTPUT_MESSAGES[IDLE]);

	pthread_t server_tid;

	if (pthread_create(&server_tid, NULL, run_server_thread, &shared_message) != EXIT_SUCCESS)
	{
		printf("error creating thread");
		exit(EXIT_FAILURE);
	}

	int server_pid = atoi(argv[1]);

	run_client_thread(&shared_message, server_pid);

	pthread_join(server_tid, NULL);

	return (EXIT_SUCCESS);
}

void run_client_thread(state_message_t *shared_msg, int server_pid)
{
	int client_coid = ConnectAttach(ND_LOCAL_NODE, server_pid, 1, _NTO_SIDE_CHANNEL, 0);
	if (client_coid == CONNECTION_ERROR)
	{
		perror("ConnectAttach");
		exit(EXIT_FAILURE);
	}

	loop_and_send_messages(client_coid, shared_msg);

	if (ConnectDetach(client_coid) == CONNECTION_ERROR)
	{
		perror("ConnectDetach");
		exit(EXIT_FAILURE);
	}
}

void loop_and_send_messages(int client_coid, state_message_t *shared_msg)
{
	server_response_t response;
	int finished = 0;

	while (!finished)
	{
		pthread_mutex_lock(&lock);
		while (!shared_message_ready)
			pthread_cond_wait(&condvar, &lock);

		if (MsgSend(client_coid, shared_msg, sizeof(*shared_msg), &response,
				sizeof(response)) == CONNECTION_ERROR)
		{
			perror("MsgSend");
			exit(EXIT_FAILURE);
		}

		handle_display_reply(&response);

		if (shared_msg->state == EXIT)
			finished = 1;

		toggle_thread_access(NOT_READY);
		pthread_mutex_unlock(&lock);
	}
}

void handle_display_reply(server_response_t *response)
{
	if (response->status_code != SRVR_OK)
		printf("Error: %s\n", response->error_msg);
}

void toggle_thread_access(int value)
{
	shared_message_ready = value;
	pthread_cond_signal(&condvar);
}

void* run_server_thread(void *shared_msg)
{
	State entrance = IDLE;
	Input parsed_input;
	State current_state;
	Output next_state;

	state_message_t *shared_state_msg;
	person_t client_msg;
	server_response_t response;
	int rcvid;

	int server_chid = ChannelCreate(0);
	if (server_chid == CONNECTION_ERROR)
	{
		perror("ChannelCreate");
		exit(EXIT_FAILURE);
	}

	int finished = 0;
	while (!finished)
	{
		rcvid = MsgReceive(server_chid, &client_msg, sizeof(client_msg), NULL);
		if (rcvid == CONNECTION_ERROR)
		{
			perror("MsgReceive");
			exit(EXIT_FAILURE);
		}

		pthread_mutex_lock(&lock);

		while (shared_message_ready)
			pthread_cond_wait(&condvar, &lock);

		shared_state_msg = (state_message_t *) shared_msg;

		current_state = shared_state_msg->state;
		parsed_input = parse_input(&client_msg);
		next_state = (State) get_next_state(current_state, parsed_input, entrance);

		if (parsed_input == INVALID_STATE || (State) next_state == current_state)
			handle_command_error(&response);
		else
		{
			set_response_ok(&response);

			if (((State) next_state == RIGHT_SCANNING || (State) next_state == LEFT_SCANNING))
				entrance = next_state;

			update_shared_message(shared_state_msg, next_state, &client_msg);

			toggle_thread_access(READY);

			if (next_state == EXITING)
				finished = 1;
		}

		pthread_mutex_unlock(&lock);

		//Send a new IDLE input to the machine if it's time to loop back to the start
		//AFTER THE OTHER THREAD HAS HAD A CHANCE TO PROCESS **important
		if (((State) next_state == LEFT_LOCKING && (State) entrance == RIGHT_SCANNING)
				|| ((State) next_state == RIGHT_LOCKING && (State) entrance == LEFT_SCANNING))
		{
			setMsgToIdle(shared_state_msg, IDLE, &client_msg);
		}

		if (MsgReply(rcvid, EOK, &response, sizeof(response)) == CONNECTION_ERROR)
		{
			perror("MsgReply");
			exit(EXIT_FAILURE);
		}
	}

	if (ChannelDestroy(server_chid) == CONNECTION_ERROR)
	{
		perror("ChannelDestroy");
		exit(EXIT_FAILURE);
	}

	return (0);
}

Input parse_input(person_t *client_msg)
{
	for (int i = IDLE_IN; i <= EXIT_IN; ++i)
		if (strncmp(INPUTS_STR[i], client_msg->command, 6) == 0)
			return i;

	return INVALID_STATE;
}

void handle_command_error(server_response_t *response)
{
	response->status_code = SRVR_WRONG_COMMAND;
	strcpy(response->error_msg, "Invalid Command");
}

void update_shared_message(state_message_t *shared_msg, State next_state, person_t *client_msg)
{
	shared_msg->state = next_state;
	strcpy(shared_msg->status_message, OUTPUT_MESSAGES[shared_msg->state]);

	if (next_state == LEFT_SCANNING || next_state == RIGHT_SCANNING)
		shared_msg->person_id = client_msg->person_id;
	if (next_state == WEIGHT_SCANNING)
		shared_msg->person_weight = client_msg->person_weight;
}

void setMsgToIdle(state_message_t *shared_msg, State next_state, person_t *client_msg)
{
	pthread_mutex_lock(&lock);

	while (shared_message_ready)
		pthread_cond_wait(&condvar, &lock);

	update_shared_message(shared_msg, next_state, client_msg);

	toggle_thread_access(READY);

	pthread_mutex_unlock(&lock);
}

void set_response_ok(server_response_t *response)
{
	response->status_code = SRVR_OK;
}

