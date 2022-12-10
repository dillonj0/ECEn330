#ifndef CARSROW
#define CARSROW

#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define CAR_LENGTH 24
#define CAR_HEIGHT 15

typedef struct {
  uint8_t rowNum;
  uint8_t numCars;
  int32_t currentState;
  int16_t yVal;
  int16_t xVals[MAX_CARS_IN_ROW];
  int16_t speed;
} cars_row_t;

void cars_row_init(cars_row_t *row, uint8_t rowNumber, uint8_t numCars);

void cars_row_tick(cars_row_t *row);

void cars_row_speed(cars_row_t *row, uint8_t rowNumber);

#endif
