#include <stdio.h>
#include "collatz.h"

#define TESTID "P2"
int failures = 0;

void test_next(int test, int n, int expected){
  int actual = collatz_next(n);
  if(expected != actual){
    printf("Failed %s Test %d\nCall: collatz_next(%d)\nExpect: %d\nActual: %d\n",
	   TESTID,test, n,expected,actual);
    failures++; 
  }
}

#define MAX_SIZE 5096

void test_steps(int test, int n, int expected){
  int actual = collatz_steps(n,0);
  if(expected != actual){
    printf("Failed %s Test %d\nCall: collatz_steps(%d)\nExpect: %d\nActual: %d\n",
	   TESTID,test, n,expected,actual);
    failures++; 
    return;
  }

}

int main(int argc, char **argv){
  int test = 1;
  test_next(test++, 1  , 1   );
  test_next(test++, 0  , -1  );
  test_next(test++, -5 , -1  );
  test_next(test++, 5  , 16  );
  test_next(test++, 16 , 8   );
  test_next(test++, 20 , 10  );
  test_next(test++, 15 , 46  );

  test_steps(test++, -7, 0 );
  test_steps(test++, 1, 0  );
  test_steps(test++, 5, 5  );
  test_steps(test++, 14, 17);
  test_steps(test++, 30, 18);
  test_steps(test++, 22, 15);
  test_steps(test++, 6, 8  );

  if(failures == 0){
    printf("OK\n");
  }
  else{
    printf("FAILURES\n");
  }
  printf("Passed %d / %d tests\n",test-failures,test);

  return 0;
}
