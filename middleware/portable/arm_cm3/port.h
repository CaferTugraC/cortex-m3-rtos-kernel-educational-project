#ifndef PORT_H
#define PORT_H

#include <stdint.h>

typedef void (*PortTickHook_t)(void);
typedef void (*PortUnblockHook_t)(void);

typedef uint32_t (*GetPspFn_t)(void);
typedef void (*SavePspFn_t)(uint32_t);
typedef void (*UpdateTaskFn_t)(void);


// global API
void schedule(void);
void init_SysTick_timer(uint32_t tick_hz, uint32_t cpu_clock);
void init_processor_faults(void);
void port_init(uint32_t tick_hz, uint32_t cpu_clock);
void port_set_tick_hook(PortTickHook_t hook);
void port_set_context_switch_hooks(GetPspFn_t get_psp, SavePspFn_t save_psp, UpdateTaskFn_t update_task);

void port_trigger_context_switch(void);

// assembly functions
extern void switch_sp_to_psp(void);
extern void init_sched_stack(void);
extern void PendSV_Handler(void);
extern void interrupt_disable(void);
extern void interrupt_enable(void);


#endif