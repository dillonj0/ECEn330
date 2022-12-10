/*
WuZuDu Froglems
Weston Smith, Zach Driskill, & Dillon Jensen
Dec. 2022
*/

#ifndef FROG
#define FROG

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LIVES 3
#define FROG_BUTTON_MOVE 24 // Has to be a multiple of 4 or it won't work
#define FROG_TICK_MOVE (FROG_BUTTON_MOVE / 4)

typedef enum { UP, DOWN, LEFT, RIGHT } direction_t;

typedef struct {
  uint16_t x;
  uint16_t y;
} coordinate;

typedef struct {
  uint16_t x_current;
  uint16_t y_current;
  uint16_t x_dest;
  uint16_t y_dest;
  bool alive;
  uint16_t lives;
  uint16_t points;
  direction_t direction;
} frog_t;

typedef enum { RESPAWN, WAITING, MOVING, DEAD, WINNING } frog_state_t;

// Init player object
void frog_init();

void frog_tick();

frog_state_t getFrogState();
void killFrog();
coordinate getFrogLocation();
uint8_t getFrogLives();
uint16_t getFrogPoints();

#endif
