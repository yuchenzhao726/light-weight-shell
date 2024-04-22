#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "command_handler.h"
#include "tools.h"

int command_handler(const char *const command){
  char *cmd = (char *)malloc(1000);
  cmd[0] = '\0';
  if(IOredir_handler(command, cmd, 0) == 0)
    cmd_handler(cmd);
  free(cmd);
  return 0;
}

/*
int cmd_handler(const char *const cmd){
  if(cmd[0] == '\0')
    return 0;
  char **argv = (char **)malloc(sizeof(char *) * strlen(cmd));

  parse_cmd(cmd, argv);
  execvp(argv[0], argv);
  fprintf(stderr, "Error: invalid program\n");
  mem_clean(argv);
  free(argv);
  return 0;
}
*/

int cmd_handler(const char *const cmd){
  if(cmd[0] == '\0')
    return 0;
  char **argv = (char **)malloc(sizeof(char *) * strlen(cmd));
  int flag = 0;
  const char *pt = cmd;
  while(*pt != '\0' && *pt != ' '){
    if(*pt == '/')
      flag = 1;
    pt ++;
  }
  if(flag){
    parse_cmd(cmd, argv);
  }
  else{
    char cmd_temp[strlen(cmd) + 10];
    char temp[] = "/usr/bin/";
    memset(cmd_temp, '\0', strlen(cmd) + 10);
    strcpy(cmd_temp, temp);
    strcat(cmd_temp, cmd);
    parse_cmd(cmd_temp, argv);
  }
  execv(argv[0], argv);
  fprintf(stderr, "Error: invalid program\n");
  mem_clean(argv);
  free(argv);
  return 0;
}