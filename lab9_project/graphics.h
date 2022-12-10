/*
WuZuDu Froglems
Weston Smith, Zach Driskill, & Dillon Jensen
Dec. 2022
*/

#ifndef GRAPHICS
#define GRAPHICS

#include "display.h"
#include "frog.h"
#include <stdio.h>

#define INITIAL_SCREEN_FILL_COLOR DISPLAY_WHITE
#define BACKGROUND_COLOR DISPLAY_BLACK
#define FROG_COLOR 0x07E0
#define DEAD_FROG_COLOR DISPLAY_RED
#define STAGE_COLOR DISPLAY_LIGHT_GRAY

typedef struct {
  display_point_t leftLeg;
  display_point_t rightLeg;
  display_point_t leftHand;
  display_point_t rightHand;
  display_point_t bodyCenter;
} FrogParts_t;

void graphics_init();
void graphics_frog_erase(frog_t *frog);
void graphics_frog_draw(frog_t *frog);
void graphics_kill_frog(frog_t *frog);
void graphics_draw_score(uint16_t score, uint16_t oldScore);
void graphics_draw_lives(uint8_t lives, uint8_t oldLives);
void drawBackDrop(); // Draw the objects in the background of the game.

#endif