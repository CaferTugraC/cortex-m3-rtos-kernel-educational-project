#ifndef SCHEDULER_PRIV_H
#define SCHEDULER_PRIV_H

#include "scheduler_config.h"

// private API
void idle_task_handler(void);
void init_task_stack(void);
void save_psp_value(uint32_t current_psp_value);
void update_next_task(void);
void update_global_tick_count(void);
void unblock_tasks(void);
uint32_t get_task_psp_value(void);
void sched_tick_handler(void);
void schedule(void);



typedef struct
{
  uintptr_t psp_value;
  uint32_t block_count;
  uint8_t current_state;
  void (*task_handler)(void);
} TCB_t;

#endif