#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable : 4996)

#include <sys/types.h> // to use variable type of system
#include <unistd.h> // to use system call
#include <errno.h>
#include <sys/wait.h>

void showPrompt();
void receiveCommand(char** cmd, char*** argv, int* argc);
void initCommand(char** cmd, char*** argv, int* argc);
void processCommand(char** cmd, char*** argv);

#define true 1
#define false 0
typedef int boolean;

#define MAX_INPUT_SIZE 100

#define DEBUG

int main(){

  boolean shellLoop = true;
  char *cmd = NULL, **argv = NULL;
  int argc = 0;

  while (shellLoop){
    showPrompt();
    receiveCommand(&cmd, &argv, &argc);
    #ifdef DEBUG
    printf("my Command is : %s\n", cmd);
    printf("Arguments... ");
    for (int i = 0; i < argc; i++){
      printf("%s,", argv[i]);
    }
    printf("\n");
    #endif
    processCommand(&cmd, &argv);
    initCommand(&cmd, &argv, &argc);
  }
  return 0;
}

void showPrompt(){
  printf("mingyu$");
  return;
}

void receiveCommand(char** cmd, char*** argv, int* argc){

  //
  // input part
  //
  char input[MAX_INPUT_SIZE] = ""; // array saved inputs
  char seps[3] = " "; // tokenizer seperator
  fgets(input, MAX_INPUT_SIZE, stdin); // for security, I must use fgets than gets

  //
  // counting number of token(argument)
  //
  int sizeOfInput = 0;
  int numberOfBlank = 0;
  boolean isBlankBefore = false;
  while(input[sizeOfInput] != '\0'){
    if (isBlankBefore == false && input[sizeOfInput] == ' '){
      numberOfBlank++;
      isBlankBefore = true;
    }
    else if (isBlankBefore == true && input[sizeOfInput] != ' '){
      isBlankBefore = false;
    }
    sizeOfInput++;
  }

  //
  // sepertate command and argument
  //
  char* token;
  token = strtok(input, seps);
  // malloc cmd, argument
  *cmd = (char*)malloc(sizeof(char) * strlen(token));
  *argv = (char**)malloc(sizeof(char*) * (numberOfBlank));

  strcpy(*cmd, token);
  token = strtok(NULL, seps);
  int argIndex = 0;
  while (token != NULL){
    (*argv)[argIndex] = (char*)malloc(sizeof(char*) * strlen(token));
    strcpy((*argv)[argIndex], token);
    token = strtok(NULL, seps);
    argIndex++;
  }
  *argc = numberOfBlank;
  return;
}

void initCommand(char** cmd, char*** argv, int* argc){
  //free(*cmd); why error occur?
  for (int i = 0; i < *argc; i++){
    free((*argv)[i]);
  }
  free(*argv);
  *cmd = NULL;
  *argv = NULL;
  *argc = 0;
}

void processCommand(char** cmd, char*** argv){
  pid_t pid;
  int status;
  pid = fork();
  if (pid < 0){
    // some error occur when make child process
  }
  else if (pid == 0){
    // child process
    execve(*cmd, *argv, 0);
  }
  else {
    // parent process
    // first challenge - wait(NULL);
    while(waitpid(pid, &status, 0) < 0)
    if (errno != EINTR){
      status = -1;
      break;
    }
    printf("Child Process is completed");
  }
  return;
}
