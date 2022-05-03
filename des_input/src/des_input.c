#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/neutrino.h>
#include <sys/netmgr.h>

#include "../../des_controller/src/des_messaging.h"

void loop_and_send_messages(int client_coid);
void handle_controller_reply(server_response_t *response);
void build_out_message(person_t *client_msg);
void prompt_for_event_type(char *message);
void handle_additional_prompt(person_t *client_msg);
int prompt_for_number(void);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Must have 2 command-line arguments\n");
		exit(EXIT_FAILURE);
	}

	int server_pid = atoi(argv[1]);

	int client_coid = ConnectAttach(ND_LOCAL_NODE, server_pid, 1, _NTO_SIDE_CHANNEL, 0);
	if (client_coid == CONNECTION_ERROR)
	{
		perror("ConnectAttach");
		exit(EXIT_FAILURE);
	}

	loop_and_send_messages(client_coid);

	if (ConnectDetach(client_coid) == CONNECTION_ERROR)
	{
		perror("ConnectDetach");
		exit(EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

void loop_and_send_messages(int client_coid)
{
	person_t client_msg;
	server_response_t response = { 0 };
	int finished = 0;

	while (!finished)
	{
		build_out_message(&client_msg);

		if (MsgSend(client_coid, &client_msg, sizeof(client_msg), &response, sizeof(response))
				== CONNECTION_ERROR)
		{
			perror("MsgSend");
			exit(EXIT_FAILURE);
		}

		handle_controller_reply(&response);

		if (strcmp("exit", client_msg.command) == 0)
			finished = 1;
	}
}

void build_out_message(person_t *client_msg)
{
	char message[128];

	prompt_for_event_type(message);

	strcpy(client_msg->command, message);

	handle_additional_prompt(client_msg);
}

void prompt_for_event_type(char *message)
{
	printf("Enter the event type (ls= left scan, rs= right scan, ws= weight scale, lo =left open,\n");
	printf("ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard\n");
	printf("right lock, gll=guard left lock, glu = guard left unlock)\n");

	scanf("%s", message);
}

void handle_additional_prompt(person_t *client_msg)
{
	if ((strncmp("ls", client_msg->command, 5) == 0) || (strncmp("rs", client_msg->command, 5) == 0))
		client_msg->person_id = prompt_for_number();
	else if (strncmp("ws", client_msg->command, 5) == 0)
		client_msg->person_weight = prompt_for_number();
}

int prompt_for_number(void)
{
	int num = 0;
	scanf("%d", &num);
	return num;
}

void handle_controller_reply(server_response_t *response)
{
	if (response->status_code != SRVR_OK)
		printf("Error: %s\n", response->error_msg);
}
