// Print the symbol of a specific stock that is in a binary file.

#include "header.h"

int main(int argc, char *argv[]){
  if(argc < 3){
    printf("usage: %s <stock.dat> <stock_num>\n",argv[0]);
    exit(1);
  }

  int in_fd = open(argv[1], O_RDONLY);
  int stock_num = atoi(argv[2]);                       // index of stock to retrieve
  FILE *out_file = stdout;                             // write to standard output
  struct stat stat_buf;

  int offset = stock_num * sizeof(stock_t);            // calculate offset into file
  off_t pos = lseek(in_fd, offset, SEEK_SET);          // jump to that location
  fstat(in_fd, &stat_buf);                             // get stats on the open file such as size
  if(pos > stat_buf.st_size){                          // check for out of bounds
    fprintf(out_file,"Off end of file\n");
  }
  else{
    stock_t stock = {};
    read(in_fd, &stock, sizeof(stock_t));                // read one stock in binary format
    fprintf(out_file,"Stock %d has symbol %s\n",
            stock_num,stock.symbol);
  }

  close(in_fd);
  return 0;
}
