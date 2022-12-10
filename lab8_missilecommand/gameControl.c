/*
Created by Dillon Jensen on 07/NOV/2022 for ECEn 330 (feat. Dr. Goeders & Dr.
Monson)
*/

#include "gameControl.h"
#include "config.h"
#include "display.h"
#include "intervalTimer.h"
#include "missile.h"
#include "plane.h"
#include "touchscreen.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEFAULT_STATS 0
#define MOD_EVEN 2
#define EXIT_LOOP 100
#define DEFAULT_CURSOR_X 10
#define DEFAULT_CURSOR_Y 10
#define DEFAULT_TEXT_SIZE 2
#define TEXT_ARRAY_LENGTH 50
#define GAME_OVER_X (DISPLAY_WIDTH * 2 / 6 - 5)
#define GAME_OVER_Y (DISPLAY_HEIGHT / 2)
#define MAX_IMPACTED_BEFORE_GAME_OVER 5

missile_t enemyMissileArray[CONFIG_MAX_ENEMY_MISSILES];
missile_t playerMissileArray[CONFIG_MAX_PLAYER_MISSILES];
missile_t planeMissileArray[CONFIG_MAX_PLANE_MISSILES];
static uint16_t gameControl_Shot = DEFAULT_STATS;
static uint16_t gameControl_Impacted = DEFAULT_STATS;
static uint16_t previousShot = DEFAULT_STATS;
static uint16_t previousImpacted = DEFAULT_STATS;
static bool tickEven = DEFAULT_STATS;
static bool gameIsOver = DEFAULT_STATS;

// Helper function to print the current stats:
static void updateStats() {
  char previousText[TEXT_ARRAY_LENGTH];
  char newText[TEXT_ARRAY_LENGTH];
  sprintf(previousText, "Shot: %d     Impacted: %d", previousShot,
          previousImpacted);
  sprintf(newText, "Shot: %d     Impacted: %d", gameControl_Shot,
          gameControl_Impacted);

  display_setCursor(DEFAULT_CURSOR_X, DEFAULT_CURSOR_Y);
  display_setTextColor(DISPLAY_BLACK);
  display_print(previousText);

  display_setCursor(DEFAULT_CURSOR_X, DEFAULT_CURSOR_Y);
  display_setTextColor(DISPLAY_WHITE);
  display_print(newText);

  previousImpacted = gameControl_Impacted;
  previousShot = gameControl_Shot;
}

// Helper function to get the x & y current location of a given missile:
static display_point_t GetMissileLocation(missile_t *missile) {
  display_point_t missileLocation;
  missileLocation.x = missile->x_current;
  missileLocation.y = missile->y_current;
  return missileLocation;
}

// Helper function to find the current explosion radius of an exploding missile:
static double GetExplosionRadius(missile_t *missile) { return missile->radius; }

// Helper function to set the explode me flag high
static void SetExplodeMe(missile_t *missile) { missile->explode_me = true; }

// Helper function to see if an enemy missile has impacted the ground.
static bool missileHasImpacted(missile_t *missile) { return missile->impacted; }

// Helper function to set the impacted flag low after acknowledging.
static void ackImpact(missile_t *missile) {
  gameControl_Impacted++;
  missile->impacted = false;
}

