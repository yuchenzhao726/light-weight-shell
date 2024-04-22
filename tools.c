#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tools.h"

void prompt(){
  char cwd[FILENAME_MAX]; // store the whole path
  char basename[FILENAME_MAX]; //temporarily store the candidate basename when iterating
  char *pt = NULL; // for current char
  char *root = "/";

  getcwd(cwd, sizeof(cwd)); // get current working directory
  pt = cwd;
  basename[0] = '\0';

  if(strcmp(cwd, root) == 0){
    basename[0] = '/';
    basename[1] = '\0';
  }
  else{
    if(*pt == '/')
      pt ++;
    while(*pt != '\0'){
      if(*pt == '/')
        basename[0] = '\0';
      
      else
        strncat(basename, pt, 1);
      
      pt ++;
    }
  }
  
  printf("[nyush %s]$ ", basename);
  fflush(stdout);
}

void mem_clean(char * *const argument){
  int i = 0;
  while(argument[i] != NULL){
    free(argument[i]);
    i ++;
  }
}

int parse_cmd(const char *const command, char **const cmd){
  const char *pt = command;
  int i = 0;

  while(*pt == ' '){
    pt ++;
  }

  while(*pt != '\n' && *pt != '\0'){
    cmd[i] = (char *)malloc(sizeof(char)*(strlen(pt) + 1));
    cmd[i][0] = '\0';
    while(*pt != ' ' && *pt != '\n'){
      strncat(cmd[i], pt, 1);
      pt ++;
    }
    i ++;
    while(*pt == ' '){
      pt ++;
    }
  }

  cmd[i] = NULL;
  return i-1;
}

int pipe_detector(const char *const input){
  char *pt = NULL;
  pt = strchr(input, '|');
  if(pt == NULL)
    return 0;
  else
    return 1;
}

// SIGINT handler
void sigint_handler(int sig){
  int a = sig;
  sig = a;
}

// SIGQUIT handler
void sigquit_handler(int sig){
  int a = sig;
  sig = a;
}

// SIGTSTOP handler
void sigtstp_handler(int sig){
  int a = sig;
  sig = a;
}

int IOredir_handler(const char *const command, char *cmd, int pipeflag){
  char temp[1001];
  if(command[0] == '\0')
    return 0;
  
  const char *pt = command;
  int indir_flag = 0;
  int outdir_flag = 0;
  
  // validate the input redirection and the output redirection
  while(*pt != '\n' && *pt != '\0'){
    if(*pt == '<'){
      if(indir_flag == 1 || pipeflag == 2 || pipeflag == 3){
        fprintf(stderr, "Error: invalid command\n");
        return -1;
      }
      else{
        indir_flag = 1;
      }
    }
    else if(*pt == '>'){
      if(outdir_flag == 1 || outdir_flag == 2 || pipeflag == 1 || pipeflag == 3){
        fprintf(stderr, "Error: invalid command\n");
        return -1;
      }
      else{
        pt ++;
        if(*pt == '>'){
          outdir_flag = 2;
          pt ++;
        }
        else
          outdir_flag = 1;
      }
    }
    pt ++;
  }
  
  if(indir_flag){
    if(inRedir(command, cmd) == -1){
      return -1;
    }
  }
  else
    strcpy(cmd, command);
  
  if(outdir_flag){
    strcpy(temp, cmd);
    cmd[0] = '\0';
    outRedir(temp, cmd, outdir_flag);
  }
  if(indir_flag == 0 && outdir_flag == 0)
    strcpy(cmd, command);
  return 0;
}

int inRedir(const char *const command, char * cmd){
  const char *pt = command;
  char path[strlen(command)];
  path[0] = '\0';
  while(*pt != '\n' && *pt != '\0' && *pt != '<'){
    strncat(cmd, pt, 1);
    pt ++;
  }
  if(*pt == '<'){
    pt ++;
    while(*pt == ' ')
      pt ++;
    while(*pt != ' ' && *pt != '\0' && *pt != '\n'){
      strncat(path, pt, 1);
      pt ++;
    }
  }
  while(*pt != '\0'){
    strncat(cmd, pt, 1);
    pt ++;
  }
  if(path[0] == '\0'){
    fprintf(stderr, "Error: invalid command\n");
    return -1;
  }
  int fd = open(path, O_RDONLY);
  if(fd == -1){
    fprintf(stderr, "Error: invalid file\n");
    return -1;
  }
  dup2(fd, 0);
  close(fd);
  return 0;
}

int outRedir(const char *const command, char * cmd, int flag){
  const char *pt = command;
  char path[strlen(command)];
  path[0] = '\0';
  while(*pt != '\n' && *pt != '\0' && *pt != '>'){
    strncat(cmd, pt, 1);
    pt ++;
  }
  if(*pt == '>'){
    pt ++;
    while(*pt == ' ' || *pt == '>')
      pt ++;
    while(*pt != ' ' && *pt != '\0' && *pt != '\n'){
      strncat(path, pt, 1);
      pt ++;
    }
  }
  while(*pt != ' ' && *pt != '\0'){
    strncat(cmd, pt, 1);
    pt ++;
  }
  int fd = -1;
  if (flag == 1){
    fd = open(path, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
  }
  else if (flag == 2){
    fd = open(path, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR);
  }
  dup2(fd, 1);
  close(fd);
  return 0;
}