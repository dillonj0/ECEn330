#include "missile.h"
#include "config.h"
#include "display.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ENEMY_ORIGIN_Y_BOUNDARY                                                \
  4 // ENEMY MISSILES WILL APPEAR IN THE TOP 1/4TH OF THE SCREEN
#define PLAYER_FIRING_ORIGIN_X_1 (DISPLAY_WIDTH / 6)
#define PLAYER_FIRING_ORIGIN_X_2 (DISPLAY_WIDTH / 2)
#define PLAYER_FIRING_ORIGIN_X_3 (5 * DISPLAY_WIDTH / 6)
#define DETECT_PLAYER_FIRING_BOUNDARY_L (DISPLAY_WIDTH / 3)
#define DETECT_PLAYER_FIRING_BOUNDARY_C (2 * DISPLAY_WIDTH / 3)
#define DETECT_PLAYER_FIRING_BOUNDARY_R (DISPLAY_WIDTH)
#define EXPLOSION_MAXIMUM_RADIUS 25
#define PLAYER_MISSILE_COLOR DISPLAY_CYAN
#define ENEMY_MISSILE_COLOR CONFIG_COLOR_ENEMY
#define PLANE_MISSILE_COLOR CONFIG_COLOR_PLANE
#define TICK_MULTIPLIER 2
#define DIFFICULTY_MAX 1000
#define PRESENT_DIFFICULTY 10
#define DEFAULT_START 0

#define ERROR_MESSAGE "You're in an error state\n"

// This same structure will be used for all missiles in the game:
//     - Defines the possible states of a missile.
enum missileState {
  INIT,
  FLYING_STATE,
  EXPLODING_STATE_GROWING,
  EXPLODING_STATE_SHRINKING,
  DEAD_STATE
};

// This is a helper function to initialize any new missile of a given type &
// state
static void init_new_missile(missile_t *missile,
                             missile_type_t new_missile_type,
                             uint32_t initial_state) {
  missile->currentState = initial_state;
  missile->type = new_missile_type;
}

// This helper function will be called to initialize the missile values in the
// INIT state of the tick function
static void init_missile_values(missile_t *missile) {
  uint32_t x_diff = missile->x_origin - missile->x_dest;
  uint32_t y_diff = missile->y_origin - missile->y_dest;
  missile->length = DEFAULT_START;
  missile->explode_me = false;
  missile->total_length = sqrt(x_diff * x_diff + y_diff * y_diff);
  missile->x_current = missile->x_origin;
  missile->impacted = false;
}

// A helper function to determine what color of line you need to draw
static uint16_t missile_color(missile_t *missile) {
  // - Look at the missile type and return the color of that type
  if (missile->type == MISSILE_TYPE_ENEMY) {
    return ENEMY_MISSILE_COLOR;
  } else if (missile->type == MISSILE_TYPE_PLAYER) {
    return PLAYER_MISSILE_COLOR;
  } else {
    return PLANE_MISSILE_COLOR;
  }
}

// A helper function to determine how far the missile is supposed to move per
// tick based on its type
static uint16_t missile_move_per_tick(missile_t *missile) {
  // - Look at the missile type and return the color of that type
  if (missile->type == MISSILE_TYPE_ENEMY) {
    return TICK_MULTIPLIER * CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
  } else if (missile->type == MISSILE_TYPE_PLAYER) {
    return TICK_MULTIPLIER * CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
  } else {
    return TICK_MULTIPLIER * CONFIG_PLANE_DISTANCE_PER_TICK;
  }
}

////////// State Machine INIT Functions //////////
// Unlike most state machines that have a single `init` function, our missile
// will have different initializers depending on the missile type.

// Initialize the missile as a dead missile.  This is useful at the start of the
// game to ensure that player and plane missiles aren't moving before they
// should.
void missile_init_dead(missile_t *missile) {
  missile_type_t newMissileType = missile->type;
  init_new_missile(missile, newMissileType, DEAD_STATE);
}

// Initialize the missile as an enemy missile.  This will randomly choose the
// origin and destination of the missile.  The origin should be somewhere near
// the top of the screen, and the destination should be the very bottom of the
// screen.
void missile_init_enemy(missile_t *missile) {
  init_new_missile(missile, MISSILE_TYPE_ENEMY, INIT);
  missile->x_origin = rand() % DISPLAY_WIDTH;
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_origin = (rand() % DISPLAY_HEIGHT) / ENEMY_ORIGIN_Y_BOUNDARY;
  missile->y_dest = DISPLAY_HEIGHT;
  missile->impacted = false;
  missile->explode_me = false;
}

// Initialize the missile as a player missile.  This function takes an (x, y)
// destination of the missile (where the user touched on the touchscreen).  The
// origin should be the closest "firing location" to the destination (there are
// three firing locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest) {
  init_new_missile(missile, MISSILE_TYPE_PLAYER, INIT);
  // Determine which station we're going to fire from
  // - If less than less boundary, shoot from station 1
  // - Else if less than center boundary, shoot from station 2
  // - Else shoot from station three (right station)
  if (x_dest < DETECT_PLAYER_FIRING_BOUNDARY_L) {
    missile->x_origin = PLAYER_FIRING_ORIGIN_X_1;
  } else if (x_dest < DETECT_PLAYER_FIRING_BOUNDARY_C) {
    missile->x_origin = PLAYER_FIRING_ORIGIN_X_2;
  } else {
    missile->x_origin = PLAYER_FIRING_ORIGIN_X_3;
  }
  missile->y_origin = DISPLAY_HEIGHT;
  missile->x_dest = x_dest;
  missile->y_dest = y_dest;
}

