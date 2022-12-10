/*
WuZuDu Froglems
Weston Smith, Zach Driskill, & Dillon Jensen
Dec. 2022
*/

#include "frog.h"
#include "buttons.h"
#include "display.h"
#include "graphics.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define POINTS_FOR_WIN 100
#define WIN_LOCATIONS 5
frog_t frog;
frog_state_t currentState;
uint8_t button_value;
// Init player object
uint16_t wins[WIN_LOCATIONS];
void frog_init() {
  frog.x_current = DISPLAY_WIDTH / 2;
  frog.y_current = DISPLAY_HEIGHT - 16;
  frog.alive = true;
  frog.points = 0;
  frog.lives = LIVES;
  display_drawCircle(DISPLAY_WIDTH / 2, 32, 10, DISPLAY_YELLOW);
  display_drawCircle(DISPLAY_WIDTH / 2 - 24 * 3, 32, 10, DISPLAY_YELLOW);
  display_drawCircle(DISPLAY_WIDTH / 2 + 24 * 3, 32, 10, DISPLAY_YELLOW);
  display_drawCircle(DISPLAY_WIDTH / 2 - 24 * 6, 32, 10, DISPLAY_YELLOW);
  display_drawCircle(DISPLAY_WIDTH / 2 + 24 * 6, 32, 10, DISPLAY_YELLOW);
  for (uint8_t i = 0; i < WIN_LOCATIONS; i++) {
    wins[i] = DISPLAY_WIDTH / 2 - 24 * 6 + i * 3 * 24;
  }
}

void frog_tick() {
  // Transtions
  switch (currentState) {
  case RESPAWN:
    currentState = WAITING;
    break;
  case WAITING:
    if (!frog.alive) {
      graphics_frog_erase(&frog);
      currentState = DEAD;
      frog.lives--;
    }
    // printf("Frog waitin'\n");
    button_value = buttons_read();
    // Checks to see if only one button is pressed
    if (button_value && !(button_value & (button_value - 1))) {
      graphics_frog_erase(&frog);
      // Determines the direction
      if (button_value & BUTTONS_BTN1_MASK) {
        frog.direction = UP;
      } else if (button_value & BUTTONS_BTN0_MASK) {
        frog.direction = DOWN;
      } else if (button_value & BUTTONS_BTN3_MASK) {
        frog.direction = LEFT;
      } else if (button_value & BUTTONS_BTN2_MASK) {
        frog.direction = RIGHT;
      }
    } else {
      button_value = 0;
    }
    if (button_value) {
      // Add function to figure out which button is pressed.
      currentState = MOVING;
      if (frog.direction == UP) {
        frog.y_dest = frog.y_current - FROG_BUTTON_MOVE;
        frog.x_dest = frog.x_current;
      } else if (frog.direction == DOWN) {
        frog.y_dest = frog.y_current + FROG_BUTTON_MOVE;
        frog.x_dest = frog.x_current;
      } else if (frog.direction == LEFT) {
        frog.x_dest = frog.x_current - FROG_BUTTON_MOVE;
        frog.y_dest = frog.y_current;
      } else if (frog.direction == RIGHT) {
        frog.x_dest = frog.x_current + FROG_BUTTON_MOVE;
        frog.y_dest = frog.y_current;
      }
    }
    break;
  case MOVING:
    // printf("frog movin'\n");
    if (!frog.alive) {
      graphics_frog_erase(&frog);
      currentState = DEAD;
      frog.lives--;
      drawBackDrop();
    } else if (frog.x_current >= DISPLAY_WIDTH + 10 || frog.x_current <= -10 ||
               frog.y_current >= DISPLAY_HEIGHT + 25) {
      graphics_frog_erase(&frog);
      currentState = DEAD;
      frog.lives--;
      drawBackDrop();
    } else if (frog.y_current <= 32) {
      uint8_t points_multiplier = 1;
      for (uint8_t i = 0; i < WIN_LOCATIONS; i++) {
        if (frog.x_current == wins[i]) {
          points_multiplier = 5;
          display_drawCircle(wins[i], 32, 10, DISPLAY_BLACK);
          wins[i] = 500;
        }
      }
      graphics_frog_erase(&frog);
      frog.points += (POINTS_FOR_WIN * points_multiplier);
      currentState = WINNING;
      drawBackDrop();
    } else if ((frog.x_current == frog.x_dest) &&
               (frog.y_current == frog.y_dest)) {
      currentState = WAITING;
      drawBackDrop();
    }

    break;
  case DEAD:
    if (frog.lives) {
      currentState = RESPAWN;
      drawBackDrop();
    } else {
    }
    break;
  case WINNING:
    currentState = RESPAWN;
    break;
  }
  // Actions
  switch (currentState) {
  case RESPAWN:
    frog.x_current = DISPLAY_WIDTH / 2;
    frog.y_current = DISPLAY_HEIGHT - 16;
    frog.alive = true;
    // printf("DEAD FROG\n");
    // printf("%d", frog.lives);
    frog.x_dest = frog.x_current;
    frog.y_dest = frog.y_current;
    break;
  case WAITING:
    // Draw the frog so it doesn't get deleted when run over
    graphics_frog_draw(&frog);
    break;
  case MOVING:
    // Undraw and Redraw the frog.
    graphics_frog_erase(&frog);

    // printf("            Frog x:\n             current: %d\n             dest:
    // %d\n", frog.x_current, frog.x_dest); printf("Frog y:\n current: %d\n
    // dest: %d\n", frog.y_current, frog.y_dest);

    // Change current
    if (frog.direction == UP) {
      frog.y_current -= FROG_TICK_MOVE;
    } else if (frog.direction == DOWN) {
      frog.y_current += FROG_TICK_MOVE;
    } else if (frog.direction == LEFT) {
      frog.x_current -= FROG_TICK_MOVE;
    } else if (frog.direction == RIGHT) {
      frog.x_current += FROG_TICK_MOVE;
    }
    graphics_frog_draw(&frog);
    break;
  case DEAD:
    graphics_kill_frog(&frog);
    // Do nothing
    break;
  case WINNING:
    break;
  }
  // printf("FROG LIVES %d", frog.lives);
}

frog_state_t getFrogState() { return currentState; }
coordinate getFrogLocation() {
  coordinate temp = {frog.x_current, frog.y_current};
  return temp;
};

void killFrog() { frog.alive = false; }

uint8_t getFrogLives() { return frog.lives; }

uint16_t getFrogPoints() { return frog.points; }