#ifndef _TOOLS_H_
#define _TOOLS_H_

// detect current working directory and print the prompt. 
void prompt();

// free the inner space allocated to the double pointer argument.
void mem_clean(char * *const argument);

// change string command to a char ** cmd
int parse_cmd(const char *const command, char **const cmd);

// detect if there's pipeline in the input.
int pipe_detector(const char *const input);

// SIGINT handler
void sigint_handler(int sig);

// SIGQUIT handler
void sigquit_handler(int sig);

// SIGTSTOP handler
void sigtstp_handler(int sig);

// At most one input redirection and one output redirection are allowed.
// pipeflag is set to 0,1,2. 
// 0: both input and output redirections are allowed.
// 1: only input redirection is allowed.
// 2: only output redirection is allowed.
// return -1 when invalid command detected.
int IOredir_handler(const char *const command, char *cmd, int pipeflag);

// return -1 when open() fails. 
int inRedir(const char *const command, char * cmd);

// >: flag = 1; >>: flag = 2
int outRedir(const char *const command, char * cmd, int flag);

#endif