/*
 * scheduler.h
 *
 *  Created on: Jan 29, 2026
 *      Author: tura
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stddef.h>
#include "return_enum.h"

#define CONFIG_MAX_TICK_HZ 5000U
#define MIN_STACK_SIZE 64U
#define MIN_STACK_FRAME_SIZE 16U  // 16 words (R0-R3, R4-R11, R12, LR, PC, xPSR), do not change, its critical for safe of stack overflow


// Task states
#define TASK_READY_STATE 0x00
#define TASK_BLOCKED_STATE 0xFF


// gloabal API
System_Status_t sched_add_task(void (*task_handler)(void), uint32_t *tsk_stack_addr, uint16_t tsk_stack_size);
System_Status_t sched_init(uint32_t clock_source);
void sched_start(void (*task_handler)(void));
System_Status_t init_idle_task(void);
void task_delay_tick(uint32_t tick_count);
void task_delay_ms(uint32_t ms);


#endif /* SCHEDULER_H */
