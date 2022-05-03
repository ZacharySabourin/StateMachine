# StateMachine
A multi-threaded, multi-process state machine for my Real-Time Programming course

The goal of this state machine is to simulate a person entering a secured space, locking the door behind them, weighing them, and then letting them leave through the other door.

There were a lot of weird requirements in this assignment, so the person can only technically leave through a door they did not enter but can enter through either side. But can also abort the process at any time. I know, it's weird.

des_controller has a server thread and a client thread. it also manages the state machine's variables. I implemented a mutex using a condvar to protect the state machine memory. This was to prevent any write attempts while the memory was being read. The odds of this happening in such a small system is extremely unlikely, but it never hurts to be safe :).

des_input acts as a client that sends the "command" messages to the des_controller's server thread.

des_controller server thread interprets the message and will then unlock access to the protected state variable.
des_contoller's client thread will then send an update to des_display with the new attempted state.

des_display acts as a server to receive the des_controller client thresd's messages. It then displays if the action taken was correct or not, and displays the current state.

What I learned:
- A firm grasp of multi-threading
- A firm grasp of various forms of IPC
- A firm grasp of C development
