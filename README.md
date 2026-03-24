# Cortex‑M3 RTOS Kernel (STM32F103C8T6) — Minimal Scheduler

A lightweight, preemptive **RTOS kernel/scheduler** designed for ARM **Cortex‑M3** microcontrollers. This project demonstrates the core mechanics of an RTOS: utilizing **SysTick** for timebase, **PendSV** for context switching, and managing tasks via the **Process Stack Pointer (PSP)**.

---

## Table of Contents
- [Features](#features)
- [Hardware Target](#hardware-target)
- [Project Structure](#project-structure)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [Configuration](#configuration)
- [Scheduler & Context Switching](#scheduler--context-switching)
- [Stack Management & Safety](#stack-management--safety)
- [Roadmap / TODO](#roadmap--todo)
- [License](#license)

---

## Features

- **Task Management**: Simple task creation via `sched_add_task()` with custom stack allocation.
- **Automated Idle Task**: An internal idle task is automatically initialized to ensure the CPU always has a valid execution context.
- **Preemptive Scheduling**: Round-robin scheduling triggered by system ticks.
- **Time Management**: 
  - Precise delays via `task_delay_tick()` and `task_delay_ms()`.
- **Efficient Context Switching**:
  - Leverages the **PendSV** exception for switching logic.
  - Manual save/restore of **R4–R11** registers within the handler.
  - Automatic hardware stacking for other registers (R0-R3, R12, LR, PC, xPSR).
- **Stack Overflow Monitoring**:
  - Implements a **Sentinel** approach by writing `0xDEADBEEF` at the stack boundary.
  - System redirects to `UsageFault_Handler` upon detecting a boundary violation.

---

## Hardware Target

- **Core**: ARM Cortex‑M3
- **Tested MCU**: STM32F103C8T6 (Blue Pill)
- **BSP**: Located in `bsp/stm32f103c8t6/`.
- **Drivers**: Includes a register-level LED driver for GPIOA.

---

## Project Structure

- `middleware/`: Core RTOS logic.
  - `scheduler.c/.h`: Task management and kernel state.
  - `portable/arm_cm3/`: Architecture-specific port (SysTick, PendSV, Assembly).
- `common/`: Global definitions and configuration.
  - `scheduler_config.h`: Tuning parameters (Max tasks, Tick Hz).
  - `return_enum.h`: Status and error codes.
- `drivers/`: Hardware abstraction (e.g., LED driver).
- `bsp/`: Startup files and Linker scripts.

---

## Quick Start

### 1. Requirements
- `arm-none-eabi-gcc` toolchain.
- A debugger (OpenOCD/ST-Link).
- Appropriate Linker Script and Startup file (provided in BSP).

### 2. Integration Example
```c
#include "scheduler.h"

// Define task stacks (Must be 8-byte aligned for Cortex-M)
uint32_t task1_stack[128];

void Task1_Handler(void) {
    while(1) {
        LED_Toggle();
        task_delay_ms(500);
    }
}

int main(void) {
    // 1. Initialize Kernel with CPU frequency (e.g., 72MHz)
    sched_init(72000000);

    // 2. Add Tasks
    sched_add_task(Task1_Handler, task1_stack, 128);

    // 3. Start Scheduler
    sched_start();

    return 0; // Should never reach here
}
```

---

## Scheduler & Context Switching

The kernel uses a **PendSV** based switching mechanism to ensure interrupts are not delayed:

1. **SysTick_Handler** increments the global tick and checks for unblocking tasks.
2. If a context switch is required, it pends the **PendSV** bit.
3. The CPU finishes higher-priority ISRs, then enters **PendSV_Handler**.
4. **PendSV_Handler** (Assembly):
   - Saves current task's **R4-R11** to its PSP.
   - Updates the Task Control Block (TCB) with the new PSP.
   - Loads the next task's PSP.
   - Restores **R4-R11** for the next task and exits to the new context.

---

## Configuration

Settings are found in `common/scheduler_config.h`:

| Constant | Description |
| :--- | :--- |
| `MAX_TASKS` | Maximum tasks including the Idle Task. |
| `TICK_HZ` | Frequency of the SysTick (e.g., 1000 for 1ms). |
| `MIN_STACK_SIZE` | Minimum allowed stack size in words. |

---

## Roadmap / TODO

- [ ] Task Priorities (Priority-based Preemption).
- [ ] Synchronization Primitives (Mutexes, Semaphores, Queues).
- [ ] Software Timers.
- [ ] Tickless Idle for Low Power.
- [ ] Dynamic Memory Allocation for Tasks.

---

## License

This project is provided "as-is" for educational purposes. Please refer to the `LICENSE` file for more details.
