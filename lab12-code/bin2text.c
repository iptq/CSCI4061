// Read a binary stock file and print its contents as text

#include "header.h"

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s <stock.dat>\n",argv[0]);
    exit(1);
  }

  int in_fd = open(argv[1], O_RDONLY);
  FILE *out_file = stdout;                             // write to standard output
  fprintf(out_file,"%s\n",STOCK_HEADER);               // print header as first line
  int stock_num = 0;                                   // tracks which stock index
  while(1){
    stock_num++;
    stock_t stock = {};
    int nbytes = read(in_fd, &stock, sizeof(stock_t)); // read one stock in binary format
    if(nbytes == 0){                                   // check for end of input
      break;
    }
    char line[MAXLINE];              
    stock2str(&stock, line, MAXLINE);                  // format and print text versions of the stock    
    fprintf(out_file,"%s\n", line);
  }

  close(in_fd);
  return 0;
}
