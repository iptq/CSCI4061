// demonstrate creation of zombie processes: children that outlive
// their parent
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv){

  // char *child_argv[] = {"zombie",NULL};
  // char *child_cmd = "zombie";

  pid_t child_pid = fork();

  // CHILD CODE
  if(child_pid == 0){
    printf("ZOMBIE %d: Brains...\n", getpid());
    sleep(1);
    exit(0);
    // execvp(child_cmd, child_argv);
  }

  // PARENT CODE
  printf("NECROMANCER: Go forth my servant #%d...\n",child_pid);
  printf("NECROMANCER: Now for a nap...\n");
  sleep(10);
  
  wait(NULL);
  // printf("NECROMANCER: I'm not waiting around to see what happens\n");
  return 0;
}
  
