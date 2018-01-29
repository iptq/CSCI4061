/*
** spock.c -- reads from a message queue
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct my_msgbuf {
  long mtype;
  char mtext[200];
};

int main(void){
  // Get a unique IPC key based on a file name
  key_t key = ftok("kirk.c", 'B');
  if(key == -1){
    perror("ftok");
    exit(1);
  }

  // Connect to message queue based on the key provided
  int msqid = msgget(key, 0644 | IPC_CREAT);
  if(msqid == -1) {
    perror("msgget");
    exit(1);
  }

  printf("spock: ready to receive messages, captain.\n");

  // Spock never quits: forever wait for messages in the queue
  struct my_msgbuf buf;
  for(;;) { 
    int received = msgrcv(msqid, &buf, sizeof buf.mtext, 0, 0);
    if (received == -1) {
      perror("msgrcv");
      exit(1);
    }
    printf("spock: \"%s\"\n", buf.mtext);
  }

  return 0;
}

