// Server code which contains a name/email pairs and will fulfill
// requests from a client through FIFOs.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NRECS 21
#define STRSIZE 128
#define SHM_SIZE (NRECS * sizeof(lookup_t))

// structure to store a lookup_t of name-to-email association
typedef struct {
  char name [STRSIZE];
  char email[STRSIZE];
} lookup_t;

lookup_t original_data[NRECS] = {
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
};


int main(int argc, char *argv[]) {
  if(argc < 2){
    printf("usage: %s restore\n",argv[0]);
    printf("       %s lookup <name>\n",argv[0]);
    printf("       %s change <name> <email>\n",argv[0]);
    exit(1);
  }

  key_t key  = ftok("shmdemo.c", 'R');                      // make the SysV IPC key
  int shmid  = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);     // connect to (and possibly create) the segment:
  lookup_t *data = (lookup_t *) shmat(shmid, (void *)0, 0); // attach to the segment to get a pointer to it:

  if( strcmp("restore",argv[1])==0 ){
    printf("Restoring\n");
    for(int i=0; i<NRECS; i++){
      data[i] = original_data[i];
    }
  }
  else if( strcmp("lookup",argv[1])==0 ){
    char *name = argv[2];
    printf("Looking up %s\n",name);
    int found = 0;
    for(int i=0; i<NRECS; i++){
      if(strcmp(name, data[i].name) == 0){
        printf("Found: %s\n",data[i].email);
        found = 1;
      }
    }
    if(!found){
      printf("Not found\n");
    }
  }
  else if( strcmp("change",argv[1])==0 ){
    char *name = argv[2];
    char *email = argv[3];
    printf("Changing %s to %s\n",name,email);
    int found = 0;
    for(int i=0; i<NRECS; i++){
      if(strcmp(name, data[i].name) == 0){
        strcpy(data[i].email, email);
        printf("Alteration complete\n");
        found = 1;
      }
      if(!found){
        printf("Not found\n");
      }
    }
  }
  else{
    printf("Unknown command '%s'\n",argv[1]);
    shmdt(data);
    exit(1);
  }

  shmdt(data);                                            // detach from the shared meory
  exit(0);
}
