#include "port.h"
#include "scheduler.h"

void schedule(void)
{
	uint32_t *pICSR = (uint32_t*)0xE000ED04U;
	// pend the pendSV exception
	*pICSR |= (1UL << 28);
}

void init_processor_faults(void)
{
  uint32_t *pSHCSR = (uint32_t*)0xE000ED24U;
  // enable all faults
  *pSHCSR |= (1UL << 16);
  *pSHCSR |= (1UL << 17);
  *pSHCSR |= (1UL << 18);
}

void init_SysTick_timer(uint32_t tick_hz)
{
  

  uint32_t *pSYST_CSRV = (uint32_t*)0xE000E010U;
  uint32_t *pSYST_RVR = (uint32_t*)0xE000E014U;

  // disable counter
  *pSYST_CSRV &= ~(1UL);

  uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1; // 1000khz, 8000000U is clock source value
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
	uint32_t *pICSR = (uint32_t*)0xE000ED04U;

	update_global_tick_count();
	unblock_tasks();

	// pend the pendSV exception
	*pICSR |= (1UL << 28);
}

// fault handlers

void HardFault_Handler(void)
{

}

void BusFault_Handler(void)
{
    
}

void MemFault_Handler(void)
{
    
}

void UsageFault_Handler(void)
{
    
}