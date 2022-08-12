// Name        : Basic Micro Controller Operating System main.c
// Author      : Mahendra Gunawardena
// Version     : Rev 0.01
// Copyright   : Your copyright notice
// Description : Demonstration of Basic Operating System Design Using both Assembly and C
//             : on TM4C123 ARM Cortex M4 Micro Controller
//============================================================================
/*
 * Basic Micro Controller Operating System main.c
 * Implementation of Basic Operating System Design Using both Assembly and C
*  on TM4C123 ARM Cortex M4 Micro Controller. The project access SW1 and SW2 and
 * to active the tricolor LED. Task1 monitors SW1 and TASK2 monitors SW2. Count1 and Count2
 * global variable are used to notify TASK3 responsible for activating the Tricolor LED
 *
 * Copyright Mahendra Gunawardena, Mitisa LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL I
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdint.h>
#include "os.h"
#include "BSP_Custom.h"
#include "inc/tm4c123gh6pm.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

uint32_t Count1;   // SW1 on time, track via incrementing Count1, and decremented when LED is active
uint32_t Count2;   // SW1 on time, track via incrementing Count2, and decremented when LED is active


// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad


//   Global Variables
//https://users.ece.utexas.edu/~valvano/Volume1/E-Book/C6_MicrocontrollerPorts.htm
#define switch_1                (*((volatile unsigned long *)0x40025040))
#define switch_2                (*((volatile unsigned long *)0x40025004))
#define led_red                 (*((volatile unsigned long *)0x40025008))
#define led_blue                (*((volatile unsigned long *)0x40025010))
#define led_green               (*((volatile unsigned long *)0x40025020))


//   Function Prototypes
void PortF_Init(void);

void Task1(void){
  Count1 = 0;
  for(;;){
    if (switch_1==0){
      Count1++;
    } else {
        Count1=0;
    }
  }
}
void Task2(void){
  Count2 = 0;
  for(;;){
      if (switch_2==0){
          Count2++;
       } else {
           Count2=0;
       }
  }
}
void Task3(void){
  for(;;){
      // if both SW1 and SW2 is active then both Count1 and Count2 will be positive. The GREEN LED is on
      // if SW1 is only active then Count1 will be increment and positive. The RED LED is on
      // if SW2 is only active then Count2 will be increment and positive. The BLUE LED is on
      // if both Count1 and Count2 are equal to zero then the tricolor LED is OFF
      if ((Count1>0)&&(Count2>0)){
          led_red = 0x00;
          led_blue = 0x00;
          led_green = 0x08;
          Count1--;
          Count2--;
      } else if (Count1>0){
          led_green = 0x00;
          led_blue = 0x00;
          led_red = 0x02;
          Count1--;
      } else if (Count2>0){
          led_red = 0x00;
           led_green = 0x00;
           led_blue = 0x04;
          Count2--;
      } else {
          led_red = 0x00;
          led_green = 0x00;
          led_blue = 0x00;
      }
  }
}
int main(void){
  OS_Init();           // initialize and disable interrupts
  // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in RGB LEDs)
  PortF_Init();
  OS_AddThreads(&Task1, &Task2, &Task3);
  OS_Launch(BSP_Clock_GetFreq()/THREADFREQ);
  return 0;            // this never executes
}

// Subroutine to initialize port F pins for input and output
// PF4 and PF0 are input SW1 and SW2 respectively
// PF3,PF2,PF1 are outputs to the LED
// Inputs: None
// Outputs: None
// Notes: These five pins are connected to hardware on the LaunchPad
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06



