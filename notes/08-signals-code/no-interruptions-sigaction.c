// A C program that does not terminate from an interrupt signal.
// Usually pressing Ctrl-C sends this to the foreground program.
//
// To stop this program from running, open another terminal and try
//  > pkill -9 a.out
// assuming you named the output program a.out
//
// This version uses portable signal handler setup
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Function run when a signals are caught
void handle_signals(int sig_num) {
  if(sig_num == SIGINT){
    printf("\nNo SIGINT-erruptions allowed.\n");
  }
  else if(sig_num == SIGTERM){
    printf("\nTry to SIGTERM me? Piss off!\n");
  }
  fflush(stdout);
}
 
int main () {
  // Set handling functions for programs using portable handler setup
  // with sigaction()
  struct sigaction my_sa = {};        // carries information about handler and options
  my_sa.sa_handler = handle_signals;  // run function handle_signals
  sigemptyset(&my_sa.sa_mask);        // don't block any other signals during handling
  my_sa.sa_flags = SA_RESTART;        // restart system calls on signals if possible 
  sigaction(SIGTERM, &my_sa, NULL);   // register SIGTERM with given action
  sigaction(SIGINT,  &my_sa, NULL);   // register SIGINT with given action

  // Infinite loop
  while(1) {        
    sleep(1);
    printf("Ma-na na-na!\n");
    fflush(stdout);
  }
  return 0;
}
