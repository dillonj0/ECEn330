/*
WuZuDu Froglems
Weston Smith, Zach Driskill, & Dillon Jensen
Dec. 2022
*/

#include "graphics.h"
#include "buttons.h"
#include "display.h"
#include "frog.h"
#include "touchscreen.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define NUMBER_OF_FRAMES 4
// LOGO DISPLAY DEFINITIONS
#define INITIAL_LOGO_X (DISPLAY_WIDTH / 4)
#define INITIAL_LOGO_Y (DISPLAY_HEIGHT * 3 / 7)
#define INITIAL_INSTRUCTION_Y (DISPLAY_HEIGHT / 9 * 5)
#define INITIAL_LOGO_SIZE 2
#define INITIAL_LOGO_TEXT "WuZuDu FROGLEMS"
#define INITIAL_INSTRUCTION "- Press a button -"
#define SCORE_TEXT_Y 5
#define SCORE_TEXT_X 5
#define SCORE_TEXT_SIZE 1
#define LIVES_TEXT_Y 5
#define LIVES_TEXT_X (3 * DISPLAY_WIDTH / 5)
#define LIVES_TEXT_SIZE 1
// BACKDROP DEFINITIONS
#define DIVIDER_1_Y (DISPLAY_HEIGHT / 2)
#define DIVIDER_2_Y (DISPLAY_HEIGHT / 2.5)
#define DIVIDER_3_Y (DISPLAY_HEIGHT / 3.3)
#define FINISH_LINE_Y (DISPLAY_HEIGHT / 5.1)
#define SAFESPACE_Y (DISPLAY_HEIGHT / 2)
#define BOTTOM_1_Y (DISPLAY_HEIGHT * 9 / 10 - 1)
#define BOTTOM_2_Y (DISPLAY_HEIGHT * 7 / 10)
#define BOTTOM_3_Y (DISPLAY_HEIGHT * 8 / 10)
#define BOTTOM_4_Y (DISPLAY_HEIGHT * 6 / 10)
// DRAWING DEFINITIONS
#define FROG_SIZE                                                              \
  0.9 // 1.35 looks the best, but you can adjust this for bigger froglems
#define BODY_RADIUS (5 * FROG_SIZE)
#define LEG_HAND_RADIUS (BODY_RADIUS / 3)
#define LEG_LENGTH (2 * BODY_RADIUS)
#define ARM_LENGTH (LEG_HAND_RADIUS)
#define ERASE false
#define DRAW true
#define LEG_HAND_MOVE_PER_FRAME FROG_TICK_MOVE
#define KILL_FROG_FRAMES 100

typedef struct {
  int8_t xL;
  int8_t xR;
  int8_t yL;
  int8_t yR;
} offset_point_t;

static FrogParts_t next_frog;
static uint8_t frameNumber;
static offset_point_t handOffset;
static offset_point_t footOffset;

// File-specific functions
static void drawLogo(); // Draw our logo at the beginning of the game.
static void drawFrog(frog_t *frog, bool drawNotErase); // Draw or erase the frog
static void
determineXYOffset(frog_t *frog); // Helper in drawing to determine where
                                 //         to put the legs and hands

void graphics_init() {
  drawLogo();
  display_fillScreen(BACKGROUND_COLOR);
  drawBackDrop();

  frameNumber = 0;
}

void graphics_frog_erase(frog_t *frog) { drawFrog(frog, ERASE); };

void graphics_draw_score(uint16_t score, uint16_t oldScore) {
  display_setTextSize(SCORE_TEXT_SIZE);
  char scoreText[20];
  if (score != oldScore) {

    sprintf(scoreText, "SCORE: %d\n", oldScore);
    display_setTextColor(BACKGROUND_COLOR);
    display_setCursor(SCORE_TEXT_X, SCORE_TEXT_Y);
    display_print(scoreText);
  }
  sprintf(scoreText, "SCORE: %d\n", score);
  display_setTextColor(STAGE_COLOR);
  display_setCursor(SCORE_TEXT_X, SCORE_TEXT_Y);
  display_print(scoreText);
}

void graphics_draw_lives(uint8_t lives, uint8_t oldLives) {
  display_setTextSize(LIVES_TEXT_SIZE);
  char scoreText[20];
  if (lives != oldLives) {
    sprintf(scoreText, "LIVES: %d\n", oldLives);
    display_setTextColor(BACKGROUND_COLOR);
    display_setCursor(LIVES_TEXT_X, LIVES_TEXT_Y);
    display_print(scoreText);
  }
  sprintf(scoreText, "LIVES: %d\n", lives);
  display_setTextColor(STAGE_COLOR);
  display_setCursor(LIVES_TEXT_X, LIVES_TEXT_Y);
  display_print(scoreText);
}

void graphics_frog_draw(frog_t *frog) { drawFrog(frog, DRAW); };

