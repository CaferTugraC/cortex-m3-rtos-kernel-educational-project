#include <stdint.h>
#include "scheduler.h"
#include "scheduler_priv.h"
#include "port.h"

static uint8_t task_count = 1U;
static uint8_t  current_task = 1; // task1 is running
static uint32_t g_tick_count = 0;

extern uint32_t _estack;

uint32_t stack_idle_task[128];
static TCB_t user_tasks[MAX_TASKS];


void sched_init(uint32_t clock_source)
{
	port_set_tick_hook(&sched_tick_handler);
	port_set_context_switch_hooks(
		&get_task_psp_value,
		&save_psp_value,
		update_next_task
	);

	port_init(TICK_HZ, clock_source);
	init_idle_task();
}

void sched_add_task(void (*task_handler)(void), uint32_t *tsk_stack_addr, uint16_t tsk_stack_size)
{
  uint32_t *p_top_of_stack = tsk_stack_addr + tsk_stack_size;

  user_tasks[task_count].task_handler = task_handler;
  user_tasks[task_count].current_state = TASK_READY_STATE;
  user_tasks[task_count].block_count = 0U; // default value

  uint32_t *pPSP = p_top_of_stack;

  // ARM Cortex-M3 stack frame sırası (hardware tarafından push edilenler):
  // R0-R3, R12, LR, PC, xPSR
  
  // xPSR (Program Status Register)
  pPSP--;
  *pPSP = DUMMY_XPSR;	// 0x01000000U

  // PC (Program Counter)
  pPSP--;
  *pPSP = (uint32_t)(uintptr_t)user_tasks[task_count].task_handler;	// PC value

  // LR (Link Register)
  pPSP--;
  *pPSP = DUMMY_LR;	// LR value

  // R12
  pPSP--;
  *pPSP = 0U;

  // R3-R0 (4 registers)
  for(uint8_t j = 0U; j < 4U; j++)
  {
	pPSP--;
	*pPSP = 0U;
  }

  // R11-R4 (8 registers - non-volatile, saved by software in PendSV)
  for(uint8_t j = 0U; j < 8U; j++)
  {
	pPSP--;
	*pPSP = 0U;
  }

  user_tasks[task_count].psp_value = (uintptr_t)pPSP;
  task_count++;
}

void sched_start(void (*start_handler)(void))
{
	switch_sp_to_psp();
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
void init_idle_task(void)
{
	user_tasks[0].task_handler = &idle_task_handler;
	uint32_t *p_top_of_stack = &stack_idle_task[128];
	
	user_tasks[0].current_state = TASK_READY_STATE;
	user_tasks[0].block_count = 0;

	uint32_t *pPSP = p_top_of_stack;

	pPSP--;
  	*pPSP = DUMMY_XPSR;	// 0x01000000U

  	// PC (Program Counter)
  	pPSP--;
  	*pPSP = (uint32_t)(uintptr_t)user_tasks[0].task_handler;	// PC value

  	// LR (Link Register)
  	pPSP--;
  	*pPSP = DUMMY_LR;	// LR value

  	// R12
  	pPSP--;
  	*pPSP = 0U;

  // R3-R0 (4 registers)
  	for(uint8_t j = 0U; j < 4U; j++)
  	{
		pPSP--;
		*pPSP = 0U;
  	}

	// R11-R4 (8 registers - non-volatile, saved by software in PendSV)
	for(uint8_t j = 0U; j < 8U; j++)
	{
		pPSP--;
		*pPSP = 0U;
	}

	user_tasks[0].psp_value = (uintptr_t)pPSP;
}
void sched_tick_handler(void)
{
	update_global_tick_count();
	unblock_tasks();
}

void schedule(void)
{
	port_trigger_context_switch();
}

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

