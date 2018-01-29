// Program to read a text version of stocks such as in stocks.txt and
// write it to a binary file.

#include "header.h"


int main(int argc, char *argv[]){
  if(argc > 1 && strcmp("-h",argv[1])==1){
    printf("usage: %s stocks.txt out.dat\n",argv[0]);
    printf("       cat stocks.txt | %s > out.dat\n",argv[0]);
    return 0;
  }

  FILE *in_file = stdin;                        // read from standard in by default
  if(argc >= 2){                                // if arg is given, read from named file
    in_file = fopen(argv[1], "r");
  }
  int out_fd = STDOUT_FILENO;                   // write to standard output by default
  if(argc >= 3){                                // or write to file if given as an argument
    out_fd = open(argv[2],O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);
  }

  int line_num = 0;
  while(1){                                     // repeat reading text lines and writing binary
    line_num++;
    char line[MAXLINE];
    char *ret = fgets(line,MAXLINE,in_file);    // get a line of text
    if(ret == NULL || strlen(line)==0){         // check for end of input
      break;
    }
    if(line_num==1){                            // skip first line which should contain header
      continue;
    }

    stock_t stock = {};                         // stock to be filled
    str2stock(line, &stock, line_num);          // fill stock from text info
    write(out_fd, &stock, sizeof(stock_t));     // write binary version to a file
  }

  if(argc >=2){
    fclose(in_file);
  }
  if(argc >= 3){
    close(out_fd);
  }
  return 0;
}
