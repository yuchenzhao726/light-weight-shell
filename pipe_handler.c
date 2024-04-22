#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "pipe_handler.h"
#include "tools.h"

int pipe_handler(const char *commands){
  char *command = (char *)malloc(strlen(commands)); // store a line of command
  const char *curr_command_ptr = commands; // pointing to current command in commands
  const char *next_command_ptr = NULL; // pointing to next command in commands
  int fd1[2];
  int fd2[2];
  int parse_status = 1; // value change to 0 if this is the last command in commands
  pid_t pid;

  if(*curr_command_ptr == '|'){
    fprintf(stderr, "Error: invalid command\n");
    return -1;
  }

  command[0] = '\0';
  next_command_ptr = get_next_command(curr_command_ptr, command, &parse_status);
  pipe(fd2);
  pid = fork();

  if(pid == 0){
    close(fd2[0]);
    dup2(fd2[1], 1);
    close(fd2[1]);

    char cmd[strlen(command)];
    char **argv = (char **)malloc(sizeof(char *)*strlen(cmd));
    if(IOredir_handler(command, cmd, 1) == -1)
      exit(EXIT_FAILURE);
    parse_cmd(cmd, argv);
    execvp(argv[0], argv);
    fprintf(stderr, "Error: invalid command\n");
    mem_clean(argv);
    free(command);
    exit(EXIT_FAILURE);
  }
  else{
    curr_command_ptr = next_command_ptr;
    next_command_ptr = get_next_command(curr_command_ptr, command, &parse_status);

    while(parse_status){
      fd1[0] = fd2[0];
      fd1[1] = fd2[1];
      pipe(fd2);
      pid = fork();
      if(pid == 0){
        close(fd2[0]);
        dup2(fd2[1], 1);
        close(fd2[1]);

        close(fd1[1]);
        dup2(fd1[0], 0);
        close(fd1[0]);

        char cmd[strlen(command)];
        char **argv = (char **)malloc(sizeof(char *)*strlen(cmd));
        if(IOredir_handler(command, cmd, 3) == -1)
          exit(EXIT_FAILURE);
        parse_cmd(cmd, argv);
        execvp(argv[0], argv);
        fprintf(stderr, "Error: invalid command\n");
        mem_clean(argv);
        free(command);
        exit(EXIT_FAILURE);
      }
      else{
        close(fd1[0]);
        close(fd1[1]);
        curr_command_ptr = next_command_ptr;
        next_command_ptr = get_next_command(curr_command_ptr, command, &parse_status);
      }
    }

    pid = fork();
    if(pid == 0){
      close(fd2[1]);
      dup2(fd2[0], 0);
      close(fd2[0]);

      char cmd[strlen(command)];
      char **argv = (char **)malloc(sizeof(char *)*strlen(cmd));
      if(IOredir_handler(command, cmd, 2) == -1)
        exit(EXIT_FAILURE);
      parse_cmd(cmd, argv);
      execvp(argv[0], argv);
      fprintf(stderr, "Error: invalid command\n");
      mem_clean(argv);
      free(command);
      exit(EXIT_FAILURE);
    }
    else{
      close(fd2[0]);
      close(fd2[1]);
      while(wait(NULL) > 0);
      free(command);
    }
  }
  return 0;
}


const char* get_next_command(const char *commands, char * cmd, int* status){
  char *pt = NULL;
  pt = strchr(commands, '|');
  if(pt == NULL){
    strcpy(cmd, commands);
    *status = 0;
    return commands;
  }
  else{
    *status = 1;
    cmd[0] = '\0';
    strncat(cmd, commands, (pt-commands));
    pt ++;
    while(*pt == ' ')
      pt ++;
    return pt;
  }
}

/*
  char **cmd = (char **)malloc(strlen(commands));
  const char *temp = commands;
  const char **ptr = &temp;
  int lastfd[2], nextfd[2];
  pid_t pid;
  lastfd[0] = -1;
  lastfd[1] = -1;
  nextfd[0] = -1;
  nextfd[1] = -1;
  cmd[0] = NULL;

  while(get_next_command(ptr, cmd) != -1){

    if(lastfd[1] != -1){
      close(lastfd[1]);
    }
    if(lastfd[0] != -1){
      close(lastfd[0]);
    }

    lastfd[0] = nextfd[0];
    lastfd[1] = nextfd[1];
    pipe(nextfd);

    pid = fork();
    if(pid == 0){
      if(lastfd[1] != -1){
        close(lastfd[1]);
      }
      if(lastfd[0] != -1){
        dup2(lastfd[0], 0);
        close(lastfd[0]);
      }
      close(nextfd[0]);
      dup2(nextfd[1], 1);
      close(nextfd[1]);
      unit_handler();
    }
  }

  // last command in pipe, there are still four fds.
  if(lastfd[1] != -1){
    close(lastfd[1]);
  }
  if(lastfd[0] != -1){
    close(lastfd[0]);
  }
  lastfd[0] = nextfd[0];
  lastfd[1] = nextfd[1];
  pid = fork();
  if(pid == 0){
    if(lastfd[1] != -1){
      close(lastfd[1]);
    }
    if(lastfd[0] != -1){
      dup2(lastfd[0], 0);
      close(lastfd[0]);
    }
    unit_handler();
  }
  else{
    close(lastfd[0]);
    close(lastfd[1]);
    while(wait(NULL) > 0);
  }
  return 0;
*/