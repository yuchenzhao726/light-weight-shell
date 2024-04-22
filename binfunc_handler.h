#ifndef _BINFUNC_HANDLER_H
#define _BINFUNC_HANDLER_H

// return 0 if it is not a built-in function, return 1 if it is a built-in function.
int binfunc_handler(const char * const input, char ** jobs, int *const job_count, int *job_pid);

// built_in cmd exit, return -1 when successful, return 1 when unsuccessful. 
int bin_exit(int argc, int job_count);

// built_in cmd cd
int bin_cd(char *const *const argv, int argc);

// built_in cmd jobs
int bin_jobs(char **jobs, int job_count, int argc);

// built_in cmd fg
int bin_fg(char **jobs, int *const job_count, int argc, char *const *const argv, int *job_pid);

#endif