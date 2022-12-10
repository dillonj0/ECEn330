#include "interrupt_test.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"

#include <stdint.h>
#include <stdio.h>
#include <xparameters.h>

#define TIMER_0_PERIOD 0.1 // 10,000,000 cycles = 0.1 s
#define TIMER_1_PERIOD 1   // 100,000,000 cycles = 1 s
#define TIMER_2_PERIOD 10  // 1,000,000,000 cycles = 10 s
#define LED_0_ON 0x1
#define LED_1_ON 0x2
#define LED_2_ON 0x4

// The first timer function; blink a light every 0.1s
static void timer0_isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  // Blink the light
  uint8_t nextLEDValue = leds_read() ^ LED_0_ON;
  leds_write(nextLEDValue);
}

// The second timer function; blink a light every 1s
static void timer1_isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1_INTERRUPT_IRQ);
  // Blink the light
  uint8_t nextLEDValue = leds_read() ^ LED_1_ON;
  leds_write(nextLEDValue);
}

// The third timer function; blink a light every 10s
static void timer2_isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_2_INTERRUPT_IRQ);
  // Blink the light
  uint8_t nextLEDValue = leds_read() ^ LED_2_ON;
  leds_write(nextLEDValue);
}

/*
This function is a small test application of your interrupt controller.  The
goal is to use the three AXI Interval Timers to generate interrupts at different
rates (10Hz, 1Hz, 0.1Hz), and create interrupt handler functions that change the
LEDs at this rate.  For example, the 1Hz interrupt will flip an LED value each
second, resulting in LED that turns on for 1 second, off for 1 second,
repeatedly.

For each interval timer:
    1. Initialize it as a count down timer with appropriate period (1s, 0.5s,
0.25s)
    2. Enable the timer's interrupt output
    3. Enable the associated interrupt input on the interrupt controller.
    4. Register an appropriate interrupt handler function (isr_timer0,
isr_timer1, isr_timer2)
    5. Start the timer

Make sure you call `interrupts_init()` first!
*/
void interrupt_test_run() {
  interrupts_init();
  leds_init();

  // Initialize timer 0
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, timer0_isr);
  interrupts_irq_enable(INTERVAL_TIMER_0);
  intervalTimer_initCountDown(INTERVAL_TIMER_0, TIMER_0_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);

  // Initialize timer 1
  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, timer1_isr);
  interrupts_irq_enable(INTERVAL_TIMER_1);
  intervalTimer_initCountDown(INTERVAL_TIMER_1, TIMER_1_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_1);

  // Initialize timer 2
  interrupts_register(INTERVAL_TIMER_2_INTERRUPT_IRQ, timer2_isr);
  interrupts_irq_enable(INTERVAL_TIMER_2);
  intervalTimer_initCountDown(INTERVAL_TIMER_2, TIMER_2_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_2);
  intervalTimer_start(INTERVAL_TIMER_2);
}