// Initialize the missile as a plane missile.  This function takes an (x, y)
// location of the plane which will be used as the origin.  The destination can
// be randomly chosed along the bottom of the screen.
void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y) {
  init_new_missile(missile, MISSILE_TYPE_PLANE, INIT);
  missile->x_origin = plane_x;
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_origin = plane_y;
  missile->y_dest = DISPLAY_HEIGHT;
}

////////// State Machine TICK Function //////////
void missile_tick(missile_t *missile) {
  // Transition state machine switch statement
  switch (missile->currentState) {
  case INIT:
    // Initialize the missile values and then go to FLYING
    init_missile_values(missile);
    missile->currentState = FLYING_STATE;
    break;
  case FLYING_STATE:
    // If the missile is at the destination:
    // - erase the old line and move to exploding state.
    // - - For enemies, erase and move to dead state, set impacted = true
    // - - For players, move to exploding state.
    if (missile->length * TICK_MULTIPLIER >= missile->total_length) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_BLACK);
      // If it's an enemy, kill it
      if ((missile->type == MISSILE_TYPE_ENEMY) ||
          (missile->type == MISSILE_TYPE_PLANE)) {
        missile->impacted = true;
        missile->currentState = DEAD_STATE;
      }
      // If it's a player, explode.
      if (missile->type == MISSILE_TYPE_PLAYER) {
        missile->currentState = EXPLODING_STATE_GROWING;
        missile->x_current = missile->x_dest;
        missile->y_current = missile->y_dest;
      }
    }
    // If the missile explode flag is high, explode it no matter what
    if (missile->explode_me) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_BLACK);
      missile->explode_me = false;
      missile->currentState = EXPLODING_STATE_GROWING;
      missile->x_dest = missile->x_current;
      missile->y_dest = missile->y_current;
    }
    break;
  case EXPLODING_STATE_GROWING:
    // If the explode radius is at the max, move to shrinking state.
    if (missile->radius >= EXPLOSION_MAXIMUM_RADIUS) {
      display_fillCircle(missile->x_dest, missile->y_dest, missile->radius,
                         DISPLAY_BLACK);
      missile->currentState = EXPLODING_STATE_SHRINKING;
    }
    break;
  case EXPLODING_STATE_SHRINKING:
    // If the explode radius is at the min, move to dead state.
    if (missile->radius <= DEFAULT_START) {
      display_fillCircle(missile->x_dest, missile->y_dest, missile->radius,
                         DISPLAY_BLACK);
      missile->currentState = DEAD_STATE;
    }
    break;
  case DEAD_STATE:
    // If the missile is an enemy missile,
    if (missile->type == MISSILE_TYPE_ENEMY) {
      if (rand() % DIFFICULTY_MAX <= PRESENT_DIFFICULTY) {
        missile_init_enemy(missile);
      }
    }
    break;
  default:
    printf(ERROR_MESSAGE);
    break;
  }
  // State action switch statement
  switch (missile->currentState) {
  case INIT:
    break;
  case FLYING_STATE:
    // Erase the old line, update missile location, draw new line.
    missile->length += missile_move_per_tick(missile);
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, DISPLAY_BLACK);
    missile->x_current = missile->x_origin +
                         TICK_MULTIPLIER *
                             ((double)missile->length / missile->total_length) *
                             (missile->x_dest - missile->x_origin);
    missile->y_current = missile->y_origin +
                         TICK_MULTIPLIER *
                             ((double)missile->length / missile->total_length) *
                             (missile->y_dest - missile->y_origin);
    if (!(missile->length * TICK_MULTIPLIER >= missile->total_length)) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, missile_color(missile));
    }
    break;
  case EXPLODING_STATE_GROWING:
    // Erase old circle, then update radius and draw new one.
    display_fillCircle(missile->x_dest, missile->y_dest, missile->radius,
                       DISPLAY_BLACK);
    missile->radius +=
        TICK_MULTIPLIER * CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
    display_fillCircle(missile->x_dest, missile->y_dest, missile->radius,
                       missile_color(missile));
    break;
  case EXPLODING_STATE_SHRINKING:
    // Erase old circle, then update radius and draw new one.
    display_fillCircle(missile->x_dest, missile->y_dest, missile->radius,
                       DISPLAY_BLACK);
    missile->radius -=
        TICK_MULTIPLIER * CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
    display_fillCircle(missile->x_dest, missile->y_dest, missile->radius,
                       missile_color(missile));
    break;
  case DEAD_STATE:
    break;
  default:
    printf(ERROR_MESSAGE);
    break;
  }
}

// Return whether the given missile is dead.
bool missile_is_dead(missile_t *missile) {
  return (missile->currentState == DEAD_STATE);
}

// Return whether the given missile is exploding.  This is needed when detecting
// whether a missile hits another exploding missile.
bool missile_is_exploding(missile_t *missile) {
  return ((missile->currentState == EXPLODING_STATE_GROWING) ||
          (missile->currentState == EXPLODING_STATE_SHRINKING));
}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile) {
  return (missile->currentState == FLYING_STATE);
}

// Used to indicate that a flying missile should be detonated.  This occurs when
// an enemy or plane missile is located within an explosion zone.
void missile_trigger_explosion(missile_t *missile) {
  missile->explode_me = true;
}
