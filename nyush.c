/*
 reference: 
 how to use getcwd: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
 Maximum path length in Linux: https://stackoverflow.com/questions/833291/is-there-an-equivalent-to-winapis-max-path-under-linux-unix
 About getline(): https://c-for-dummies.com/blog/?p=1112
 About pid library: https://stackoverflow.com/questions/5460421/how-do-you-write-a-c-program-to-execute-another-program
 About chdir(): https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
 dup2() manual: https://man7.org/linux/man-pages/man2/dup.2.html
 identifier library: https://techoverflow.net/2019/06/21/how-to-fix-undeclared-o_rdonly-o_rdwr-o_wronly-in-c/
 Significant usage about execvp(): https://stackoverflow.com/questions/21896000/c-execvp-and-command-line-arguments
 pipe(3p): https://man7.org/linux/man-pages/man3/pipe.3p.html
 strchr():https://cplusplus.com/reference/cstring/strchr/
 Why does strncpy not null terminate?: https://stackoverflow.com/questions/1453876/why-does-strncpy-not-null-terminate
*/

/*
In this program, cmdname = ls, cd, grep, etc. cmd = cmdname + argv; command = cmd + redirections; commands = command + pipe;
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tools.h"
#include "command_handler.h"
#include "pipe_handler.h"
#include "binfunc_handler.h"


int main(){
  size_t cmdsize = 32;
  char *user_input = (char *)malloc(cmdsize * sizeof(char));
  char **jobs = (char **)malloc(101);
  pid_t pid;
  int status;
  int job_count = 0;
  int job_pid[100];

  jobs[0] = NULL;

  // continuously prompt in shell process
  while(1){
    // restore default signal handler for child process
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    signal(SIGTSTP, sigtstp_handler);

    prompt();
    user_input[0] = '\0';
    getline(&user_input, &cmdsize, stdin);

    // handle the no input case. 
    if(*user_input == '\0'){
      return 0;
    }

    // detect and handle built-in functions
    if(binfunc_handler(user_input, jobs, &job_count, job_pid))
      continue;
    else{ // not built-in condition
      if(pipe_detector(user_input)){
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        pipe_handler(user_input);
      }
      else{
        pid = fork();
        if(pid < 0){
          fprintf(stderr, "Error: lack of pids\n");
          break;
        } 
        else if(pid == 0){
          signal(SIGINT, SIG_DFL);
          signal(SIGQUIT, SIG_DFL);
          signal(SIGTSTP, SIG_DFL);
          command_handler(user_input);
          break;
        } 
        else{
          waitpid(pid, &status, WUNTRACED);
          // job suspended
          if(WIFSTOPPED(status)){
            job_count ++;
            jobs[job_count-1] = (char*)malloc(strlen(user_input)+1);
            strcpy(jobs[job_count-1], user_input);
            job_pid[job_count-1] = pid;
            jobs[job_count] = NULL;
          }
        }
      }
    }
  }
  free(user_input);
  mem_clean(jobs);
  free(jobs);
}