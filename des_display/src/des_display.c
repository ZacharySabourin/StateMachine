#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/neutrino.h>
#include <sys/netmgr.h>

#include "../../des_controller/src/des_state/des_state.h"
#include "../../des_controller/src/des_messaging.h"

void server_msg_handler(const state_message_t *client_msg, server_response_t *response);
int is_valid_state(State state);
void handle_invalid_state(server_response_t * response);

int main(void)
{
	state_message_t client_msg = { 0 };
	server_response_t response = { 0 };
	int rcvid;

	printf("des_display pid: %d\n", getpid());

	int server_chid = ChannelCreate(0);
	if (server_chid == CONNECTION_ERROR)
	{
		perror("ChannelCreate");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		rcvid = MsgReceive(server_chid, &client_msg, sizeof(client_msg), NULL);
		if (rcvid == CONNECTION_ERROR)
		{
			perror("MsgReceive");
			exit(EXIT_FAILURE);
		}

		server_msg_handler(&client_msg, &response);

		if (MsgReply(rcvid, EOK, &response, sizeof(response)) == CONNECTION_ERROR)
		{
			perror("MsgReply");
			exit(EXIT_FAILURE);
		}

		if (client_msg.state == EXIT)
			break;
	}

	if (ChannelDestroy(server_chid) == CONNECTION_ERROR)
	{
		perror("ChannelDestroy");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

void server_msg_handler(const state_message_t *client_msg, server_response_t *response)
{
	if (is_valid_state(client_msg->state) == INVALID_STATE)
		handle_invalid_state(response);
	else
	{
		printf("%s\n", client_msg->status_message);

		if (client_msg->state == LEFT_SCANNING || client_msg->state == RIGHT_SCANNING)
			printf("Id Scanned: %d\n", client_msg->person_id);
		if (client_msg->state == WEIGHT_SCANNING)
			printf("Weight Scanned: %d\n", client_msg->person_weight);

		response->status_code = SRVR_OK;
	}
}

int is_valid_state(State state)
{
	if (state > NUM_STATES || state < 0)
		return INVALID_STATE;

	for (int i = IDLE; i <= EXIT; ++i)
		if (state == i)
			return state;

	return INVALID_STATE;
}

void handle_invalid_state(server_response_t * response)
{
	response->status_code = SRVR_INVALID_STATE;
	strcpy(response->error_msg, "Invalid State");
}
