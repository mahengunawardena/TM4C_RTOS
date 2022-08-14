; Name        : Basic Micro Controller Operating System OSasm.asm
; Author      : Mahendra Gunawardena
; Version     : Rev 0.01
; Copyright   : Your copyright notice
; Description : Demonstration of Basic Operating System Design Using both Assembly and C
;             : on TM4C123 ARM Cortex M4 Micro Controller
;============================================================================
;
; Basic Micro Controller Operating System main.c
; Implementation of Basic Operating System Design Using both Assembly and C
;  on TM4C123 ARM Cortex M4 Micro Controller. The project access SW1 and SW2 and
; to active the tricolor LED. Task1 monitors SW1 and TASK2 monitors SW2. Count1 and Count2
; global variable are used to notify TASK3 responsible for activating the Tricolor LED
;
; Copyright Mahendra Gunawardena, Mitisa LLC
 ;
; Redistribution and use in source and binary forms, with or without modification,
; are permitted provided that the following conditions are met:
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
; INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
; AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL I
; BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
; GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
; HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;


        .thumb
        .text
        .align 2

        .global  RunPt            ; currently running thread
        .global  OS_DisableInterrupts
        .global  OS_EnableInterrupts
        .global	 OS_EndCritical
        .global  OS_StartCritical
        .global  StartOS
        .global  SysTick_Handler

OS_DisableInterrupts:  .asmfunc
        CPSID   I
        BX      LR
       .endasmfunc


OS_EnableInterrupts:  .asmfunc
        CPSIE   I
        BX      LR
       .endasmfunc

 ;*********** StartCritical ************************
; make a copy of previous I bit, disable interrupts
; inputs:  none
; outputs: previous I bit
OS_StartCritical:  .asmfunc
        MRS    R0, PRIMASK  ; save old status
        CPSID  I            ; mask all (except faults)
        BX     LR
        .endasmfunc

;*********** EndCritical ************************
; using the copy of previous I bit, restore I bit to previous value
; inputs:  previous I bit
; outputs: none
OS_EndCritical:  .asmfunc
        MSR    PRIMASK, R0
        BX     LR
 		.endasmfunc
;*********** WaitForInterrupt ************************
; go to low power mode while waiting for the next interrupt
; inputs:  none
; outputs: none
OS_WaitForInterrupt:  .asmfunc
        WFI
        BX     LR
 		.endasmfunc

RunPtAddr .field RunPt,32
SysTick_Handler:  .asmfunc     ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
    PUSH    {R4-R11}           ; 3) Save remaining regs r4-11
    LDR     R0, RunPtAddr      ; 4) R0=pointer to RunPt, old thread
    LDR     R1, [R0]           ;    R1 = RunPt
    STR     SP, [R1]           ; 5) Save SP into TCB
    LDR     R1, [R1,#4]        ; 6) R1 = RunPt->next
    STR     R1, [R0]           ;    RunPt = R1
    LDR     SP, [R1]           ; 7) new thread SP; SP = RunPt->sp;
    POP     {R4-R11}           ; 8) restore regs r4-11
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restore R0-R3,R12,LR,PC,PSR
    .endasmfunc


StartOS:  .asmfunc
    LDR     R0, RunPtAddr      ; currently running thread
    LDR     R2, [R0]           ; R2 = value of RunPt
    LDR     SP, [R2]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11
    POP     {R0-R3}            ; restore regs r0-3
    POP     {R12}
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R1}               ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread
    .endasmfunc

    .end
