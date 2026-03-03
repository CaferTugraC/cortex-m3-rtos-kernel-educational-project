#include "scheduler.h"
#include "scheduler_priv.h"
#include "port.h"


static volatile uint8_t task_count = 0U;
static volatile uint8_t  current_task = 0;
static volatile uint32_t g_tick_count = 0;

extern uint32_t _estack;

uint32_t stack_idle_task[128];
static TCB_t user_tasks[MAX_TASKS];


System_Status_t sched_init(uint32_t clock_source)
{
	if (clock_source == 0U) return INVALID_PARAM;
	port_set_tick_hook(&sched_tick_handler);
	port_set_context_switch_hooks(
		&get_task_psp_value,
		&check_task_stack_overflow,
		&save_psp_value,
		&update_next_task
	);

	if(port_init(TICK_HZ, clock_source) == ERROR_INIT) return ERROR_INIT;
	
	return init_idle_task();
}

System_Status_t sched_add_task(void (*task_handler)(void), uint32_t *tsk_stack_addr, uint16_t tsk_stack_size)
{
  if (task_count >= MAX_TASKS)
  {
	  /* Error: reached max task value. */
	  return REACHED_MAX_TASK;
  }
  if (task_handler == NULL || tsk_stack_addr == NULL) return INVALID_PARAM;
  if (tsk_stack_size < MIN_STACK_SIZE) return INVALID_PARAM;

  // set STACK_END_VALUE to end of stack for stackowerflow protection.
  tsk_stack_addr[MIN_STACK_FRAME_SIZE] = STACK_END_VALUE;
  user_tasks[task_count].stack_limit = &tsk_stack_addr[MIN_STACK_FRAME_SIZE];


  uint32_t *p_top_of_stack = tsk_stack_addr + tsk_stack_size;

  user_tasks[task_count].task_handler = task_handler;
  user_tasks[task_count].current_state = TASK_READY_STATE;
  user_tasks[task_count].block_count = 0U; // default value

  uint32_t *pPSP = p_top_of_stack;

  // ARM Cortex-M3 stack frame:
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
  return OK;
}

void sched_start()
{
	switch_sp_to_psp();
	user_tasks[current_task].task_handler();
}

void idle_task_handler(void)
{
  while(1)
	{
		//printf("idle task");
	}
}

uint32_t sched_get_tick(void)
{
	return g_tick_count;
}

System_Status_t init_idle_task(void)
{
	return sched_add_task(&idle_task_handler, &stack_idle_task[0], 128);
}

void sched_tick_handler(void)
{
	update_global_tick_count();
	unblock_tasks();
}

System_Status_t schedule(void)
{
	if(check_task_stack_overflow() != 0)
	{
		return ERROR_STACK_OVERFLOW;
	}
	
	port_trigger_context_switch();
	return OK;
}

void task_delay_tick(uint32_t tick_count)
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

void task_delay_ms(uint32_t ms)
{
	task_delay_tick((ms * TICK_HZ) / 1000U);
}

uint32_t get_task_psp_value(void)
{
  return user_tasks[current_task].psp_value;
}

System_Status_t check_task_stack_overflow(void)
{
	uint32_t *pStack = (uint32_t*)user_tasks[current_task].stack_limit;

	if(*pStack != STACK_END_VALUE || (uint32_t)pStack > user_tasks[current_task].psp_value)
	{
		__asm volatile("BL UsageFault_Handler");
		return ERROR_STACK_OVERFLOW; // stack owerflow danger!
	}
	return OK;
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
			if(user_tasks[i].block_count <= g_tick_count)
			{
				user_tasks[i].current_state = TASK_READY_STATE;
			}
		}
	}
}
