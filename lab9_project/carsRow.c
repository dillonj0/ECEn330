#include "carsRow.h"
#include "config.h"
#include "display.h"
#include <stdio.h>

#define ROW_OFFSET 24

typedef enum { CARS_INIT_ST, CARS_MOVE_ST } cars_row_state_type;

void cars_row_init(cars_row_t *row, uint8_t rowNumber, uint8_t numberCars) {
  row->rowNum = rowNumber;
  row->numCars = numberCars;
  row->currentState = CARS_INIT_ST;
  for (uint16_t i = 0; i < numberCars; i++) {
    row->xVals[i] = i * ((DISPLAY_WIDTH + CAR_LENGTH) / numberCars);
  }
  cars_row_speed(row, rowNumber);
  row->yVal = DISPLAY_HEIGHT - (rowNumber * ROW_OFFSET + 20);
}

void cars_row_tick(cars_row_t *row) {
  // printf("Ticking Row\n");
  // printf("Car0 X: %d\n", row->xVals[0]);
  // SM Transitions
  switch (row->currentState) {
  case CARS_INIT_ST:
    row->currentState = CARS_MOVE_ST;
    break;
  case CARS_MOVE_ST:
    break;
  }

  // SM Actions
  switch (row->currentState) {
  case CARS_INIT_ST:
    break;
  case CARS_MOVE_ST:
    for (uint16_t i = 0; i < row->numCars; i++) {
      display_drawRect(row->xVals[i], row->yVal, CAR_LENGTH, CAR_HEIGHT,
                       DISPLAY_BLACK);
      row->xVals[i] += row->speed;
      if (row->speed < 0) {
        if (row->xVals[i] <= -CAR_LENGTH)
          row->xVals[i] = DISPLAY_WIDTH;
      } else {
        if (row->xVals[i] >= DISPLAY_WIDTH + CAR_LENGTH)
          row->xVals[i] = -CAR_LENGTH;
      }
      display_drawRect(row->xVals[i], row->yVal, CAR_LENGTH, CAR_HEIGHT,
                       DISPLAY_RED);
    }
    break;
  }
}

void cars_row_speed(cars_row_t *row, uint8_t rowNumber) {
  if (row->numCars >= 4) {
    row->speed = (rand() % 3) + 1;
  } else if (row->numCars >= 2) {
    row->speed = (rand() % 5) + 2;
  } else {
    row->speed = (rand() % 4) + 7;
  }
  if ((rowNumber == 3) || (rowNumber == 5) || (rowNumber == 7)) {
    row->speed = -1 * row->speed;
  }
}
