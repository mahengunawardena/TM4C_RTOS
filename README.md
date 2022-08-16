# Implementing a simple RTOS on TIVA TM4C123

# Scope
The purpose of this project is to demonstrate the configuation of Code Composer Studio to configuration of Assembly and C developemetn enviroment for the purpose of RTOS
Development.

# Technical Details    

Integrated Development Enviorment: Code Composor Studio 12.00    
Compiler Version : TI v5.2.4  
Hardware : TIVA (TM4C123) Launch Pad   
Host System: Ubuntu 20.04 LTS   

The software has 3 tasks. Task1 is attached to SW1, Task2 is attached to SW2 and Task3 is attached to RGB LED. OSasm assembly code is responsible for thread switching.
The scheduler is Round Robin managed by the OSasm code. When SW1 is pressed Count1 is incremented. During the Task3 process Count1 is decremented. If the Count1 is 
positive then the RED LED is illuminated. When SW2 is pressed Count2 is incremented. During the Task3 process Count2 is decremented. If the Count2 is positive then
the BLUE LED is illuminated. If both Count1 and Count2 are positive the GREEN LED is illuminated, as weel is both Count1 and Count2 is decremented. 
