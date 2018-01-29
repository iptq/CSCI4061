#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd_clock.h"

#define LCD_MAX_BITS 30


/* Global variable used to control the LCD display. Making changes to
   this variable will change the clock time */
int LCD_DISPLAY_PORT;

/* Time of day in seconds; set by the hardware, read only */
int TIME_OF_DAY_SEC;

/* Reset an lcd_display to be empty */
void reset_lcd_display(lcd_display *lcd){
  int i,j;
  for(i=0; i<5; i++){
    for(j=0; j<32; j++){
      lcd->chars[i][j] = ' ';
    }
    lcd->chars[i][31] = '\0';
  }
  lcd->chars[1][12] = 'o';
  lcd->chars[3][12] = 'o';
}  

/* Print an lcd_display */
void print_lcd_display(lcd_display *lcd){
  int i=0;
  for(i=0; i<5; i++){
    printf("%s\n",lcd->chars[i]);
  }
}  

/* Data and functions to set the state of the lcd clock display */

/* Position and char in display */
typedef struct {
  int r,c; char ch;
} charpos;
    
/* Collection of characters corresponding to one bit in the state being set */
typedef struct {
  int len;
  charpos pos[2];
} charpos_coll;

/* Correspondence of bit positions to which characters should be set */
charpos_coll bits2chars[LCD_MAX_BITS] = {
  { .len=2, .pos={{ 0,19,'~'}, { 0,20,'~'},} }, //  0
  { .len=1, .pos={{ 1,21,'|'},             } }, //  1
  { .len=1, .pos={{ 3,21,'|'},             } }, //  2
  { .len=2, .pos={{ 4,19,'~'}, { 4,20,'~'},} }, //  3
  { .len=1, .pos={{ 3,18,'|'},             } }, //  4
  { .len=1, .pos={{ 1,18,'|'},             } }, //  5
  { .len=2, .pos={{ 2,19,'~'}, { 2,20,'~'},} }, //  6

  { .len=2, .pos={{ 0,14,'~'}, { 0,15,'~'},} }, //  7 
  { .len=1, .pos={{ 1,16,'|'},             } }, //  8 
  { .len=1, .pos={{ 3,16,'|'},             } }, //  9 
  { .len=2, .pos={{ 4,14,'~'}, { 4,15,'~'},} }, // 10 
  { .len=1, .pos={{ 3,13,'|'},             } }, // 11 
  { .len=1, .pos={{ 1,13,'|'},             } }, // 12 
  { .len=2, .pos={{ 2,14,'~'}, { 2,15,'~'},} }, // 13 

  { .len=2, .pos={{ 0, 9,'~'}, { 0,10,'~'},} }, // 14   
  { .len=1, .pos={{ 1,11,'|'},             } }, // 15   
  { .len=1, .pos={{ 3,11,'|'},             } }, // 16   
  { .len=2, .pos={{ 4, 9,'~'}, { 4,10,'~'},} }, // 17   
  { .len=1, .pos={{ 3, 8,'|'},             } }, // 18   
  { .len=1, .pos={{ 1, 8,'|'},             } }, // 19   
  { .len=2, .pos={{ 2, 9,'~'}, { 2,10,'~'},} }, // 20   

  { .len=2, .pos={{ 0, 4,'~'}, { 0, 5,'~'},} }, // 21    
  { .len=1, .pos={{ 1, 6,'|'},             } }, // 22    
  { .len=1, .pos={{ 3, 6,'|'},             } }, // 23    
  { .len=2, .pos={{ 4, 4,'~'}, { 4, 5,'~'},} }, // 24    
  { .len=1, .pos={{ 3, 3,'|'},             } }, // 25    
  { .len=1, .pos={{ 1, 3,'|'},             } }, // 26    
  { .len=2, .pos={{ 2, 4,'~'}, { 2, 5,'~'},} }, // 27    

  { .len=2, .pos={{ 3, 0,'A'}, { 3, 1,'M'},} }, // 28
  { .len=2, .pos={{ 4, 0,'P'}, { 4, 1,'M'},} }, // 29
};

/* Assumes ints are at least 32 bits */
void set_lcd_display_state(lcd_display *lcd, int state){
  int i,j;
  int mask = 0x1;
  reset_lcd_display(lcd);
  for(i=0; i<LCD_MAX_BITS; i++){
    if( state & (mask << i) ){ // ith bit set, fill in characters 
      charpos_coll coll = bits2chars[i]; 
      for(j=0; j<coll.len; j+=2){
        charpos pos = coll.pos[j];
        lcd->chars[pos.r][pos.c] = pos.ch;
      }
    }
  }
}


/* Use the global LCD_DISPLAY_PORT to print the time */
void print_lcd_clock(){
  lcd_display lcd;
  set_lcd_display_state(&lcd, LCD_DISPLAY_PORT);
  print_lcd_display(&lcd);
  return;
}


/* Print the most signficant (right-most) to least signficant bit in
   the integer passed in */
#define INT_BITS 32
void showbits(int x){
  int i;
  int mask = 0x1;
  for(i=INT_BITS-1; i>=0; i--){
    int shifted_mask = mask << i;
    if(shifted_mask & x){
      putchar('1');
    } else {
      putchar('0');
    }
  }
  /* Equivalent short version */
  //    (x&(1<<i)) ? putchar('1'): putchar('0');
}
