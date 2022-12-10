/* This code was created by DILLON JENSEN on 9/SEP/2022
   for ECEn 330, Lab 2, "GPIO driver".
*/

#define ALL_ACTIVE 0x0F
#define RECT_WIDTH DISPLAY_WIDTH / 4
#define RECT_HEIGHT DISPLAY_HEIGHT / 2
#define CURSOR_Y DISPLAY_HEIGHT / 4
#define BTN0 0
#define BTN1 1
#define BTN2 2
#define BTN3 3
#define ON 1
#define OFF 0

#include "buttons.h"
#include "display.h"
#include "leds.h"
#include "switches.h"
#include <stdint.h>
#include <stdio.h>

// helper function to draw the rectangles and text on screen for the buttons
static void drawOnScreen(uint8_t buttonValue, uint8_t on);

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void gpioTest_switches() {
  // setup
  leds_init(0);
  switches_init();
  uint8_t ledsValue = 0;

  while (1) {
    leds_write(ledsValue);

    // compare old and new values with an XOR (^)
    ledsValue = switches_read();

    if (ledsValue == ALL_ACTIVE) {
      leds_write(0);
      return;
    }
  }
}

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will run until all 4 pushbuttons
// are simultaneously pressed.
void gpioTest_buttons() {
  buttons_init();
  // Initialize display driver, and fill screen with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  uint8_t buttonsValue = 0;

  while (1) {
    // if there has been a change, print the proper stuff to screen
    uint8_t changedButton =
        buttons_read() ^ buttonsValue; // will have a 1 for the bit that changed
    if (changedButton) {               // a bit has changed

      // update last value to match current value
      buttonsValue = buttons_read();

      // identify which button changed
      uint8_t button;
      switch (changedButton) {
      case BUTTONS_BTN0_MASK:
        button = BTN0;
        break;
      case BUTTONS_BTN1_MASK:
        button = BTN1;
        break;
      case BUTTONS_BTN2_MASK:
        button = BTN2;
        break;
      case BUTTONS_BTN3_MASK:
        button = BTN3;
        break;
      default:
        break;
      }

      // print or erase a rectangle accordingly
      if (buttonsValue & changedButton) {
        // the bit changed to a 1
        drawOnScreen(button, ON);
      } else {
        // the bit changed to a 0
        drawOnScreen(button, OFF);
      }
    }

    // return if all buttons are pressed
    if (buttonsValue == ALL_ACTIVE) {
      display_fillScreen(DISPLAY_BLACK); // Blank the screen.
      // Write a cute farewell message
      display_setCursor(RECT_HEIGHT, RECT_HEIGHT);
      display_print("goodbye\n");
      return;
    }
  }
}

/* This is the function that actually draws the rectangle and
writes the text on the screen. It identifies first the location
of the text and coordinate for the rectangle based off of which
button we've told it has changed. If the button is in the ON
position, the rectangle is drawn. If the button is OFF, the
rectangle gets covered over in black.*/
static void drawOnScreen(uint8_t button, uint8_t on) {
  uint16_t color = DISPLAY_BLACK;
  display_setTextColor(DISPLAY_BLACK);

  // set the coordinates of the rectangle and text to draw
  uint16_t rect_x_coord = DISPLAY_WIDTH - (RECT_WIDTH + (RECT_WIDTH * button));
  uint16_t text_x_coord = rect_x_coord + 10;
  display_setCursor(text_x_coord, CURSOR_Y);

  if (on) {
    display_setTextColor(DISPLAY_WHITE);
    // determine the color of the rectangle & print it
    switch (button) {
    case BTN0:
      color = DISPLAY_DARK_RED;
      display_fillRect(rect_x_coord, 0, RECT_WIDTH, RECT_HEIGHT, color);
      display_print("button 0\n");
      break;
    case BTN1:
      color = DISPLAY_BLUE;
      display_fillRect(rect_x_coord, 0, RECT_WIDTH, RECT_HEIGHT, color);
      display_print("button 1\n");
      break;
    case BTN2:
      color = DISPLAY_DARK_GREEN;
      display_fillRect(rect_x_coord, 0, RECT_WIDTH, RECT_HEIGHT, color);
      display_print("button 2\n");
      break;
    case BTN3:
      color = DISPLAY_DARK_YELLOW;
      display_fillRect(rect_x_coord, 0, RECT_WIDTH, RECT_HEIGHT, color);
      display_print("button 3\n");
      break;
    default:
      display_fillScreen(DISPLAY_BLACK);
      display_print("*** INVALID INPUT ***\n");
      break;
    }
  } else {
    // I.E. on == false, the button has been released.
    // print black text over the old one
    switch (button) {
    case BTN0:
      display_print("button 0\n");
      break;
    case BTN1:
      display_print("button 1\n");
      break;
    case BTN2:
      display_print("button 2\n");
      break;
    case BTN3:
      display_print("button 3\n");
      break;
    default:
      display_fillScreen(DISPLAY_BLACK);
      display_print("*** INVALID INPUT ***\n");
      break;
    }

    // erase the rectangle at the appropriate coordinate
    display_fillRect(rect_x_coord, 0, RECT_WIDTH, RECT_HEIGHT, color);
  }
}