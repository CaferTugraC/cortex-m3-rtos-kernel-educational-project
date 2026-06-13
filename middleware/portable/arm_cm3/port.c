#include <stddef.h>
#include "port.h"


static PortTickHook_t tick_hook_fn = NULL;
static GetPspFn_t get_psp_fn = NULL;
static SavePspFn_t save_psp_fn = NULL;
static UpdateTaskFn_t update_task_fn = NULL;
static CheckStackOverflowFn_t check_stack_overflow_fn = NULL;

// Wrapper functions
uint32_t port_get_task_psp(void)
{
    if (get_psp_fn != NULL) return get_psp_fn();
    return 0;
}

void port_save_task_psp(uint32_t psp)
{
    if (save_psp_fn != NULL) save_psp_fn(psp);
}

void port_update_next_task(void)
{
    if (update_task_fn != NULL) update_task_fn();
}

// Hook set funcitons
void port_set_tick_hook(PortTickHook_t hook)
{
    tick_hook_fn = hook;
}

void port_set_context_switch_hooks(GetPspFn_t get_psp, CheckStackOverflowFn_t check_stack_overflow, SavePspFn_t save_psp, UpdateTaskFn_t update_task)
{
  get_psp_fn = get_psp;
  check_stack_overflow_fn = check_stack_overflow;
  save_psp_fn = save_psp;
  update_task_fn = update_task;
}

void port_trigger_context_switch(void)
{
  volatile uint32_t *pICSR = (volatile uint32_t*)0xE000ED04U;
	// pend the pendSV exception
	*pICSR |= (1UL << 28);
}

System_Status_t port_init(uint32_t tick_hz, uint32_t cpu_clock)
{
  init_processor_faults();
  
  return init_SysTick_timer(tick_hz, cpu_clock);
}

void init_processor_faults(void)
{
  volatile uint32_t *pSHCSR = (volatile uint32_t*)0xE000ED24U;
  // enable all faults
  *pSHCSR |= (1UL << 16);
  *pSHCSR |= (1UL << 17);
  *pSHCSR |= (1UL << 18);
}

System_Status_t init_SysTick_timer(uint32_t tick_hz, uint32_t cpu_clock)
{
  if (tick_hz == 0U || cpu_clock == 0U) return KERNEL_ERROR_INVALID_PARAM;
  if(tick_hz > CONFIG_MAX_TICK_HZ) return KERNEL_ERROR_INVALID_PARAM;

  volatile uint32_t *pSYST_CSRV = (volatile uint32_t*)0xE000E010U;
  volatile uint32_t *pSYST_RVR = (volatile uint32_t*)0xE000E014U;

  // disable counter
  *pSYST_CSRV &= ~(1UL);

  uint32_t reload_value = (cpu_clock / tick_hz) - 1;
  if (reload_value > 0x00FFFFFFU) return KERNEL_ERROR_INVALID_PARAM;

  *pSYST_RVR &= ~(0x00FFFFFFU);
  *pSYST_RVR |= reload_value;

  // enable systick exception
  *pSYST_CSRV |= (1UL << 1);
  // set clock source
  *pSYST_CSRV |= (1UL << 2);
  // enable counter
  *pSYST_CSRV |= (1UL << 0);
  
  return KERNEL_OK;
}

void SysTick_Handler(void)
{
	if(tick_hook_fn != NULL)  tick_hook_fn();

	if(check_stack_overflow_fn != NULL && check_stack_overflow_fn() != 0)
	{
		return;
	}

	port_trigger_context_switch();
}

// fault handlers

void HardFault_Handler_c(uint32_t *pBaseStackFrame)
{
  while(1)
    {
      
    }
}

void BusFault_Handler_c(uint32_t *pBaseStackFrame)
{
    while(1)
    {
      
    }
}

void MemManage_Handler_c(uint32_t *pBaseStackFrame)
{
    while(1)
    {
      
    }
}

void UsageFault_Handler_c(uint32_t *pBaseStackFrame)
{
    while(1)
    {
      
    }
}