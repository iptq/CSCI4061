// Demonstrate use of mmap() to print the first 128 characters of a
// text file
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[]){
  int fd = open("gettysburg.txt", O_RDONLY); // open file to get file descriptor
  int size = 128;                            // size for mmap()'ed memory
  char *file_chars =                         // pointer to file contents
    mmap(NULL, size, PROT_READ, MAP_SHARED,  // call mmap with given size and file descriptor
         fd, 0);                             // read only, potentially share, offset 0

  for(int i=0; i<size; i++){                 // iterate through characters in file/mapped mem
    printf("%c",file_chars[i]);              // print each
  }
  printf("\n");

  munmap(file_chars, size);                  // unmap and close file
  close(fd);
  return 0;
}
