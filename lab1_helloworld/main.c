/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "display.h"

#define TEXT_SIZE 2
#define CURSOR_X 10
#define CURSOR_Y (DISPLAY_HEIGHT / 2)
#define CIRCLE_X (DISPLAY_WIDTH / 4)
#define CIRCLE_RADIUS 25
#define TRIANGLE_X (DISPLAY_WIDTH / 2)
#define TRIANGLE_BASE_X_L (DISPLAY_WIDTH / 2 - 30)
#define TRIANGLE_BASE_X_R (DISPLAY_WIDTH / 2 + 30)
#define TRIANGLE_BASE_Y 30
#define TRIANGLE_POINT_Y ((DISPLAY_HEIGHT / 2) - 30)

// Print out "hello world" on both the console and the LCD screen.
int main() {
  // Initialize display driver, and fill screen with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // Configure display text settings
  display_setTextColor(DISPLAY_RED); // Make the text red.
  display_setTextSize(TEXT_SIZE);    // Make the text a little larger.

  // ~~~~~~~~~~ DRAW LINES & SHAPES ON THE LCD ~~~~~~~~
  // two green lines from corner to opposite corner
  display_drawLine(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_GREEN);
  display_drawLine(0, DISPLAY_HEIGHT, DISPLAY_WIDTH, 0, DISPLAY_GREEN);

  // Draw shapes
  display_drawCircle(CIRCLE_X, CURSOR_Y, CIRCLE_RADIUS, DISPLAY_RED);
  display_fillCircle((DISPLAY_WIDTH - CIRCLE_X), CURSOR_Y, CIRCLE_RADIUS,
                     DISPLAY_RED);
  display_fillTriangle(TRIANGLE_X, TRIANGLE_POINT_Y, TRIANGLE_BASE_X_L,
                       TRIANGLE_BASE_Y, TRIANGLE_BASE_X_R, TRIANGLE_BASE_Y,
                       DISPLAY_YELLOW);
  display_drawTriangle(TRIANGLE_X, (DISPLAY_HEIGHT - TRIANGLE_POINT_Y),
                       TRIANGLE_BASE_X_L, (DISPLAY_HEIGHT - TRIANGLE_BASE_Y),
                       TRIANGLE_BASE_X_R, (DISPLAY_HEIGHT - TRIANGLE_BASE_Y),
                       DISPLAY_YELLOW);
  // ~~~~~~~ DONE DRAWING LINES & SHAPES ON LCD ~~~~~~~

  // Print out a message to console when process has finished.
  printf("Process finished with exit code 0\n");
  return 0;
}
