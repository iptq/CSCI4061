// Use mmap() to print the symbol of a specific stock that is in a
// binary file.

#include "stock.h"

int main(int argc, char *argv[]){
  if(argc < 3){
    printf("usage: %s <stock.dat> <stock_num>\n",argv[0]);
    exit(1);
  }

  int in_fd = open(argv[1], O_RDONLY);
  int stock_num = atoi(argv[2]);             // index of stock to retrieve

  struct stat stat_buf;
  fstat(in_fd, &stat_buf);                   // get stats on the open file such as size
  int size = stat_buf.st_size;               // size for mmap()'ed memory is size of file

  stock_t *stocks =                          // pointer to file contents as stock_array
    mmap(NULL, size, PROT_READ,              // call mmap with given size and file descriptor
         MAP_SHARED, in_fd, 0);              // read/write, potentially share, offset 0

  printf("%s\n",stocks[stock_num].symbol);   // access indicated stock via mapped mem

  munmap(stocks, size);                      // unmap and close file
  close(in_fd);
  return 0;
}
