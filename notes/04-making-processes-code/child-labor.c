// Demonstrate the basics of fork/exec to launch a child process to do
// "labor".
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv){

  char *child_argv[] = {"ls","-l","-ah",NULL};          // argument array to child, must end with NULL 
  char *child_cmd = "ls";                               // actual command to run, must be on path

  printf("I'm %d, and I really don't feel like '%s'ing\n",
         getpid(),child_cmd);                           // use of getpid() to get current PID
  printf("I have a solution\n");

  pid_t child_pid = fork();                             // clone a child

  if(child_pid == 0){                                   // child will have a 0 here
    printf("   I'm %d My pa '%d' wants me to '%s'. This sucks.\n",
           getpid(), getppid(), child_cmd);             // use of getpid() and getppid()

    execvp(child_cmd, child_argv);                      // replace running image with child_cmd

    printf("   I don't feel like myself anymore...\n"); // unreachable statement
  }
  else{                                                 // parent will see nonzero in child_pid
    printf("Great, junior %d is taking care of that\n",
           child_pid);
  }
  return 0;
}
  
