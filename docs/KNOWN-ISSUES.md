# Known Issues

Known issues in the current code. New ones are added here as they are found. For design limitations, see [README.md](../README.md#design-limitations).

**Rules:**

- Every issue has a permanent ID (`KI-01`, `KI-02`, ...). The README links to these IDs.
- A new issue is appended to the end of the list with the next ID. IDs are never reused.
- A fixed issue is not deleted. Its **Status** field is updated to `Fixed (<commit>)`.

---

<a id="ki-01"></a>

## KI-01: Unused TCB slots appear `READY`

- **Status:** Open
- **File:** `middleware/scheduler.c` (`update_next_task()`)

Because `user_tasks[]` is static, it is zero-initialized, and `TASK_READY_STATE` is also `0x00`. However, `update_next_task()` scans up to `MAX_TASKS` instead of `task_count`. If fewer than `MAX_TASKS - 1` tasks are added, the scheduler picks an empty slot with PSP = 0 and `stack_limit` = `NULL`, and the system crashes.

**Workaround:** Set `MAX_TASKS` to exactly "number of user tasks + 1".

<a id="ki-02"></a>

## KI-02: `sched_init()` does not catch port errors

- **Status:** Open
- **File:** `middleware/scheduler.c` (`sched_init()`)

`port_init()` returns `INVALID_PARAM` on failure, e.g. when `TICK_HZ > CONFIG_MAX_TICK_HZ` or when `cpu_clock / TICK_HZ` exceeds 24 bits. However, `sched_init()` only checks for `ERROR_INIT` and returns `OK` even if SysTick was not configured. In that case SysTick stays disabled and after `sched_start()` no user task ever runs; only the idle task runs.

**Workaround:** Make sure `TICK_HZ` does not exceed `CONFIG_MAX_TICK_HZ` and that `clock_source / TICK_HZ` fits in 24 bits.

<a id="ki-03"></a>

## KI-03: SysTick starts before `sched_start()`

- **Status:** Open
- **Files:** `middleware/scheduler.c` (`sched_init()`), `middleware/portable/arm_cm3/port.c` (`init_SysTick_timer()`)

SysTick is enabled inside `sched_init()`, even before the idle task is created. Also, because SysTick's counter register (`SYST_CVR`) is not cleared, it is unpredictable when the first tick arrives. Its reset value is UNKNOWN, and after a soft reset from the debugger it can keep its previous value. As a result:

- If a tick arrives before the idle task is added, `user_tasks[0].stack_limit` is `NULL`, so the overflow check reads address 0x0. The canary does not match and the system halts with a false stack overflow.
- If a tick arrives after the idle task is added but before `sched_start()`, PendSV runs with an uninitialized PSP. The idle task's TCB gets corrupted and `current_task` changes.

**Workaround:** There is no reliable workaround. Keeping the code between `sched_init()` and `sched_start()` short reduces the risk but does not remove it.

<a id="ki-04"></a>

## KI-04: Tick overflow ends delays early

- **Status:** Open
- **File:** `middleware/scheduler.c` (`task_delay_tick()`, `unblock_tasks()`)

If the sum `block_count = g_tick_count + n` exceeds 32 bits, `block_count` wraps around to a small value. This happens when the counter is close to rollover or when a very large `n` is passed. Since `block_count <= g_tick_count` is satisfied immediately, the task wakes up on the next tick, i.e. **too early**. The counter rolls over in ~49.7 days at `TICK_HZ = 1000`.

**Workaround:** Do not pass very large `n` values. Keep this issue in mind if the system runs for longer than ~49.7 days.

<a id="ki-05"></a>

## KI-05: The kernel calls the port layer bypassing `port.h`

- **Status:** Open
- **File:** `middleware/scheduler.c` (`check_task_stack_overflow()`)

The `__asm volatile("BL UsageFault_Handler");` line is ARM-specific and calls a port-layer function by name, bypassing the `port.h` interface. Therefore the kernel cannot be ported to another architecture without changing this line. The inline assembly also declares no clobber list. This is currently harmless because the handler never returns.

<a id="ki-06"></a>

## KI-06: Task stacks are not 8-byte aligned

- **Status:** Open
- **Files:** `app/main.c` (task stacks), `middleware/scheduler.c` (`stack_idle_task`, `sched_add_task()`)

The demo task stacks and `stack_idle_task` are defined without `aligned(8)`, and `sched_add_task()` does not align the top of the stack either. Therefore the compiler may place the arrays at an address where `mod 8 = 4`. You can check this with `arm-none-eabi-nm -n build/final.elf | grep stack`. In that case tasks start with an SP that violates AAPCS, which can cause problems with `double` / `uint64_t` and variadic functions (e.g. `printf`).

**Workaround:** Define your own task stacks with `__attribute__((aligned(8)))` and an even number of words. The idle task's stack requires a code change.

<a id="ki-07"></a>

## KI-07: The LED driver has a race condition

- **Status:** Open
- **File:** `drivers/led.c` (`led_on()`, `led_off()`)

`led_on()` / `led_off()` perform a read-modify-write on `GPIOA_ODR`, and several tasks use this register. If a tick arrives between the read and the write and another task changes ODR, that change is lost. The atomic solution is to use the `GPIOA_BSRR` / `GPIOA_BRR` registers.

<a id="ki-08"></a>

## KI-08: If `TICK_HZ == clock_source`, SysTick never fires

- **Status:** Open
- **File:** `middleware/portable/arm_cm3/port.c` (`init_SysTick_timer()`)

The reload value becomes 0 and this case is not checked. Since SysTick never fires, no user task ever runs.

**Workaround:** Choose a `TICK_HZ` smaller than `clock_source`. This does not happen with the default settings.

<a id="ki-09"></a>

## KI-09: MSP alignment is broken in handlers

- **Status:** Open
- **File:** `middleware/portable/arm_cm3/port_asm.s` (`PendSV_Handler`, `switch_sp_to_psp`)

These two functions push a single register onto the stack (`PUSH {LR}`) and then call C functions. This breaks the 8-byte SP alignment required by AAPCS. It shows no symptoms at the moment because the called C functions do not use 64-bit data. Pushing an even number of registers (e.g. `PUSH {R4, LR}`) keeps the alignment.

<a id="ki-10"></a>

## KI-10: The linker script does not define the initialization sections

- **Status:** Open
- **Files:** `bsp/stm32f103c8t6/stm32f103c8t6_linker_script.ld`, `Makefile`

`.init_array`, `.fini_array` and `.ARM.exidx` are placed as orphans, and `Reset_Handler` does not copy them. `__libc_init_array()` effectively does nothing. This is harmless for C, but C++ constructors would not run. Because `-nostartfiles` is not used, newlib's `crt0.o` is also linked in and occupies ~1.5 KB of RAM in `.data`.

<a id="ki-11"></a>

## KI-11: The build system is incomplete

- **Status:** Open
- **File:** `Makefile`

Header dependencies are not tracked (no `-MMD`), so changing a `.h` file does not recompile the affected `.c` files. The `make load` target only starts the OpenOCD server; it does not flash the board.

**Workaround:** Run `make clean && make` after changing a header. Use the OpenOCD command from the README to flash.

---

Last synced with KNOWN-ISSUES-TR.md: 2026-10-07
