#include <stdint.h>
#include <stdio.h>

#include "scheduler.h"
#include "port.h"

static uint8_t task_count = 0U;
static uint8_t  current_task = 1; // task1 is running
static uint32_t g_tick_count = 0;


static TCB_t user_tasks[MAX_TASKS];


void sched_add_task(void (*task_handler)(void), uint32_t *tsk_stack_addr, uint16_t tsk_stack_size)
{
  uint32_t *p_top_of_stack = &tsk_stack_addr[tsk_stack_size];

  user_tasks[task_count].task_handler = task_handler;
  user_tasks[task_count].current_state = TASK_READY_STATE;
  user_tasks[task_count].block_count = 0U; // default value

  uint32_t *pPSP = p_top_of_stack;

  pPSP--;
  *pPSP = DUMMY_XPSR;	// 0x01000000U

  pPSP--;
  // cppcheck-suppress misra-c2012-11.4	
  *pPSP = (uint32_t)(uintptr_t)user_tasks[task_count].task_handler;	// PC value

  pPSP--;
  *pPSP = DUMMY_LR;	// LR value
  for(uint8_t j = 0U; j < 13U; j++)
  {
	pPSP--;
	*pPSP = 0U;
  }

  user_tasks[task_count].psp_value = (uintptr_t)pPSP;
  task_count++;
}


void sched_init(void)
{
	init_processor_faults();
	init_SysTick_timer(TICK_HZ);
}

void sched_start(void (*start_handler)(void))
{
	switch_sp_to_psp();
	init_sched_stack();
	start_handler();
}

void idle_task_handler(void)
{
  while(1)
	{
		//printf("idle task");
	}
}

/* USEFUL FUNCTIONS START */

void task_delay(uint32_t tick_count)
{
	// disable interrupt
	interrupt_disable();

	if(current_task != 0U)
	{
		user_tasks[current_task].block_count = g_tick_count + tick_count;
		user_tasks[current_task].current_state = TASK_BLOCKED_STATE;
		schedule();
	}

	// enable interrupt
	interrupt_enable();
}

uint32_t get_task_psp_value(void)
{
  return user_tasks[current_task].psp_value;
}

void save_psp_value(uint32_t current_psp_value)
{
  user_tasks[current_task].psp_value = current_psp_value;
}

void update_next_task(void)
{
	int state = TASK_BLOCKED_STATE;

	for(uint8_t i = 0U; i < (MAX_TASKS); i++)
	{
		current_task++;
		current_task = current_task % MAX_TASKS;
		state = user_tasks[current_task].current_state;
		if( (state == TASK_READY_STATE) && (current_task != 0U) )
		{
			break;
		}
	}

	if(state != TASK_READY_STATE)
	{
		current_task = 0;
	}

}

void update_global_tick_count(void)
{
	g_tick_count++;
}

void unblock_tasks(void)
{
	for(uint8_t i = 1U; i < MAX_TASKS; i++)
	{
		if(user_tasks[i].current_state != TASK_READY_STATE)
		{
			if(user_tasks[i].block_count == g_tick_count)
			{
				user_tasks[i].current_state = TASK_READY_STATE;
			}
		}
	}
}

// void init_task_stack(void)
// {
  

//   for(int i = 0 ; i < MAX_TASKS ; i++)
//   {
// 	uint32_t *pPSP;

//     pPSP = user_tasks[i].psp_value;

//     pPSP--;
//     *pPSP = DUMMY_XPSR; // 0x01000000

//     pPSP--;
//     *pPSP = (uint32_t)user_tasks[i].task_handler; // PC value

//     pPSP--;
//     *pPSP = DUMMY_LR; // LR value

//     for(int j = 0; j < 13; j++)
//     {
//     	pPSP--;
//     	*pPSP = 0;
//     }

//     user_tasks[i].psp_value = (uintptr_t)pPSP;
//   }
// }

