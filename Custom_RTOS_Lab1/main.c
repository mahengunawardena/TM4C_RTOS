#include <stdint.h>
#include "os.h"
#include "BSP_Custom.h"
#include "inc/tm4c123gh6pm.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

uint32_t Count1;   // number of times thread1 loops
uint32_t Count2;   // number of times thread2 loops
// P2OUT at 0x40004C03
//#define LED_RED   (*((volatile uint8_t *)(0x42000000+32*0x4C03+4*0)))  /* Port 2.0 Output */
//#define LED_GREEN (*((volatile uint8_t *)(0x42000000+32*0x4C03+4*1)))  /* Port 2.1 Output */
//#define LED_BLUE  (*((volatile uint8_t *)(0x42000000+32*0x4C03+4*2)))  /* Port 2.2 Output */

// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad

// 1. Pre-processor Directives Section
// Constant declarations to access port registers using
// symbolic names instead of addresses

//#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
//#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
//#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
//#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
//#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
//#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
//#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
//#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
//#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
//#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))


//https://users.ece.utexas.edu/~valvano/Volume1/E-Book/C6_MicrocontrollerPorts.htm
#define switch_1                (*((volatile unsigned long *)0x40025040))
#define switch_2                (*((volatile unsigned long *)0x40025004))
#define led_red                 (*((volatile unsigned long *)0x40025008))
#define led_blue                (*((volatile unsigned long *)0x40025010))
#define led_green               (*((volatile unsigned long *)0x40025020))

// 2. Declarations Section
//   Global Variables
unsigned long SW1,SW2;  // input from PF4,PF0
unsigned long Out;      // outputs to PF3,PF2,PF1 (multicolor LED)

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
  OS_Init();           // initialize, disable interrupts, 48 MHz
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



