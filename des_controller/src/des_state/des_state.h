#ifndef DES_STATE_H_
#define DES_STATE_H_

#ifndef DES_MESSAGING_H_
#include "../des_messaging.h"
#endif

#define INVALID_STATE (-1)

#define NUM_STATES 13
#define NUM_INPUTS 13

typedef enum
{
	IDLE = 0,
	LEFT_SCANNING = 1,
	LEFT_UNLOCKING = 2,
	LEFT_OPENING = 3,
	WEIGHT_SCANNING = 4,
	LEFT_CLOSING = 5,
	LEFT_LOCKING = 6,
	RIGHT_SCANNING = 7,
	RIGHT_UNLOCKING = 8,
	RIGHT_OPENING = 9,
	RIGHT_CLOSING = 10,
	RIGHT_LOCKING = 11,
	EXIT = 12
} State;

typedef enum
{
	IDLE_IN,
	L_SCAN_IN,
	GUARD_L_UNLOCK_IN,
	L_OPEN_IN,
	WEIGHT_SCAN_IN,
	L_CLOSE_IN,
	GUARD_L_LOCK_IN,
	R_SCAN_IN,
	GUARD_R_UNLOCK_IN,
	R_OPEN_IN,
	R_CLOSE_IN,
	GUARD_R_LOCK_IN,
	EXIT_IN
} Input;

typedef enum
{
	PROMPTING_FOR_EVENT_TYPE = 0,
	LEFT_SENSOR_SCANNED = 1,
	GUARD_LEFT_DOOR_UNLOCKED = 2,
	LEFT_DOOR_OPENED = 3,
	WEIGHT_SCANNED = 4,
	LEFT_DOOR_CLOSED = 5,
	GUARD_LEFT_DOOR_LOCKED = 6,
	RIGHT_SENSOR_SCANNED = 7,
	GUARD_RIGHT_DOOR_UNLOCKED = 8,
	RIGHT_DOOR_OPENED = 9,
	RIGHT_DOOR_CLOSED = 10,
	GUARD_RIGHT_DOOR_LOCKED = 11,
	EXITING = 12
} Output;

typedef struct state_message
{
	State state;
	int person_weight;
	int person_id;
	char status_message[128];
} state_message_t;

typedef struct state_machine
{
	State system_state;
} state_machine_t;

typedef Output (*output_action)(Input, State);

extern output_action state_handlers[NUM_STATES];

Output get_next_state(State current_state, Input input, State entrance);

Output idle_state_handler(Input, State entrance);
Output left_scan_state_handler(Input, State entrance);
Output left_unlock_state_handler(Input, State entrance);
Output left_open_state_handler(Input, State entrance);
Output weight_scan_state_handler(Input, State entrance);
Output left_close_state_handler(Input, State entrance);
Output left_lock_state_handler(Input, State entrance);
Output right_scan_state_handler(Input, State entrance);
Output right_unlock_state_handler(Input, State entrance);
Output right_open_state_handler(Input, State entrance);
Output right_close_state_handler(Input, State entrance);
Output right_lock_state_handler(Input, State entrance);
Output exit_state_handler(Input, State entrance);

#endif
