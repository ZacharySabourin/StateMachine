#include "./des_state.h"

output_action state_handlers[NUM_STATES] = { idle_state_handler, left_scan_state_handler,
		left_unlock_state_handler, left_open_state_handler, weight_scan_state_handler,
		left_close_state_handler, left_lock_state_handler, right_scan_state_handler,
		right_unlock_state_handler, right_open_state_handler, right_close_state_handler,
		right_lock_state_handler, exit_state_handler };

Output get_next_state(State current_state, Input input, State entrance)
{
	return state_handlers[current_state](input, entrance);
}

Output idle_state_handler(Input input, State entrance)
{
	if (input == L_SCAN_IN)
		return LEFT_SENSOR_SCANNED;
	else if (input == R_SCAN_IN)
		return RIGHT_SENSOR_SCANNED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return PROMPTING_FOR_EVENT_TYPE;
}

Output left_scan_state_handler(Input input, State entrance)
{
	if (input == GUARD_L_UNLOCK_IN)
		return GUARD_LEFT_DOOR_UNLOCKED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return LEFT_SENSOR_SCANNED;
}

Output left_unlock_state_handler(Input input, State entrance)
{
	if (input == L_OPEN_IN)
		return LEFT_DOOR_OPENED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return GUARD_LEFT_DOOR_UNLOCKED;
}

Output left_open_state_handler(Input input, State entrance)
{
	if (input == WEIGHT_SCAN_IN && entrance == LEFT_SCANNING)
		return WEIGHT_SCAN_IN;
	else if (input == L_CLOSE_IN && entrance == RIGHT_SCANNING)
		return LEFT_DOOR_CLOSED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return LEFT_DOOR_OPENED;
}

Output left_close_state_handler(Input input, State entrance)
{
	if (input == GUARD_L_LOCK_IN)
		return GUARD_LEFT_DOOR_LOCKED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return LEFT_DOOR_CLOSED;
}

Output left_lock_state_handler(Input input, State entrance)
{
	if (input == IDLE_IN && entrance == RIGHT_SCANNING)
		return PROMPTING_FOR_EVENT_TYPE;
	else if (input == GUARD_R_UNLOCK_IN && entrance == LEFT_SCANNING)
		return GUARD_RIGHT_DOOR_UNLOCKED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return GUARD_LEFT_DOOR_LOCKED;
}

Output weight_scan_state_handler(Input input, State entrance)
{
	if (input == L_CLOSE_IN && entrance == LEFT_SCANNING)
		return LEFT_DOOR_CLOSED;
	else if (input == R_CLOSE_IN && entrance == RIGHT_SCANNING)
		return RIGHT_DOOR_CLOSED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return WEIGHT_SCANNED;
}

Output right_scan_state_handler(Input input, State entrance)
{
	if (input == GUARD_R_UNLOCK_IN)
		return GUARD_RIGHT_DOOR_UNLOCKED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return RIGHT_SENSOR_SCANNED;
}

Output right_unlock_state_handler(Input input, State entrance)
{
	if (input == R_OPEN_IN)
		return RIGHT_DOOR_OPENED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return GUARD_RIGHT_DOOR_UNLOCKED;
}

Output right_open_state_handler(Input input, State entrance)
{
	if (input == R_CLOSE_IN && entrance == LEFT_SCANNING)
		return RIGHT_DOOR_CLOSED;
	else if (input == WEIGHT_SCAN_IN && entrance == RIGHT_SCANNING)
		return WEIGHT_SCANNED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return RIGHT_DOOR_OPENED;
}

Output right_close_state_handler(Input input, State entrance)
{
	if (input == GUARD_R_LOCK_IN)
		return GUARD_RIGHT_DOOR_LOCKED;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return RIGHT_DOOR_CLOSED;
}

Output right_lock_state_handler(Input input, State entrance)
{
	if (input == GUARD_L_UNLOCK_IN && entrance == RIGHT_SCANNING)
		return GUARD_LEFT_DOOR_UNLOCKED;
	else if (input == IDLE_IN && entrance == LEFT_SCANNING)
		return PROMPTING_FOR_EVENT_TYPE;
	else if (input == EXIT_IN)
		return exit_state_handler(EXIT_IN, entrance);
	else
		return GUARD_RIGHT_DOOR_LOCKED;
}

Output exit_state_handler(Input input, State entrance)
{
	return EXITING;
}
