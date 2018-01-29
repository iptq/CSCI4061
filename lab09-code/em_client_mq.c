// Client code to look up an email based on name; contacts servers via
// System V IPC message queues.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// structure to store a lookup. client fills in with name, server
// fills in email if found in 
typedef struct {
  long mtype;                   // contains PID of sending process
  char name[256];               // name desired
  char email[256];              // email associated with name
} lookup_t;

int main(int argc, char *argv[]){
  setvbuf(stdout, NULL, _IONBF, 0); 

  if(argc < 2){
    printf("usage: %s <name>\n",argv[0]);
    exit(1);
  }

  key_t key;
  key = ftok("em_server_mq.c", 0);
  int to_server_qid = msgget(key, 0644 | IPC_CREAT);
  key = ftok("em_client_mq.c", 0);
  int to_clients_qid = msgget(key, 0644 | IPC_CREAT);

  int my_pid = getpid();
  lookup_t request = {};
  request.mtype = my_pid;
  strcpy(request.name, argv[1]);

  printf("%ld A CLIENT: sending request: {pid=%ld  name=\"%s\" }\n",
         request.mtype, request.mtype, request.name);
  msgsnd(to_server_qid, &request, sizeof(lookup_t), 0);

  lookup_t response = {};
  msgrcv(to_clients_qid, &response, sizeof(lookup_t), my_pid, 0);

  printf("%ld D CLIENT: received response: {pid=%ld  name=\"%s\" email=\"%s\"}\n",
         response.mtype, response.mtype, response.name, response.email);

  if(my_pid != response.mtype){
    printf("CLIENT PROBLEM: PID %d vs %ld\n", my_pid, response.mtype);
    exit(1);
  }

  exit(0);
}