static void drawLogo() {
  display_fillScreen(INITIAL_SCREEN_FILL_COLOR);
  display_setTextColor(BACKGROUND_COLOR);
  display_setCursor(INITIAL_LOGO_X, INITIAL_LOGO_Y);
  display_setTextSize(INITIAL_LOGO_SIZE);
  display_print(INITIAL_LOGO_TEXT);
  uint8_t flash_button_press = 0;
  bool drawOrErase = ERASE;
  while (!(buttons_read())) {
    // Tell the user to press a button
    if (!flash_button_press) {
      display_setCursor(INITIAL_LOGO_X - 17, INITIAL_INSTRUCTION_Y);
      uint16_t color = INITIAL_SCREEN_FILL_COLOR;
      if (drawOrErase) {
        color = BACKGROUND_COLOR;
      }
      display_setTextColor(color);
      display_print(INITIAL_INSTRUCTION);
      drawOrErase++;
    }
    flash_button_press++;
  }
}

void drawBackDrop() {
  display_drawLine(DISPLAY_WIDTH, DIVIDER_1_Y, 0, DIVIDER_1_Y, STAGE_COLOR);
  display_drawLine(DISPLAY_WIDTH, DIVIDER_2_Y, 0, DIVIDER_2_Y, STAGE_COLOR);
  display_drawLine(DISPLAY_WIDTH, DIVIDER_3_Y, 0, DIVIDER_3_Y, STAGE_COLOR);
  display_drawLine(DISPLAY_WIDTH, FINISH_LINE_Y, 0, FINISH_LINE_Y, STAGE_COLOR);

  display_fillRect(0, SAFESPACE_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT / 10,
                   STAGE_COLOR);

  display_drawLine(DISPLAY_WIDTH, BOTTOM_1_Y, 0, BOTTOM_1_Y, STAGE_COLOR);
  display_drawLine(DISPLAY_WIDTH, BOTTOM_2_Y, 0, BOTTOM_2_Y, STAGE_COLOR);
  display_drawLine(DISPLAY_WIDTH, BOTTOM_3_Y, 0, BOTTOM_3_Y, STAGE_COLOR);
  display_drawLine(DISPLAY_WIDTH, BOTTOM_4_Y, 0, BOTTOM_4_Y, STAGE_COLOR);
}

static void drawFrog(frog_t *frog, bool drawNotErase) {
  // If we're erasing the frog, draw in BACKGROUND_COLOR
  uint16_t color = BACKGROUND_COLOR;
  if (drawNotErase) {
    color = FROG_COLOR;
  }

  // Determine what frame of the frog we're drawing
  if ((getFrogState() == MOVING)) {
    if (drawNotErase) {
      display_fillRect(0, SAFESPACE_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT / 10,
                       STAGE_COLOR);
      frameNumber++;
      frameNumber = frameNumber % NUMBER_OF_FRAMES;
    }
  } else {
    frameNumber = 0;
  }

  determineXYOffset(frog);
  // Set x values
  next_frog.bodyCenter.x = frog->x_current;
  next_frog.leftHand.x = next_frog.bodyCenter.x + handOffset.xL;
  next_frog.rightHand.x = next_frog.bodyCenter.x + handOffset.xR;
  next_frog.leftLeg.x = next_frog.bodyCenter.x - footOffset.xL;
  next_frog.rightLeg.x = next_frog.bodyCenter.x - footOffset.xR;
  // Set y values
  next_frog.bodyCenter.y = frog->y_current;
  next_frog.leftHand.y = next_frog.bodyCenter.y + handOffset.yL;
  next_frog.rightHand.y = next_frog.bodyCenter.y + handOffset.yR;
  next_frog.leftLeg.y = next_frog.bodyCenter.y - footOffset.yL;
  next_frog.rightLeg.y = next_frog.bodyCenter.y - footOffset.yR;

  // Draw the frog:
  // Body
  display_fillCircle(next_frog.bodyCenter.x, next_frog.bodyCenter.y,
                     BODY_RADIUS, color);
  // Left leg & foot
  display_fillTriangle(next_frog.bodyCenter.x, next_frog.bodyCenter.y,
                       next_frog.leftLeg.x, next_frog.leftLeg.y,
                       next_frog.rightHand.x, next_frog.rightHand.y, color);
  display_fillCircle(next_frog.leftLeg.x, next_frog.leftLeg.y, LEG_HAND_RADIUS,
                     color);
  // Right leg & foot
  display_fillTriangle(next_frog.bodyCenter.x, next_frog.bodyCenter.y,
                       next_frog.rightLeg.x, next_frog.rightLeg.y,
                       next_frog.leftHand.x, next_frog.leftHand.y, color);
  display_fillCircle(next_frog.rightLeg.x, next_frog.rightLeg.y,
                     LEG_HAND_RADIUS, color);
  // Left hand
  display_fillCircle(next_frog.leftHand.x, next_frog.leftHand.y,
                     LEG_HAND_RADIUS, color);
  // Right hand
  display_fillCircle(next_frog.rightHand.x, next_frog.rightHand.y,
                     LEG_HAND_RADIUS, color);
}