// Helper function to clean up missile explosion detection:
static void explodeOrSpareForNow(uint8_t enemy) {
  bool isPlane = false;
  display_point_t currentLocation;
  // Check if we need to explode the selected missile
  // - Is it an enemy or a plane missile?
  if (enemy <= CONFIG_MAX_ENEMY_MISSILES) {
    // Get location if it's flying,
    if (missile_is_flying(&enemyMissileArray[enemy])) {
      currentLocation = GetMissileLocation(&enemyMissileArray[enemy]);
    }
    // Otherwise, do nothing
    else {
      return;
    }
  }
  // Otherwise it's a plane missile, so treat it as such:
  else {
    // get location if it's flying
    if (missile_is_flying(&planeMissileArray[DEFAULT_STATS])) {
      currentLocation = GetMissileLocation(&planeMissileArray[DEFAULT_STATS]);
    }
    // otherwise do nothing
    else {
      return;
    }
    isPlane = true;
  }
  // Compare to exploding player missiles:
  for (uint8_t player = DEFAULT_STATS; player < CONFIG_MAX_PLAYER_MISSILES;
       player++) {
    // Check if the missile is exploding:
    if (missile_is_exploding(&playerMissileArray[player])) {
      // If the enemy missile is within the radius of the explosion, move
      // to expode state.
      display_point_t explodingPlayerLocation =
          GetMissileLocation(&playerMissileArray[player]);
      int16_t x_diff = explodingPlayerLocation.x - currentLocation.x;
      int16_t y_diff = explodingPlayerLocation.y - currentLocation.y;
      int16_t exRadius = GetExplosionRadius(&playerMissileArray[player]);
      if (x_diff * x_diff + y_diff * y_diff <= exRadius * exRadius) {
        // Move enemy to explode state.
        // Is it a plane or an enemy missile?
        if (!isPlane) {
          SetExplodeMe(&enemyMissileArray[enemy]);
        } else {
          SetExplodeMe(&planeMissileArray[DEFAULT_STATS]);
        }
        player = EXIT_LOOP;
      }
    }
  }
  // Compare to exploding enemy missiles:
  for (uint8_t otherEnemy = DEFAULT_STATS;
       otherEnemy < CONFIG_MAX_ENEMY_MISSILES; otherEnemy++) {
    // Check if the missile is exploding:
    if (missile_is_exploding(&enemyMissileArray[otherEnemy])) {
      // If the enemy missile is within the radius of the explosion, move
      // to expode state.
      display_point_t explodingPlayerLocation =
          GetMissileLocation(&enemyMissileArray[otherEnemy]);
      int16_t x_diff = explodingPlayerLocation.x - currentLocation.x;
      int16_t y_diff = explodingPlayerLocation.y - currentLocation.y;
      int16_t exRadius = GetExplosionRadius(&enemyMissileArray[otherEnemy]);
      if (x_diff * x_diff + y_diff * y_diff <= exRadius * exRadius) {
        // Move enemy to explode state.
        // - Is it a plane or an enemy missile?
        if (!isPlane) {
          SetExplodeMe(&enemyMissileArray[enemy]);
        } else {
          SetExplodeMe(&planeMissileArray[DEFAULT_STATS]);
        }
        otherEnemy = EXIT_LOOP;
      }
    }
  }
  // Compare to exploding plane missile:
  if (missile_is_exploding(&planeMissileArray[DEFAULT_STATS])) {
    display_point_t planeMissileLocation =
        GetMissileLocation(&planeMissileArray[DEFAULT_STATS]);
    int16_t x_diff = planeMissileLocation.x - currentLocation.x;
    int16_t y_diff = planeMissileLocation.y - currentLocation.y;
    int16_t exRadius = GetExplosionRadius(&planeMissileArray[DEFAULT_STATS]);
    if (x_diff * x_diff + y_diff * y_diff <= exRadius * exRadius) {
      // Move enemy to explode state.
      // - Is it a plane or an enemy missile?
      if (!isPlane) {
        SetExplodeMe(&enemyMissileArray[enemy]);
      } else {
        SetExplodeMe(&planeMissileArray[DEFAULT_STATS]);
      }
    }
  }
}

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {
  // Blank out the screen, set text size
  display_fillScreen(DISPLAY_BLACK);
  display_setTextSize(DEFAULT_TEXT_SIZE);
  // Initialize all the enemy missiles once:
  for (uint8_t enemy = DEFAULT_STATS; enemy < CONFIG_MAX_ENEMY_MISSILES;
       enemy++) {
    missile_t newMissile;
    enemyMissileArray[enemy] = newMissile;
    missile_init_enemy(&enemyMissileArray[enemy]);
  }
  // Player missiles initialized as dead
  for (uint8_t player = DEFAULT_STATS; player < CONFIG_MAX_PLAYER_MISSILES;
       player++) {
    missile_t newMissile;
    playerMissileArray[player] = newMissile;
    missile_init_dead(&playerMissileArray[player]);
  }
  // Initialize all the plane missiles once:
  for (uint8_t plane = DEFAULT_STATS; plane < CONFIG_MAX_PLANE_MISSILES;
       plane++) {
    missile_t newMissile;
    planeMissileArray[plane] = newMissile;
    missile_init_dead(&planeMissileArray[plane]);
  }

  plane_init(&planeMissileArray[DEFAULT_STATS]);
}

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick() {
  // If the game is over, stop play and print this message:
  if (gameIsOver) {
    display_setCursor(GAME_OVER_X, GAME_OVER_Y);
    display_print("GAME OVER");
  }
  // If the game is not over, execute tick command:
  else {
    // Check if we need to make a new player missile
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      // Create a new player missile if there is one available:
      for (uint8_t player = DEFAULT_STATS; player < CONFIG_MAX_PLAYER_MISSILES;
           player++) {
        // Scan through the player missiles and shoot the first one that is
        // dead:
        if (missile_is_dead(&playerMissileArray[player])) {
          display_point_t missileDestination = touchscreen_get_location();
          missile_init_player(&playerMissileArray[player], missileDestination.x,
                              missileDestination.y);
          player = EXIT_LOOP;
          gameControl_Shot++;
        }
      }
    }
    // Check if an enemy missile needs to explode or has impacted:
    for (uint8_t enemy = DEFAULT_STATS; enemy < CONFIG_MAX_ENEMY_MISSILES;
         enemy++) {
      explodeOrSpareForNow(enemy);
      // Check for impacted missile and update score accordingly
      if (missileHasImpacted(&enemyMissileArray[enemy])) {
        ackImpact(&enemyMissileArray[enemy]);
      }
    }
    // Do the same for plane missile:
    explodeOrSpareForNow(CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLANE_MISSILES);
    if (missileHasImpacted(&planeMissileArray[DEFAULT_STATS])) {
      ackImpact(&planeMissileArray[DEFAULT_STATS]);
    }

    // Tick half of the enemy missiles
    for (uint8_t enemy = DEFAULT_STATS; enemy < CONFIG_MAX_ENEMY_MISSILES;
         enemy++) {
      // Only tick even or odd missiles, alternating each time.
      if (enemy % MOD_EVEN != tickEven) {
        missile_tick(&enemyMissileArray[enemy]);
      }
    }
    // Tick half of the player missiles:
    for (uint8_t player = DEFAULT_STATS; player < CONFIG_MAX_PLAYER_MISSILES;
         player++) {
      // Only tick even or odd missiles, alternating each time.
      if (player % MOD_EVEN != tickEven) {
        missile_tick(&playerMissileArray[player]);
      }
    }
    // Tick the plane:
    plane_tick();
    // If the plane is within the radius of a player explosion, move to dead
    // state & erase
    for (uint8_t player = DEFAULT_STATS; player < CONFIG_MAX_PLAYER_MISSILES;
         player++) {
      // Sort through each player missile and check if it's exploding
      if (missile_is_exploding(&playerMissileArray[player])) {
        // If it's exploding, compare to the plane's position & see if the plane
        // needs to explode
        display_point_t planeLocation = plane_getXY();
        display_point_t explosionLocation =
            GetMissileLocation(&playerMissileArray[player]);
        int16_t x_diff = planeLocation.x - explosionLocation.x;
        int16_t y_diff = planeLocation.y - explosionLocation.y;
        uint8_t radius = GetExplosionRadius(&playerMissileArray[player]);
        // See if the plane is within the explosion radius
        // - If so, explode it & exit loop
        if (x_diff * x_diff + y_diff * y_diff <= radius * radius) {
          plane_explode();
          player = EXIT_LOOP;
        }
      }
    }
    // If the plane is within the radius of an enemy explosion, move to dead
    // state & erase
    for (uint8_t enemy = DEFAULT_STATS; enemy < CONFIG_MAX_ENEMY_MISSILES;
         enemy++) {
      // Sort through each player missile and check if it's exploding
      if (missile_is_exploding(&enemyMissileArray[enemy])) {
        // If it's exploding, compare to the plane's position & see if the plane
        // needs to explode
        display_point_t planeLocation = plane_getXY();
        display_point_t explosionLocation =
            GetMissileLocation(&enemyMissileArray[enemy]);
        int16_t x_diff = planeLocation.x - explosionLocation.x;
        int16_t y_diff = planeLocation.y - explosionLocation.y;
        uint8_t radius = GetExplosionRadius(&enemyMissileArray[enemy]);
        // See if the plane is within the explosion radius
        // - If so, explode it & exit loop
        if (x_diff * x_diff + y_diff * y_diff <= radius * radius) {
          plane_explode();
          enemy = EXIT_LOOP;
        }
      }
    }
    // Tick the plane missile half of the time:
    if (tickEven) {
      missile_tick(&planeMissileArray[DEFAULT_STATS]);
    }

    // Update the stats if necessary:
    if ((previousImpacted != gameControl_Impacted) ||
        (previousShot != gameControl_Shot)) {
      updateStats();
    }

    // If we're past maximum impacted score, go to game over:
    if (gameControl_Impacted >= MAX_IMPACTED_BEFORE_GAME_OVER) {
      gameIsOver = true;
      printf("\n%d missiles impacted:\n Impacted limit reached.\n  *** GAME "
             "OVER ***\n",
             MAX_IMPACTED_BEFORE_GAME_OVER);
    }

    // Alternate whether we're ticking even or odd
    tickEven = !tickEven;
  }
}

// Make dead enemies respawn after a delay
//
/*
// Initialize all the player] missiles once:
    for (uint8_t player = 0; player < CONFIG_MAX_PLAYER_MISSILES; player++){
        missile_t *newMissile;
        playerMissileArray[player] = newMissile;
        missile_init_player(playerMissileArray[player]);
    }
*/