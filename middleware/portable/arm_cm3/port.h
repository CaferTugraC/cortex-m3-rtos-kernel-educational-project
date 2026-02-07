#ifndef PORT_H
#define PORT_H

#include <stdint.h>
#include "return_enum.h"
#include "scheduler_config.h"

typedef void (*PortTickHook_t)(void);


typedef uint32_t (*GetPspFn_t)(void);
typedef System_Status_t (*CheckStackOverflowFn_t)(void);
typedef void (*SavePspFn_t)(uint32_t);
typedef void (*UpdateTaskFn_t)(void);


// global API
System_Status_t init_SysTick_timer(uint32_t tick_hz, uint32_t cpu_clock);
void init_processor_faults(void);
System_Status_t port_init(uint32_t tick_hz, uint32_t cpu_clock);
void port_set_tick_hook(PortTickHook_t hook);
void port_set_context_switch_hooks(GetPspFn_t get_psp, CheckStackOverflowFn_t get_stack_limit, SavePspFn_t save_psp, UpdateTaskFn_t update_task);

void port_trigger_context_switch(void);

// assembly functions
extern void switch_sp_to_psp(void);
extern void init_sched_stack(void);
extern void PendSV_Handler(void);
extern void interrupt_disable(void);
extern void interrupt_enable(void);

#define DUMMY_XPSR 		  0x01000000U
#define DUMMY_LR          0xFFFFFFFDU

#endif