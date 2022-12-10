#include "plane.h"
#include "config.h"
#include "display.h"
#include "gameControl.h"
#include "missile.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEFAULT_FOR_LOOP_INDEX 0
#define DRAW_PLANE 1
#define ERASE_PLANE 0
#define PLANE_WIDTH 20
#define PLANE_HEIGHT (10 / 2)
#define PLANE_DROP_ZONE_START_X (DISPLAY_WIDTH * 2 / 3)
#define PLANE_DROP_ZONE_WIDTH (DISPLAY_WIDTH / 2)
#define PLANE_ORIGIN_X (DISPLAY_WIDTH + PLANE_WIDTH)
#define PLANE_ORIGIN_Y (DISPLAY_HEIGHT / 5 + (rand() % DISPLAY_HEIGHT / 5))
#define PLANE_OFF_SCREEN_X (0)
#define DRAW true
#define ERASE false
#define EXPLODE true
#define DO_NOT_EXPLODE false
#define MISSILE_LAUNCHED true
#define NOT_LAUNCHED false
#define MAX_PLANE_DIFFICULTY 1000
#define CURRENT_PLANE_DIFFICULTY 10
#define ERROR_MESSAGE "*** you're in an unreachable error state ***\n "
#define MISSILE_DEAD_STATE_CODE 4

static missile_t *planeMissile;
static display_point_t origin;
static display_point_t currentPosition;
static uint16_t missileDrop_x_coordinate;
static bool isExploded;
static bool missileLaunched;

// This same structure will be used for all the plane in the game:
//     - Defines the possible states of a plane.
enum planeState {
  INIT,
  FLYING_STATE,
  EXPLODED_STATE,
  DEAD_STATE
} currentPlaneState = INIT;

// Helper function to draw the plane:
static void DrawPlane(display_point_t drawHere, bool drawNotErase) {
  display_drawTriangle(drawHere.x - PLANE_WIDTH, drawHere.y, (drawHere.x),
                       (drawHere.y - PLANE_HEIGHT), (drawHere.x),
                       (drawHere.y + PLANE_HEIGHT),
                       (DISPLAY_WHITE * drawNotErase));
}

// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile) {
  planeMissile = plane_missile;
  missileLaunched = NOT_LAUNCHED;
  currentPosition.x = PLANE_ORIGIN_X;
  currentPosition.y = PLANE_ORIGIN_Y;
  missileDrop_x_coordinate =
      PLANE_DROP_ZONE_START_X - (rand() % PLANE_DROP_ZONE_WIDTH);
  plane_missile->currentState = DEAD_STATE;
}

// State machine tick function
void plane_tick() {
  // Transition & mealy action state machine statement:
  switch (currentPlaneState) {
  case INIT:
    // Move to flying state and initialize the plane and missile;
    currentPlaneState = FLYING_STATE;
    plane_init(planeMissile);
    break;
  case FLYING_STATE:
    // If the plane is hit by an explosion, move to exploded state
    // - Do the same if the plane has flown off the edge of the screen.
    if (isExploded || (currentPosition.x <= PLANE_OFF_SCREEN_X)) {
      currentPlaneState = EXPLODED_STATE;
      DrawPlane(currentPosition, ERASE);
      isExploded = DO_NOT_EXPLODE;
      currentPosition.x = PLANE_ORIGIN_X;
      currentPosition.y = PLANE_ORIGIN_Y;
    }
    break;
  case EXPLODED_STATE:
    // If the plane missile has impacted, move to dead state & wait to respawn
    if (planeMissile->currentState == MISSILE_DEAD_STATE_CODE) {
      currentPlaneState = DEAD_STATE;
      DrawPlane(currentPosition, ERASE);
    }
    break;
  case DEAD_STATE:
    // Respawn after a random length of time if missile is dead
    if ((rand() % MAX_PLANE_DIFFICULTY <= CURRENT_PLANE_DIFFICULTY) &&
        (planeMissile->currentState == MISSILE_DEAD_STATE_CODE)) {
      currentPlaneState = INIT;
    }
    break;
  default:
    printf(ERROR_MESSAGE);
    break;
  }
  // Moore action state machine statement:
  switch (currentPlaneState) {
  case INIT:

    break;
  case FLYING_STATE:
    // Erase old plane
    DrawPlane(currentPosition, ERASE);
    // Update x coordinate
    currentPosition.x -= CONFIG_PLANE_DISTANCE_PER_TICK;
    // Draw new plane
    DrawPlane(currentPosition, DRAW);
    // If the plane is past the drop coordinate, launch a missile:
    if (!missileLaunched && (currentPosition.x <= missileDrop_x_coordinate)) {
      missile_init_plane(planeMissile, currentPosition.x, currentPosition.y);
      missileLaunched = MISSILE_LAUNCHED;
    }
    break;
  case EXPLODED_STATE:
    // No state action
    break;
  case DEAD_STATE:
    // No state action
    break;
  default:
    printf(ERROR_MESSAGE);
    break;
  }
}

// Trigger the plane to expode
void plane_explode() { isExploded = EXPLODE; }

// Get the XY location of the plane
display_point_t plane_getXY() { return currentPosition; }