#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#pragma warning(disable : 4996)

#include <sys/types.h> // to use variable type of system
#include <unistd.h> // to use system call
#include <errno.h>
#include <sys/wait.h>

void showPrompt();
void receiveCommand(char*** argv, int* argc);
void initCommand(char*** argv, int* argc);
void processCommand(char*** argv, int* argc);

#define true 1
#define false 0
typedef int boolean;

#define MAX_INPUT_SIZE 100

#define DEBUG_NO

int main(){

  boolean shellLoop = true;
  char **argv = NULL;
  int argc = 0;

  while (shellLoop){
    showPrompt();
    receiveCommand(&argv, &argc);
    #ifdef DEBUG
    printf("Arguments... ");
    for (int i = 0; i < argc; i++){
      printf("%s,", argv[i]);
    }
    printf("\n");
    #endif
    processCommand(&argv, &argc);
    initCommand(&argv, &argc);
  }
  return 0;
}

void showPrompt(){
  printf("mingyu$ ");
  return;
}

void receiveCommand(char*** argv, int* argc){

  //
  // input part
  //
  char input[MAX_INPUT_SIZE] = ""; // array saved inputs
  char seps[3] = " "; // tokenizer seperator
  do {
    fgets(input, MAX_INPUT_SIZE, stdin); // for security, I must use fgets than gets
  } while(strlen(input) == 1); // catch the case that user only type enter

  //
  // counting number of token(argument)
  //
  int sizeOfInput = 0; // size of input(to iterate)
  int numberOfBlank = 0; // how many blank in input string
  boolean isBlankBefore = false; // to find pattern 'blank char blank'
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
  input[sizeOfInput-1] = '\0';
  numberOfBlank += 1; // consider command

  //
  // sepertate command and argument
  //
  *argv = (char**)malloc(sizeof(char*) * (numberOfBlank));
  char* token;
  token = strtok(input, seps);
  int argIndex = 0;
  while(token != NULL){
    (*argv)[argIndex] = (char*)malloc(strlen(token)+1);
    strcpy((*argv)[argIndex], token);
    token = strtok(NULL, seps);
    argIndex++;
  }
  *argc = numberOfBlank;
  return;
}

void initCommand(char*** argv, int* argc){
  for (int i = 0; i < *argc; i++){
    free((*argv)[i]);
  }
  free(*argv);
  *argv = NULL;
  *argc = 0;
}

void processCommand(char*** argv, int* argc){
  #ifdef DEBUG
    printf("Arguments... ");
    for (int i = 0; i < *argc; i++){
      printf("%s.", (*argv)[i]);
    }
    printf("\n");
  #endif
  // exit code
  if (strcmp((*argv)[0],"exit") == 0){
    exit(0);
  }
  pid_t pid;
  int status = 1;
  pid = fork();
  if (pid < 0){
    // some error occur when make child process
    status = -1;
  }
  else if (pid == 0){
    // child process
    execvp((*argv)[0], *argv);
  }
  else {
    // parent process
    // first challenge - wait(NULL);
    while(waitpid(pid, &status, 0) < 0)
    if (errno != EINTR){
      status = -1;
      break;
    }
    printf("Child Process is completed\n");
  }
  return;
}
