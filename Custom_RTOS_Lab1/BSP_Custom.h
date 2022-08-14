/*
 * BSP_Custom.h
 *
 *  Created on: Aug 6, 2022
 *      Author: mahen
 */

// ------------BSP_Clock_InitFastest------------
// Configure the system clock to run at the fastest
// and most accurate settings.  For example, if the
// LaunchPad has a crystal, it should be used here.
// Call BSP_Clock_GetFreq() to get the current system
// clock frequency for the LaunchPad.
// Input: none
// Output: none
void BSP_Clock_InitFastest(void);

// ------------BSP_Clock_GetFreq------------
// Return the current system clock frequency for the
// LaunchPad.
// Input: none
// Output: system clock frequency in cycles/second
uint32_t BSP_Clock_GetFreq(void);

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
void BSP_PeriodicTask_Init(void(*task)(void), uint32_t freq, uint8_t priority);



