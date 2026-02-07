#include <stddef.h>
#include "port.h"


static PortTickHook_t tick_hook_fn = NULL;
static GetPspFn_t get_psp_fn = NULL;
static SavePspFn_t save_psp_fn = NULL;
static UpdateTaskFn_t update_task_fn = NULL;
static CheckStackOverflowFn_t check_stack_overflow_fn = NULL;


// Hook kayıt fonksiyonları
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
  uint32_t *pICSR = (uint32_t*)0xE000ED04U;
	// pend the pendSV exception
	*pICSR |= (1UL << 28);
}

void port_init(uint32_t tick_hz, uint32_t cpu_clock)
{
  init_processor_faults();
  init_SysTick_timer(tick_hz, cpu_clock);

}

void init_processor_faults(void)
{
  uint32_t *pSHCSR = (uint32_t*)0xE000ED24U;
  // enable all faults
  *pSHCSR |= (1UL << 16);
  *pSHCSR |= (1UL << 17);
  *pSHCSR |= (1UL << 18);
}

void init_SysTick_timer(uint32_t tick_hz, uint32_t cpu_clock)
{
  uint32_t *pSYST_CSRV = (uint32_t*)0xE000E010U;
  uint32_t *pSYST_RVR = (uint32_t*)0xE000E014U;

  // disable counter
  *pSYST_CSRV &= ~(1UL);

  uint32_t count_value = (cpu_clock / tick_hz) - 1; // 1000khz, 8000000U is clock source value
  *pSYST_RVR &= ~(0x00FFFFFFFF);
  *pSYST_RVR |= count_value;

  // enable systick exception
  *pSYST_CSRV |= (1UL << 1);
  // set clock source
  *pSYST_CSRV |= (1UL << 2);
  // enable counter
  *pSYST_CSRV |= (1UL << 0);
}

void SysTick_Handler(void)
{
	if(tick_hook_fn != NULL)
  {
    tick_hook_fn();
  }

  if(check_stack_overflow_fn() != 0)
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

void MemFault_Handler_c(uint32_t *pBaseStackFrame)
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