#include "lcd_clock.h"


// Breaks down the give time of day in seconds into a tod_t struct
tod_t time_breakdown(int time_of_day_sec){
  int tod_hours = time_of_day_sec / 3600;
  int ispm = tod_hours >= 12 ? 1 : 0;
  tod_hours = tod_hours % 12;
  tod_hours = tod_hours == 0 ? 12 : tod_hours;
  int tod_minutes = (time_of_day_sec / 60) % 60;
  int tod_seconds = time_of_day_sec % 60;
  tod_t tod = {.hours = tod_hours,
               .minutes = tod_minutes,
               .seconds = tod_seconds,
               .ispm = ispm};
  return tod;
}
  
// Binary masks for each digit for ease of setting
int digit_masks[10] = {
  0b0111111,                     // zero
  0b0000110,                     // one 
  0b1011011,                     // two 
  0b1001111,                     // three 
  0b1100110,                     // four 
  0b1101101,                     // five 
  0b1111101,                     // six 
  0b0000111,                     // seven 
  0b1111111,                     // eight 
  0b1101111,                     // nine 
};

#define BLANK   0b0000000                     // blank
#define AM_MASK (0x1 << 28)
#define PM_MASK (0x1 << 29)

int display_bits_from_tod(tod_t tod){
  int hr_tens = tod.hours >= 10 ? digit_masks[1] : BLANK;
  int hr_ones = digit_masks[tod.hours % 10];
  int mn_tens = digit_masks[tod.minutes / 10];
  int mn_ones = digit_masks[tod.minutes % 10];
  int ampm = tod.ispm ? PM_MASK : AM_MASK;

  int state = 0;
  state |= (mn_ones <<  0);
  state |= (mn_tens <<  7);
  state |= (hr_ones << 14);
  state |= (hr_tens << 21);
  state |= ampm;

  return state;
}

/* Examine the TIME_OF_DAY_SEC to determine hour, minute, and am/pm.
   Set the LCD_DISPLAY_PORT bits to show the proper time. */
void lcd_update(){
  tod_t tod = time_breakdown(TIME_OF_DAY_SEC);
  int state = display_bits_from_tod(tod);
  LCD_DISPLAY_PORT = state;
}
