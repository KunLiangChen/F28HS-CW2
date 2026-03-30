#include <stdio.h>  // debugging only
#include "gpio.h"
#include "lcd-binary.h"
#include "cw2-aux.h"


/* ***************************************************************************** */
/* HINT: use the CPP variable ASM with ifdef's to select Asm (or C) versions of the code. */
/* ***************************************************************************** */


/*
  Hardware Interface function.
  Set the mode for pin number @pin@ to @mode@ (can be INPUT or OUTPUT (encoded as int)).
  compare the pin to determine the register
  moudle it to 0 - 9 (19 -> 9 etc.)
  use shift and orr i
*/
void pin_mode(volatile uint32_t *gpio, int pin, int mode)
{
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  /* ***************************************************************************** */
  asm volatile(
  "\tLDR R2, %[gpio]\n"
  "\tMOV R0, %[pin]\n"
  "\tMOV R1, %[mode]\n"
  "\tCMP R0, #10\n"
  "\tBGE 1f\n"
  "\tMOV R4, %[fsel0]\n"
  "\tLSL R4, R4, #2\n"
  "\tADD R2, R2, R4\n"  
  "\tB 3f\n"
  
  "\t1:\n"
  "\t CMP R0, #20\n"
  "\tBGE 2f\n"
  "\tMOV R4, %[fsel1]\n"
  "\tLSL R4, R4, #2\n"
  "\tADD R2, R2, R4\n" 
  "\tSUB R0, R0, #10\n"
  "\tB 3f\n"
  
  "\t2:\n"
  "\tMOV R4, %[fsel2]\n"
  "\tLSL R4, R4, #2\n"
  "\tADD R2, R2, R4\n" 
  "\tSUB R0, R0, #20\n"
  
  "\t3:\n"
  "\tADD R3, R0, R0, LSL #1\n"
  "\tLDR R4, [R2]\n"
  "\tMOV R5, #7\n" 
  "\tBIC R4, R4, R5, LSL R3\n"
  "\tAND R1, R1, #7\n"
  "\tORR R4, R4, R1, LSL R3\n"
  
  "\tSTR R4, [R2]\n"
  
  
  :
  :[gpio] "m" (gpio), [pin] "r" (pin), [mode] "r" (mode),
   [fsel0] "I" (GPIO_GPFSEL0),
   [fsel1] "I" (GPIO_GPFSEL1),
   [fsel2] "I" (GPIO_GPFSEL2)
  :"r5", "r4", "r3", "r2", "r1", "r0", "cc"
  );
}

/*
  Hardware Interface function.
  Send a @value@ along pin number @pin@. Values should be LOW or HIGH (encoded as int).
  check whether value is 1 or 0 and we have two different operation
*/
void digital_write (volatile uint32_t *gpio, int pin, int value)
{
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  /* ***************************************************************************** */
  asm volatile(
    "\tLDR R2, %[gpio]\n"
    "\tMOV R0, #1\n"
    "\tLSL R0, R0, %[pin]\n"  //move 1<<pin to specific bit
    
    "\tCMP %[val], #0\n"
    "\tBEQ 1f\n"
    
    //if not 0
    "\tMOV R1, %[set]\n"
    "\tSTR R0, [R2, R1, LSL #2]\n"
    "\tB 2f\n"
    
    "\t1:\n" //if 0
    "\tMOV R1, %[clr]\n"
    "\tSTR R0, [R2, R1, LSL #2]\n"
    
    "\t2:\n"
  :
  :[gpio] "m" (gpio), [pin] "r" (pin), [val] "r" (value),
   [set] "I" (GPIO_GPSET0),
   [clr] "I" (GPIO_GPCLR0)
  : "r5","r4","r3","r0","r1","r2", "cc"
  );

}

/*
  Hardware Interface function.
  Read input from a button device connected to pin @button@.. Result can be LOW or HIGH (encoded as int).
*/
int read_button(volatile uint32_t *gpio, int button) {
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  /* ***************************************************************************** */
  // fill in your code and replace the return statement below with the value read from the button
  return LOW;
}

