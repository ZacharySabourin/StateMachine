#ifndef DES_MESSAGING_H_
#define DES_MESSAGING_H_

#define SRVR_OK 0
#define SRVR_WRONG_COMMAND 1
#define SRVR_INVALID_STATE 2

#define READY 1
#define NOT_READY 0

#define CONNECTION_ERROR (-1)

struct response
{
	int status_code;
	char error_msg[128];
}typedef server_response_t;

typedef struct person
{
	char command[128];
	int person_id;
	int person_weight;
} person_t;

#endif
