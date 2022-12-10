/*
WuZuDu Froglems
Weston Smith, Zach Driskill, & Dillon Jensen
Dec. 2022
*/

#include "buttons.h"
#include "carsRow.h"
#include "config.h"
#include "frog.h"
#include "graphics.h"
#define NUM_ROWS_FIRST 3
#define NUM_ROWS_SECOND 3
#define NUM_ROWS (NUM_ROWS_FIRST + NUM_ROWS_SECOND)
#define FROG_RADIUS 10
// Variables
uint8_t cars_per_row[6] = {NUM_ROW1_CARS, NUM_ROW2_CARS, NUM_ROW3_CARS,
                           NUM_ROW5_CARS, NUM_ROW6_CARS, NUM_ROW7_CARS};
cars_row_t car_rows[NUM_ROWS];
typedef enum { INTRO, PLAY, GAME_OVER, RESET } game_control_state;
static game_control_state currentState;

void gameControl_init() {
  currentState = PLAY;
  graphics_init();
  frog_init();
  // FIRST ROWS
  for (uint8_t i = 0; i < NUM_ROWS_FIRST; i++) {
    cars_row_init(&car_rows[i], i + 1, cars_per_row[i]);
  }
  for (uint8_t i = NUM_ROWS_FIRST; i < (NUM_ROWS); i++) {
    cars_row_init(&car_rows[i], i + 2, cars_per_row[i]);
  }
  graphics_draw_score(0, 0);
  graphics_draw_lives(0, 0);
}

void gameControl_tick() {
  // Transitions
  switch (currentState) {
  case PLAY:
    if (!getFrogLives()) {
      currentState = GAME_OVER;
      display_fillScreen(DISPLAY_BLUE);
      display_setTextColor(DISPLAY_WHITE);

      display_setCursor(DISPLAY_WIDTH * 7 / 28, DISPLAY_HEIGHT * 4 / 9);
      display_setTextSize(3);
      display_print("GAME OVER\n");

      char scoreText[20];
      sprintf(scoreText, "Final score: %d\n", getFrogPoints());
      display_setCursor(DISPLAY_WIDTH * 6 / 28, DISPLAY_HEIGHT * 5 / 9);
      display_setTextSize(2);
      display_print(scoreText);
    }
    break;
  case GAME_OVER:
    if (buttons_read()) {
      gameControl_init();
      currentState = PLAY;
    }
    break;
  }
  // Actions
  switch (currentState) {
  case PLAY:
    // Ticks

    frog_tick();
    if (getFrogState() == RESPAWN) {
      for (uint8_t i = 0; i < NUM_ROWS_FIRST; i++) {
        cars_row_speed(&car_rows[i], i + 1);
      }
      for (uint8_t i = NUM_ROWS_FIRST; i < (NUM_ROWS); i++) {
        cars_row_speed(&car_rows[i], i + 2);
      }
    }
    for (uint8_t i = 0; i < NUM_ROWS; i++) {
      cars_row_tick(&car_rows[i]);
    }
    // 	Collision Detection
    coordinate temp_frog = getFrogLocation();
    for (uint8_t i = 0; i < NUM_ROWS; i++) {
      for (uint8_t j = 0; j < cars_per_row[i]; j++) {
        int16_t temp_x1 = car_rows[i].xVals[j];
        int16_t temp_x2 = car_rows[i].xVals[j] + CAR_LENGTH;
        int16_t temp_y1 = car_rows[i].yVal;
        int16_t temp_y2 = car_rows[i].yVal + CAR_HEIGHT;
        int16_t frog_left = temp_frog.x - FROG_RADIUS;
        int16_t frog_right = temp_frog.x + FROG_RADIUS;
        int16_t frog_up = temp_frog.y - FROG_RADIUS;
        int16_t frog_down = temp_frog.y + FROG_RADIUS;
        // printf("X1: %d X2: %d Y1: %d Y2: %d\n", temp_x1, temp_x2, temp_y1,
        // temp_y2); printf("Frog L: %d Frog R: %d Frog U: %d Frog D: %d\n",
        // frog_left, frog_right, frog_up, frog_down);

        if (((frog_left < temp_x1) && (frog_right > temp_x1)) &&
            (((frog_up < temp_y1) && (frog_down > temp_y1)) ||
             ((frog_up < temp_y2) && (frog_down > temp_y2)))) {
          killFrog();
          // printf("DEATH\n");
          // printf("X1: %d X2: %d Y1: %d Y2: %d\n", temp_x1, temp_x2, temp_y1,
          // temp_y2); printf("Frog L: %d Frog R: %d Frog U: %d Frog D: %d\n",
          // frog_left, frog_right, frog_up, frog_down);
        } else if (((frog_left < temp_x2) && (frog_right > temp_x2)) &&
                   (((frog_up < temp_y1) && (frog_down > temp_y1)) ||
                    ((frog_up < temp_y2) && (frog_down > temp_y2)))) {
          killFrog();
          // printf("DEATH\n");
          // printf("X1: %d X2: %d Y1: %d Y2: %d\n", temp_x1, temp_x2, temp_y1,
          // temp_y2); printf("Frog L: %d Frog R: %d Frog U: %d Frog D: %d\n",
          // frog_left, frog_right, frog_up, frog_down);
        }
      }
    }
    static uint16_t oldScore = 0;
    static uint8_t oldLives = 0;
    uint16_t tempScore = getFrogPoints();
    uint8_t tempLives = getFrogLives();
    graphics_draw_score(tempScore, oldScore);
    graphics_draw_lives(tempLives, oldLives);
    oldScore = getFrogPoints();
    oldLives = getFrogLives();
    break;
  case GAME_OVER:
    break;
  }

  // 	for(uint8_t i = 0; i < NUM_ROWS; i ++){
  // 		for(uint8_t i = 0; i <
  // 	}
}
