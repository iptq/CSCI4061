#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){

  char *verbosity = getenv("ROCK");
  if(verbosity == NULL){
    printf("ROCK not set\n");
  }
  else{
    printf("ROCK is %s\n",verbosity);
    printf("Turning VOLUME to 11\n");
    int fail = setenv("VOLUME","11",1);
    if(fail){
      printf("Couldn't change VOLUME\n");
    }
  }
  char *volume = getenv("VOLUME");
  if(volume == NULL){
    volume = "not set";
  }
  printf("VOLUME is %s\n",volume);
  return 0;
}
  

  
