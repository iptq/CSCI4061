#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){

  char *child_argv[] = {"--", "derpalerp",NULL};
  char *child_cmd = "cowsay";

  printf("Running command '%s'\n",child_cmd);
  printf("------------------\n");
  
  pid_t child = fork();
  if (child == 0) { // child
    execvp(child_cmd,child_argv);
  } else {
    waitpid(child, 0, 0);
    printf("------------------\n");
    printf("Finished\n");
  }
  return 0;
}
  
