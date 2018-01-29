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

#define MAXLINE       4096      // max length of lines

// stock structure
typedef struct {
  char   symbol[16];            // ticker symbol like GOOG                     
  double open;                  // price at market open
  double close;                 // price at market close
  long   volume;                // quantity of shares exchanged during day
  char   name[128];             // name of company 
} stock_t;

// header for text-based stock files
const char *STOCK_HEADER = "Symbol	Open	Close	Volume	Name";

#define MAX_FIELDS    16        // maximum number of fields allowed in parse_into_tokens()

// The following function should really be in their own C file but it
// is sufficient to include them in the header for this small lab example.

// Parse the contents of input_command so that tokens[i] will point to
// the ith space-separated string in it. Set ntok to the number of
// tokens that are found.
void parse_into_tokens(char input_command[], char *tokens[], int *ntok, char *seps){
  int i = 0;
  char *tok = strtok(input_command,seps);
  while(tok!=NULL && i<MAX_FIELDS){
    tokens[i] = tok;            // assign tokens to found string
    i++;
    tok = strtok(NULL,seps);
  }
  tokens[i] = NULL;             // null terminate tokens to ease argv[] work
  *ntok = i;
  return;
}

// Trim all spaces off the end of a string by replacing them with
// nulls. Returns the new length of the string.
int trim(char *str){
  int last = strlen(str);
  int idx = last-1;
  while(idx>=0 && str[idx]==' '){
    str[idx] = '\0';
    idx--;
  }
  return idx+1;
}

// The string buf contains a text representation of a stock which
// should be parsed to fill the fields of argument stock with its
// fields.  line_num may refer to a line number from a file to help
// diagnose bugs. Assumes that stock fields are separated by TABs in
// the string buf.
void str2stock(char buf[], stock_t *stock, int line_num){
  char *fields[MAX_FIELDS];
  int nfields = -1;
  parse_into_tokens(buf, fields, &nfields,"\t\n");    // parse based on tab separator

  strcpy(stock->symbol,fields[0]);              // char   symbol[16];   // ticker symbol like GOOG                     
  sscanf(fields[1], "%lf", &(stock->open));     // double open;         // price at market open                        
  sscanf(fields[2], "%lf", &(stock->close));    // double close;        // price at market close                       
  sscanf(fields[3], "%ld", &(stock->volume));   // long   volume;       // quantity of shares exchanged during day     
  strcpy(stock->name,fields[4]);                // char   name[128];    // name of company                             

  trim(stock->symbol);                          // get rid of whitespace at the end of
  trim(stock->name);                            // the string fields
}

// Format the fields of stock to create a string representation of it
// in buf. Separate each field with a TAB character. Respects maximum
// size buf_max and will not overflow it.  Returns a pointer to buf so
// it can be used in one-liners like
//
//   fprintf(out_file,"%s\n", stock2str(&stock, line, MAX_LINE))
char *stock2str(stock_t *stock, char buf[], int buf_max){
  int off = 0;
  off += snprintf(buf+off, buf_max-off, "%s\t",stock->symbol);   // char   symbol[16]; // ticker symbol like GOOG                     
  off += snprintf(buf+off, buf_max-off, "%.2f\t",stock->open);   // double open;       // price at market open                        
  off += snprintf(buf+off, buf_max-off, "%.2f\t",stock->close);  // double close;      // price at market close                       
  off += snprintf(buf+off, buf_max-off, "%ld\t",stock->volume);  // long   volume;     // quantity of shares exchanged during day     
  off += snprintf(buf+off, buf_max-off, "%s\t",stock->name);     // char   name[128];  // name of company                             
  buf[off-1]='\0';
  return buf;
}
