#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// usage: sleep_die secs
//
// Sleep for given number of seconds then raise SIGINT which will
// cause the program to die.
int main(int argc, char *argv[]){

  int secs = atoi(argv[1]);

  struct timespec tm = {
    .tv_nsec = 0,
    .tv_sec  = secs,
  };
  nanosleep(&tm,NULL);
  printf("O happy dagger! This is thy sheath;\n");
  printf("there rust, and let me die.\n");
  raise(SIGINT);

  return secs;
}
  