// Determine the offset for a hand or foot given the current direction of the
// frog
static void determineXYOffset(frog_t *frog) {
  // *********************** I'M ANTICIPATING HAVING 4 FRAMES OF MOVEMENT
  // *******
  uint8_t frameMultiplier;
  switch (frameNumber) {
  case (1):
    frameMultiplier = 1;
    break;
  case (2):
    frameMultiplier = 2;
    break;
  case (3):
    frameMultiplier = 1;
    break;
  default:
    frameMultiplier = 0;
    break;
  }

  switch (frog->direction) {
  case (DOWN):
    handOffset.xL = (-1) * (BODY_RADIUS + ARM_LENGTH);
    handOffset.xR = BODY_RADIUS + ARM_LENGTH;
    handOffset.yL = frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    handOffset.yR = frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    footOffset.xL = (-1) * BODY_RADIUS;
    footOffset.xR = BODY_RADIUS;
    footOffset.yL =
        LEG_LENGTH + (-1) * frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    footOffset.yR =
        LEG_LENGTH + (-1) * frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    break;
  case (UP):
    handOffset.xL = BODY_RADIUS + ARM_LENGTH;
    handOffset.xR = (-1) * (BODY_RADIUS + ARM_LENGTH);
    handOffset.yL = (-1) * frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    handOffset.yR = (-1) * frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    footOffset.xL = BODY_RADIUS;
    footOffset.xR = (-1) * BODY_RADIUS;
    footOffset.yL =
        (-1) * LEG_LENGTH + frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    footOffset.yR =
        (-1) * LEG_LENGTH + frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    break;
  case (RIGHT):
    handOffset.xL = frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    handOffset.xR = frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    handOffset.yL = (-1) * (BODY_RADIUS + ARM_LENGTH);
    handOffset.yR = BODY_RADIUS + ARM_LENGTH;
    footOffset.xL = LEG_LENGTH - frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    footOffset.xR = LEG_LENGTH - frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    footOffset.yL = (-1) * BODY_RADIUS;
    footOffset.yR = BODY_RADIUS;
    break;
  case (LEFT):
    handOffset.xL = (-1) * frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    handOffset.xR = (-1) * frameMultiplier * LEG_HAND_MOVE_PER_FRAME;
    handOffset.yL = BODY_RADIUS + ARM_LENGTH;
    handOffset.yR = (-1) * (BODY_RADIUS + ARM_LENGTH);
    footOffset.xL =
        (-1) * (LEG_LENGTH - frameMultiplier * LEG_HAND_MOVE_PER_FRAME);
    footOffset.xR =
        (-1) * (LEG_LENGTH - frameMultiplier * LEG_HAND_MOVE_PER_FRAME);
    footOffset.yL = BODY_RADIUS;
    footOffset.yR = (-1) * BODY_RADIUS;
    break;
  default:
    printf("*** Error in GRAPHICS.C determineXYOffset() ***\n");
  }
}

void graphics_kill_frog(frog_t *frog) {
  graphics_frog_erase(frog);
  uint16_t color;
  for (int8_t frame = KILL_FROG_FRAMES; frame >= 0; frame--) {
    color = (DEAD_FROG_COLOR * frame) / KILL_FROG_FRAMES;
    display_fillCircle(next_frog.bodyCenter.x, next_frog.bodyCenter.y,
                       BODY_RADIUS, color);
    // Left leg & foot
    display_fillTriangle(next_frog.bodyCenter.x, next_frog.bodyCenter.y,
                         next_frog.leftLeg.x, next_frog.leftLeg.y,
                         next_frog.rightHand.x, next_frog.rightHand.y, color);
    display_fillCircle(next_frog.leftLeg.x, next_frog.leftLeg.y,
                       LEG_HAND_RADIUS, color);
    // Right leg & foot
    display_fillTriangle(next_frog.bodyCenter.x, next_frog.bodyCenter.y,
                         next_frog.rightLeg.x, next_frog.rightLeg.y,
                         next_frog.leftHand.x, next_frog.leftHand.y, color);
    display_fillCircle(next_frog.rightLeg.x, next_frog.rightLeg.y,
                       LEG_HAND_RADIUS, color);
    // Left hand
    display_fillCircle(next_frog.leftHand.x, next_frog.leftHand.y,
                       LEG_HAND_RADIUS, color);
    // Right hand
    display_fillCircle(next_frog.rightHand.x, next_frog.rightHand.y,
                       LEG_HAND_RADIUS, color);
  }
}