#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stddef.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>

#define MAXLINE       4096      // max length of lines

// stock structure
typedef struct {
  char   symbol[16];            // ticker symbol like GOOG                     
  double open;                  // price at market open
  double close;                 // price at market close
  long   volume;                // quantity of shares exchanged during day
  char   name[128];             // name of company 
} stock_t;
