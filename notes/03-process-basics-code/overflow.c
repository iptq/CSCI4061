#include <stdio.h>

// This program traverses stuff that it really ought not to...

int main(int argc, char *argv[]){
  char a[3] = {'A','B','C'};
  int i = 0;
  while(1){
    printf("%c",a[i]);
    i++;
    if(i%40 == 0){
      printf("\n");
    }
  }
  return 0;
}
