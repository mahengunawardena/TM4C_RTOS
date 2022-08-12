// os.h
// Runs on TM4C123


#ifndef __OS_H
#define __OS_H  1


// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch
// initialize OS controlled I/O: SysTick, 48 MHz crystal
// Inputs: none
// Outputs: none
void OS_Init(void);

// ******** OS_AddThread ***************
// add three foregound threads to the scheduler
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThreads(void(*task0)(void),
                 void(*task1)(void),
                 void(*task2)(void));

// ******** OS_Launch ***************
// start the scheduler, enable interrupts
// Inputs: number of 20.833ns clock cycles for each time slice
//         (maximum of 24 bits)
// Outputs: none (does not return)
void OS_Launch(uint32_t theTimeSlice);

#endif
