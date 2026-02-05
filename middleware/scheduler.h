/*
 * main.h
 *
 *  Created on: Jan 29, 2026
 *      Author: tura
 */

#ifndef MAIN_H_
#define MAIN_H_

#define MAX_TASKS 5U // min 1 for idle task

#define TICK_HZ 1000U

#define HSI_CLOCK               8000000U
#define SYSTICK_TIM_CLK         HSI_CLOCK

// Task first stack values
#define DUMMY_XPSR 				0x01000000U
#define DUMMY_LR          0xFFFFFFFDU

// Task states
#define TASK_READY_STATE 0x00
#define TASK_BLOCKED_STATE 0xFF

void idle_task_handler(void);
void init_task_stack(void);
void save_psp_value(uint32_t current_psp_value);
void update_next_task(void);
uint32_t get_task_psp_value(void);
void task_delay(uint32_t tick_count);
void sched_add_task(void (*task_handler)(void), uint32_t tsk_stack_addr, uint8_t tsk_stack_size);
void sched_init(void);
void sched_start(void (*task_handler)(void));

typedef struct
{
  uintptr_t psp_value;
  uint32_t block_count;
  uint8_t current_state;
  void (*task_handler)(void);
} TCB_t;

#endif /* MAIN_H_ */
