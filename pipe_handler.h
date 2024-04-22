#ifndef _PIPE_HANDLER_H_
#define _PIPE_HANDLER_H_

// return 0 when succeeds.
// To be noticed, pipe handler are still running in shell process.
int pipe_handler(const char *commands);

// int unit_handler();

// get current command and return a pointer to the next command
// store the command into **cmd
// move commands to the beginning of next single command. 
// status change from 1 to 0 when current command is the last command
const char* get_next_command(const char *commands, char * cmd, int* status);

#endif