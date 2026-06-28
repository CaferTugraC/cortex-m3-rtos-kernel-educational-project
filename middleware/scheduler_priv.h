#ifndef SCHEDULER_PRIV_H_
#define SCHEDULER_PRIV_H_

#include "scheduler_config.h"
#include <stdint.h>
#include "return_enum.h"

#define STACK_END_VALUE 0xDEADBEEFU

typedef struct
{
  uintptr_t psp_value;
  uint32_t *stack_limit;
  uint32_t block_count;
  uint8_t current_state;
  void (*task_handler)(void);
} TCB_t;

// private API
void idle_task_handler(void);
void save_psp_value(uint32_t current_psp_value);
void update_next_task(void);
void update_global_tick_count(void);
void unblock_tasks(void);
uint32_t get_task_psp_value(void);
System_Status_t check_task_stack_overflow(void);
void sched_tick_handler(void);
System_Status_t schedule(void);


#endif