#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include "tools.h"
#include "binfunc_handler.h"

int binfunc_handler(const char *const input, char ** jobs, int *const job_count, int *job_pid){
  int argc;
  int flag = 0;
  char **argv = (char **)malloc(1000);
  char *const *const temp = argv;

  if((argc = parse_cmd(input, argv)) == -1){
    return 1;
  }

  // built-in cmdname exit
  if(strcmp(argv[0], "exit") == 0)
    flag = bin_exit(argc, *job_count);

  // built-in cmdname cd
  else if(strcmp(argv[0], "cd") == 0){
    bin_cd(temp, argc);
    flag = 1;
  }

  // built-in cmdname fg
  else if(strcmp(argv[0], "fg") == 0){
    bin_fg(jobs, job_count, argc, argv, job_pid);
    flag = 1;
  }

  // built-in cmdname jobs
  else if(strcmp(argv[0], "jobs") == 0){
    bin_jobs(jobs, *job_count, argc);
    flag = 1;
  }

  mem_clean(argv);
  free(argv);
  return flag;
}

int bin_exit(int argc, int job_count){
  // with no argument
  if(argc != 0){
    fprintf(stderr, "Error: invalid command\n");
    return 1;
  }
  // with suspended jobs
  else if(job_count != 0){
    fprintf(stderr, "Error: there are suspended jobs\n");
    return 1;
  }
  else
    exit(0);
}

int bin_cd(char *const *const argv, int argc){
  if(argc == 0 || argc >= 2){
    fprintf(stderr, "Error: invalid command\n");
    return -1;
  }
  else if(chdir(argv[1]) != 0){
    fprintf(stderr, "Error: invalid directory\n");
    return -1;
  }
  return 0;
}

int bin_jobs(char **jobs, int job_count, int argc){
  int i = 0;
  if(argc != 0){
    fprintf(stderr, "Error: invalid command\n");
    return -1;
  }
  for(i=0;i<job_count;i++){
    printf("[%d] %s", i+1, jobs[i]);
  }
  return 0;
}

int bin_fg(char **jobs, int *const job_count, int argc, char *const *const argv, int *job_pid){
  if(argc == 0 || argc >= 2){
    fprintf(stderr, "Error: invalid command\n");
    return -1;
  }
  else if((*job_count) < atoi(argv[1]) || atoi(argv[1]) < 1){
    fprintf(stderr, "Error: invalid job\n");
    return -1;
  }
  else{
    int i;
    int job_no = atoi(argv[1]);
    char *temp = jobs[job_no-1];
    pid_t temp_pid = job_pid[job_no-1];
    kill(job_pid[job_no-1], SIGCONT);

    (*job_count) --;
    for(i=job_no-1;i<*job_count;i++){
      jobs[i] = jobs[i+1];
      job_pid[i] = job_pid[i+1];
    }
    jobs[*job_count] = NULL;

    int status;
    waitpid(temp_pid, &status, WUNTRACED);
    // job suspended
    if(WIFSTOPPED(status)){
      (*job_count) ++;
      jobs[(*job_count)-1] = (char*)malloc(strlen(temp));
      strcpy(jobs[(*job_count)-1], temp);
      job_pid[(*job_count)-1] = temp_pid;
      jobs[(*job_count)] = NULL;
    }
    else{
      free(temp);
    }

    return 0;
  }
}