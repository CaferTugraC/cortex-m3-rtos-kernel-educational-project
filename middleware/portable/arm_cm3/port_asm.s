.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.section .text

.global PendSV_Handler
.global switch_sp_to_psp
.global init_sched_stack
.global interrupt_disable
.global interrupt_enable

.extern _estack
.extern get_task_psp_value
.extern save_psp_value
.extern update_next_task

.type PendSV_Handler, %function
PendSV_Handler:
    MRS R0, PSP
    STMDB R0!, {R4-R11}
    PUSH {LR}
    BL save_psp_value

    BL update_next_task
    BL get_task_psp_value
    LDMIA R0!, {R4-R11}
    MSR PSP, R0
    POP {LR}
    BX LR

.type switch_sp_to_psp, %function
.thumb_func
switch_sp_to_psp:
    PUSH {LR}
    BL get_task_psp_value
    MSR PSP, R0
    POP {LR}

    MRS R0, CONTROL
    ORR R0, R0, #0x2
    MSR CONTROL, R0
    ISB
    BX LR

.type init_sched_stack, %function
.thumb_func
init_sched_stack:
    LDR R0, =_estack
    MSR MSP, R0
    BX LR

.type interrupt_disable, %function
.thumb_func
interrupt_disable:
    CPSID i
    BX LR

.type interrupt_enable, %function
.thumb_func
interrupt_enable:
    CPSIE i
    BX LR

.align 4
.end
