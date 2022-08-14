#include <stdint.h>
#include "BSP_Custom.h"
#include "CortexM.h"
#include "inc/tm4c123gh6pm.h"

// The following function are defined in OSasm.asm
extern void OS_DisableInterrupts(void); // Disable interrupts
extern void OS_EnableInterrupts(void);  // Enable interrupts
extern long OS_StartCritical (void);    // previous I bit, disable interrupts
extern void OS_EndCritical(long sr);    // restore I bit to previous value
//void WaitForInterrupt(void);  // low power mode

static uint32_t ClockFrequency = 16000000; // cycles/second

// ------------BSP_PeriodicTask_Init------------
// Activate an interrupt to run a user task periodically.
// Give it a priority 0 to 6 with lower numbers
// signifying higher priority.  Equal priority is
// handled sequentially.
// Input:  task is a pointer to a user function
//         freq is number of interrupts per second
//           1 Hz to 10 kHz
//         priority is a number 0 to 6
// Output: none
void (*PeriodicTask)(void);   // user function
void BSP_PeriodicTask_Init(void(*task)(void), uint32_t freq, uint8_t priority){long sr;
  if((freq == 0) || (freq > 10000)){
    return;                        // invalid input
  }
  if(priority > 6){
    priority = 6;
  }
  sr = OS_StartCritical();
  PeriodicTask = task;             // user function
  // ***************** Wide Timer5A initialization *****************
  SYSCTL_RCGCWTIMER_R |= 0x20;     // activate clock for Wide Timer5
  while((SYSCTL_PRWTIMER_R&0x20) == 0){};// allow time for clock to stabilize
  WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;// disable Wide Timer5A during setup
  WTIMER5_CFG_R = TIMER_CFG_16_BIT;// configure for 32-bit timer mode
                                   // configure for periodic mode, default down-count settings
  WTIMER5_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  WTIMER5_TAILR_R = (ClockFrequency/freq - 1); // reload value
  WTIMER5_TAPR_R = 0;              // bus clock resolution
  WTIMER5_ICR_R = TIMER_ICR_TATOCINT;// clear WTIMER5A timeout flag
  WTIMER5_IMR_R |= TIMER_IMR_TATOIM;// arm timeout interrupt
//PRIn Bit   Interrupt
//Bits 31:29 Interrupt [4n+3]
//Bits 23:21 Interrupt [4n+2],
//Bits 15:13 Interrupt [4n+1]
//Bits 7:5   Interrupt [4n]   n=26 => (4n)=104
  NVIC_PRI26_R = (NVIC_PRI26_R&0xFFFFFF00)|(priority<<5); // priority
// interrupts enabled in the main program after all devices initialized
// vector number 120, interrupt number 104
// 32 bits in each NVIC_ENx_R register, 104/32 = 3 remainder 8
  NVIC_EN3_R = 1<<8;               // enable IRQ 104 in NVIC
  WTIMER5_CTL_R |= TIMER_CTL_TAEN; // enable Wide Timer0A 32-b
  OS_EndCritical(sr);
}


// ------------BSP_Clock_InitFastest------------
// Configure the system clock to run at the fastest
// and most accurate settings.  For example, if the
// LaunchPad has a crystal, it should be used here.
// Call BSP_Clock_GetFreq() to get the current system
// clock frequency for the LaunchPad.
// Input: none
// Output: none
void BSP_Clock_InitFastest(void){
  // 0) configure the system to use RCC2 for advanced features
  //    such as 400 MHz PLL and non-integer System Clock Divisor
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;   // clear XTAL field
  SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;// configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;// clear oscillator source field
  SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;// configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;
  // 4) set the desired system divider and the system divider least significant bit
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;  // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~0x1FC00000) // clear system clock divider field
                  + (4<<22);      // configure for 80 MHz clock
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS)==0){};
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
  ClockFrequency = 80000000;
}

// ------------BSP_Clock_GetFreq------------
// Return the current system clock frequency for the
// LaunchPad.
// Input: none
// Output: system clock frequency in cycles/second
uint32_t BSP_Clock_GetFreq(void){
  return ClockFrequency;
}



