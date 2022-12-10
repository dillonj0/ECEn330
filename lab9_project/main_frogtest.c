#include "buttons.h"
#include "display.h"
#include "frog.h"
#include "gameControl.h"
#include "interrupts.h"
#include "intervalTimer.h"

#define CONFIG_GAME_TIMER_PERIOD 50e-3
#define RUNTIME_TICKS 1500

volatile bool interrupt_flag;

uint32_t isr_triggered_count;
uint32_t isr_handled_count;

void game_isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  interrupt_flag = true;
  isr_triggered_count++;
}

int main() {
  interrupt_flag = false;
  isr_triggered_count = 0;
  isr_handled_count = 0;

  display_init();
  buttons_init();
  gameControl_init();

  // Initialize timer interrupts
  interrupts_init();
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, game_isr);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);

  intervalTimer_initCountDown(INTERVAL_TIMER_0, CONFIG_GAME_TIMER_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);

  // Main game loop
  while (isr_triggered_count < RUNTIME_TICKS) {
    while (!interrupt_flag)
      ;
    interrupt_flag = false;
    isr_handled_count++;

    gameControl_tick();
  }
}
