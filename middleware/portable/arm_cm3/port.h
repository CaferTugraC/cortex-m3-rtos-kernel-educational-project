#ifndef PORT_H
#define PORT_H

#include <stdint.h>

void schedule(void);
void init_SysTick_timer(uint32_t tick_hz);
void init_processor_faults(void);

extern void switch_sp_to_psp(void);
extern void init_sched_stack(void);
extern void PendSV_Handler(void);
extern void interrupt_disable(void);
extern void interrupt_enable(void);


#endif