// Server code which contains a name/email pairs and will fulfill
// requests from a client through System V IPC message queues.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// data the server has and clients want: pairings of name and email
char *data[][2] = {
  {"Chris Kauffman"       ,"kauffman@umn.edu"},
  {"Christopher Jonathan" ,"jonat003@umn.edu"},
  {"Amy Larson"           ,"larson@cs.umn.edu"},
  {"Chris Dovolis"        ,"dovolis@cs.umn.edu"},
  {"Dan Knights"          ,"knights@cs.umn.edu"},
  {"George Karypis"       ,"karypis@cs.umn.edu"},
  {"Steven Jensen"        ,"sjensen@cs.umn.edu"},
  {"Daniel Keefe"         ,"dfk@umn.edu"},
  {"Michael W. Whalen"    ,"whalen@cs.umn.edu"},
  {"Catherine Qi Zhao"    ,"qzhao@umn.edu"},
  {"Dan Challou"          ,"challou@cs.umn.edu"},
  {"Steven Wu"            ,"zsw@umn.edu"},
  {"Michael Steinbach"    ,"steinbac@cs.umn.edu"},
  {"Jon Weissman"         ,"jon@cs.umn.edu"},
  {"Victoria Interrante"  ,"interran@cs.umn.edu"},
  {"Shana Watters"        ,"watt0087@umn.edu"},
  {"James Parker"         ,"jparker@cs.umn.edu"},
  {"James Moen"           ,"moen0017@cs.umn.edu"},
  {"Daniel Giesel"        ,"giese138@umn.edu"},
  {"Jon Read"             ,"readx028@umn.edu"},
  {"Sara Stokowski"       ,"stoko004@umn.edu"},
  {NULL                   , NULL},
};

// structure to store a lookup. client fills in with name, server
// fills in email if found in 
typedef struct {
  long mtype;                   // contains PID of sending process
  char name[256];               // name desired
  char email[256];              // email associated with name
} lookup_t;

int signalled = 0;
void handle_signals(int sig_num){
  signalled = 1;
}

int main() {
  setvbuf(stdout, NULL, _IONBF, 0); 
  struct sigaction my_sa = {};                                       // portable signal handling setup with sigaction()
  my_sa.sa_handler = handle_signals;                                 // run function handle_signals
  sigaction(SIGTERM, &my_sa, NULL);                                  // register SIGCONT with given action
  sigaction(SIGINT,  &my_sa, NULL);                                  // register SIGCONT with given action

  printf("starting server\n");

  key_t key;
  key = ftok("em_server_mq.c", 0);
  int to_server_qid = msgget(key, 0644 | IPC_CREAT);
  key = ftok("em_client_mq.c", 0);
  int to_clients_qid = msgget(key, 0644 | IPC_CREAT);
  
  printf("SERVER: created/attached to message queues\n");

  while(!signalled){
    lookup_t request;
    msgrcv(to_server_qid, &request, sizeof(lookup_t), 0, 0);
    printf("%ld B SERVER: received request {pid=%ld  name=\"%s\" }\n",
           request.mtype, request.mtype, request.name);
    char *name = request.name;
    int found = 0;
    for(int i=0; data[i][0] != NULL; i++){
      if(strcmp(name, data[i][0]) == 0){
        strcpy(request.email, data[i][1]);
        found = 1;
      }
    }
    if(!found){
      strcpy(request.email, "NOT FOUND");
    }
    printf("%ld C SERVER: filling request {pid=%ld  name=\"%s\"  email=\"%s\" }\n",
           request.mtype,request.mtype, request.name, request.email);

    msgsnd(to_clients_qid, &request, sizeof(request), 0);
  }

  msgctl(to_server_qid,  IPC_RMID, NULL); // clean up the message queues
  msgctl(to_clients_qid, IPC_RMID, NULL);
  
  exit(0);
}
