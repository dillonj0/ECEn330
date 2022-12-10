#include <stdio.h>

#include "armInterrupts.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "touchscreen.h"
#include "utils.h"

#define PERIOD_S 0.01
#define CIRCLE_RADIUS_MAX 50
#define MS_PER_S 1000

volatile bool press_detected = false;
volatile display_point_t point;

static enum {
  TEST_IDLE_ST,
  TEST_TOUCH_PRESSED_ST,
  TEST_TOUCH_RELEASED_ST
} currentState = TEST_IDLE_ST;

uint8_t radius;

// Tick the test code application
void test_tick() {
  // Get the current status of the touchscreen
  touchscreen_status_t status = touchscreen_get_status();

  // Transition
  switch (currentState) {
  case TEST_IDLE_ST:
    if (status != TEST_IDLE_ST) {
      currentState = TEST_TOUCH_PRESSED_ST;

      // New button press detected, draw filled circle
      point = touchscreen_get_location();
      radius = 1;
      printf("x: %hu y: %hu\n", point.x, point.y);
    }
    break;
  case TEST_TOUCH_PRESSED_ST:
    if (status == TOUCHSCREEN_RELEASED)
      currentState = TEST_TOUCH_RELEASED_ST;
    break;
  case TEST_TOUCH_RELEASED_ST:
    currentState = TEST_IDLE_ST;
    break;
  }

  // Action
  switch (currentState) {
  case TEST_TOUCH_PRESSED_ST:
    if (radius < CIRCLE_RADIUS_MAX) {
      display_fillCircle(point.x, point.y, radius, DISPLAY_RED);
      radius++;
    }
    break;
  case TEST_TOUCH_RELEASED_ST:
    // Press released, draw empty circle and acknowledge press
    press_detected = false;
    display_fillCircle(point.x, point.y, radius, DISPLAY_BLACK);
    display_drawCircle(point.x, point.y, radius, DISPLAY_RED);
    touchscreen_ack_touch();
    break;
  }
}

// Interrupt service routine to run FSM ticks
void isr() {
  // Acknowledge timer interrupt
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);

  // Repeatedly tick the touch screen state machine
  touchscreen_tick();
  test_tick();
}

// Touchscreen test program
int main() {
  printf("========== Starting touchscreen test program ==========\n");

  // Initialize drivers
  display_init();
  interrupts_init();
  touchscreen_init(PERIOD_S);

  // Fill screen black
  display_fillScreen(DISPLAY_BLACK);

  // Set up interrupts
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);

  intervalTimer_initCountDown(INTERVAL_TIMER_0, PERIOD_S);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);

  while (1) {
  }
  return 0;
}