#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int i;
  for (i = 0; i < 8; i++) {
    pid_t child = fork();
    if (child == 0) {
      break;
    } else { // make sure this is only executed on the parent process
      waitpid(child, 0, 0); // wait for the child process to exit with status 0
    }
  }
  printf("I am number %d, my pid is %d\n", i, getpid());
  return 0;
}
