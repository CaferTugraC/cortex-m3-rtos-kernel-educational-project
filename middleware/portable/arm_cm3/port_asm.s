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
.extern HardFault_Handler_c
.extern BusFault_Handler_c
.extern MemFault_Handler_c
.extern UsageFault_Handler_c

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
    MOV R0, #0x0
    MSR PRIMASK, R0
    BX LR

.type HardFault_Handler, %function
.thumb_func
HardFault_Handler:
    TST LR, #4;
    ITE EQ;
    MRSEQ R0, MSP;
    MRSNE R0, PSP;
    B hard_fault_handler_c;

.type BusFault_Handler, %function
.thumb_func
BusFault_Handler:
    TST LR, #4;
    ITE EQ;
    MRSEQ R0, MSP;
    MRSNE R0, PSP;
    B BusFault_Handler_c;

.type MemFault_Handler, %function
.thumb_func
MemFault_Handler:
    TST LR, #4;
    ITE EQ;
    MRSEQ R0, MSP;
    MRSNE R0, PSP;
    B MemFault_Handler_c;

.type UsageFault_Handler, %function
.thumb_func
UsageFault_Handler:
    TST LR, #4;
    ITE EQ;
    MRSEQ R0, MSP;
    MRSNE R0, PSP;
    B UsageFault_Handler_c;

.align 4
.end
