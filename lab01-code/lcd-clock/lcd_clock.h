#ifndef LCD_CLOCK_H
#define LCD_CLOCK_H 1

/* Global variable used to control the LCD display. Making changes to
   this variable will change the clock time */
extern int LCD_DISPLAY_PORT;

/* Time of day in seconds; set by the hardward, read only */
extern int TIME_OF_DAY_SEC;

/* Convenience struct for representing the LCD clock as characters */
typedef struct {
  char chars[5][32];
} lcd_display;

/* Use the global LCD_DISPLAY_PORT to print the time */
void print_lcd_clock();

/* Print the most signficant (right-most) to least signficant bit in
   the integer passed in */
void showbits(int x);

// Breaks time down into 12-hour format
typedef struct{
  int hours;
  int minutes;
  int seconds;
  int ispm;
} tod_t;

// Breaks down the give time of day in seconds into a tod_t struct
tod_t time_breakdown(int time_of_day);

// Produce an integer which are the bits representing how the LCD
// should display based on the given time of day
int display_bits_from_tod(tod_t tod);

// Update the global variable LCD_DISPLAY_PORT to match the current
// time of day
void lcd_update();

#endif
