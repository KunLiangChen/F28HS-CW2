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
  compare the pin number and choose the correct register.
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
  "\tADD R0, R0, R0, LSL #1\n"
  "\tLSL R1, R1, R0\n"
  "\tSTR R1, [%[gpio], #GPIO_GPFSEL0]\n"
  "\t1: CMP R0, #20\n"
  "\tBGE 2f\n"
  "\tADD R0, R0, R0, LSL #1\n"
  "\tLSL R1, R1, R0\n"
  "\tSTR R1, [%[gpio], #GPIO_GPFSEL1]\n"
  "\t2: ADD R0, R0, R0, LSL #1\n"
  "\tLSL R1, R1, R0\n"
  "\tSTR R1, [%[gpio], #GPIO_GPFSEL2]\n"
  :
  :[gpio] "m" (gpio), [pin] "r" (pin), [mode] "r" (mode)
  :"r2", "r1", "r0", "cc"
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

