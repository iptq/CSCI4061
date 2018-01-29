#include <string.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "lcd_clock.h"

#define TESTID "lcd_update.c"
int failures = 0;
int testn = 0;

#define TOLERANCE 1e-4

#define GENTEST 0

int compare_tod_t(tod_t x, tod_t y){
  return
    x.hours == y.hours &&
    x.minutes == y.minutes &&
    x.seconds == y.seconds &&
    x.ispm == y.ispm;
}

void print_tod_t(tod_t tod){
  printf("{\n"); 
  printf("  .hours   = %d,\n",tod.hours);
  printf("  .minutes = %d,\n",tod.minutes);
  printf("  .seconds = %d,\n",tod.seconds);
  printf("  .ispm    = %d,\n",tod.ispm);
  printf("}\n");
}

void test_time_breakdown(int time_sec, tod_t expect){
  testn++;
  printf("Running %s Test %d for time_breakdown()\n", TESTID,testn);
  TIME_OF_DAY_SEC = -1;
  LCD_DISPLAY_PORT = -1;

  tod_t actual = time_breakdown(time_sec);

  if(TIME_OF_DAY_SEC != -1 || LCD_DISPLAY_PORT != -1){
    printf("\nFailed %s Test %d for time_breakdown()\n", TESTID,testn);
    printf("Do not modify TIME_OF_DAY_SEC or LCD_DISPLAY_PORT in this function\n");
    failures++;
  }
  else if( !compare_tod_t(actual,expect) ){
    printf("\nFailed %s Test %d for time_breakdown()\n", TESTID,testn);
    printf("Actual and Expected tod_t differ\n");
    printf("time_sec: %d\n",time_sec);
    printf("EXPECT: "); print_tod_t(expect);
    printf("ACTUAL: "); print_tod_t(actual);
    failures++;
  }
  return;
}    

void test_display_bits_from_tod(tod_t tod, int expect){
  testn++;
  printf("Running %s Test %d for display_bits_from_tod()\n", TESTID,testn);
  TIME_OF_DAY_SEC = -1;
  LCD_DISPLAY_PORT = -1;

  int actual = display_bits_from_tod(tod);

  if(TIME_OF_DAY_SEC != -1 || LCD_DISPLAY_PORT != -1){
    printf("\nFailed %s Test %d for display_bits_from_tod()\n", TESTID,testn);
    printf("Do not modify TIME_OF_DAY_SEC or LCD_DISPLAY_PORT in this function\n");
    failures++;
  }
  else if(actual != expect){
    printf("\nFailed %s Test %d for display_bits_from_tod()\n", TESTID,testn);
    printf("Actual and Expected bits differ\n");
    printf("tod = \n"); print_tod_t(tod);
    printf("         3         2         1         0\n");
    printf("index:  10987654321098765432109876543210\n");
    printf("EXPECT: "); showbits(expect); printf("\n");
    printf("ACTUAL: "); showbits(actual); printf("\n");
    printf("guide:     |      |      |      |      |\n");
    printf("\n");
    failures++;
  }
  return;
}

void test_lcd_update(int time_sec, int expect){
  testn++;
  printf("Running %s Test %d for lcd_update()\n", TESTID,testn);
  TIME_OF_DAY_SEC = time_sec;
  lcd_update();
  if(LCD_DISPLAY_PORT != expect || TIME_OF_DAY_SEC != time_sec){
    printf("\nFailed %s Test %d for lcd_update()\n", TESTID,testn);
    printf("Expect TIME_OF_DAY_SEC: %d\n",time_sec);
    printf("Actual TIME_OF_DAY_SEC: %d\n",TIME_OF_DAY_SEC);
    printf("Bits of LCD_DISPLAY_PORT\n");
    printf("index:  10987654321098765432109876543210\n");
    printf("EXPECT: "); showbits(expect); printf("\n");
    printf("ACTUAL: "); showbits(LCD_DISPLAY_PORT); printf("\n");
    printf("guide:     |      |      |      |      |\n");
    printf("\n");
    failures++;
  }
  return;
}


typedef struct {
  int time_sec;
  tod_t tod;
  int bits;
} lcd_test_case;

/* Global array of test data; terminated by a struct with time_sec=-1 */
lcd_test_case tests[] = {
  { .time_sec = 0,
    .tod = {
      .hours   = 12,
      .minutes = 0,
      .seconds = 0,
      .ispm    = 0,
    },
    .bits=0b00010000110101101101111110111111,
  },
  { .time_sec = 101,
    .tod = {
      .hours   = 12,
      .minutes = 1,
      .seconds = 41,
      .ispm    = 0,
    },
    .bits=0b00010000110101101101111110000110,
  },
  { .time_sec = 4170,
    .tod = {
      .hours   = 1,
      .minutes = 9,
      .seconds = 30,
      .ispm    = 0,
    },
    .bits=0b00010000000000011001111111101111,
  },
  { .time_sec = 43199,
    .tod = {
      .hours   = 11,
      .minutes = 59,
      .seconds = 59,
      .ispm    = 0,
    },
    .bits=0b00010000110000011011011011101111,
  },
  { .time_sec = 43200,
    .tod = {
      .hours   = 12,
      .minutes = 0,
      .seconds = 0,
      .ispm    = 1,
    },
    .bits=0b00100000110101101101111110111111,
  },
  { .time_sec = 47089,
    .tod = {
      .hours   = 1,
      .minutes = 4,
      .seconds = 49,
      .ispm    = 1,
    },
    .bits=0b00100000000000011001111111100110,
  },
  { .time_sec = 67089,
    .tod = {
      .hours   = 6,
      .minutes = 38,
      .seconds = 9,
      .ispm    = 1,
    },
    .bits=0b00100000000111110110011111111111,
  },
  { .time_sec = 86399,
    .tod = {
      .hours   = 11,
      .minutes = 59,
      .seconds = 59,
      .ispm    = 1,
    },
    .bits=0b00100000110000011011011011101111,
  },
  { .time_sec = 46348,
    .tod = {
      .hours   = 12,
      .minutes = 52,
      .seconds = 28,
      .ispm    = 1,
    },
    .bits=0b00100000110101101111011011011011,
  },


  { .time_sec = -1,  },
};

int main(int argc, char **argv){
  int i;

  /* Run time_breakdown() tests */
  for(i=0; tests[i].time_sec != -1; i++){
    test_time_breakdown(tests[i].time_sec,tests[i].tod);
  }
    
  /* Run display_bits_from_tod() tests */
  for(i=0; tests[i].time_sec != -1; i++){
    test_display_bits_from_tod(tests[i].tod,tests[i].bits);
  }

  /* Run lcd_update() tests */
  for(i=0; tests[i].time_sec != -1; i++){
    test_lcd_update(tests[i].time_sec,tests[i].bits);
  }

  printf("%s Tests Passed/Run: %d / %d\n",TESTID,(testn-failures),testn);

  return failures;
}
