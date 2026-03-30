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
  "\tADD R2, R2, %[fsel0]\n"  
  "\tB 3f\n"
  
  "\t1:\n"
  "\t CMP R0, #20\n"
  "\tBGE 2f\n"
  "\tADD R2, R2, %[fsel1]\n"
  "\tSUB R0, R0, #10\n"
  "\tB 3f\n"
  
  "\t2:\n"
  "\tADD R2, R2, %[fsel2]\n"
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
*/
void digital_write (volatile uint32_t *gpio, int pin, int value)
{
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  /* ***************************************************************************** */
  asm volatile(
  "\tLDR R2, %[gpio]\n"
  
  :
  :[gpio] "m" (gpio), [pin] "r" (pin), [val] "r" (value)
  : "r2", "cc"
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

