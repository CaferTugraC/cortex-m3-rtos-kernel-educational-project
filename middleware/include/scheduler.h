/*
 * main.h
 *
 *  Created on: Jan 29, 2026
 *      Author: tura
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

// Task states
#define TASK_READY_STATE 0x00
#define TASK_BLOCKED_STATE 0xFF


// gloabal API
void sched_add_task(void (*task_handler)(void), uint32_t *tsk_stack_addr, uint16_t tsk_stack_size);
void sched_init(uint32_t clock_source);
void sched_start(void (*task_handler)(void));
void init_idle_task(void);
void task_delay_tick(uint32_t tick_count);
void task_delay_ms(uint32_t ms);


#endif /* SCHEDULER_H */
