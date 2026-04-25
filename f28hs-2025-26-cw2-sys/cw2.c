/* 
 *
 * F28HS CW2
 * pinCrack: button input of a sequence of numbers followed by cracking a secret PIN
 * Uses interval timers for the timeout/delay function

 * Compile:    	      make cw2
 * Run (e.g):         sudo ./cw2 -d -e -s 112
 * Run (unit-test):   sudo ./cw2 -u -s 112 -r 121

 ***********************************************************************
 * The development of this code was heavily based on the wiringPi library by Gordon Henderson.
 * This instance of the code, however, does not depend directly on the wiringPi library any more.
 *
 * wiringPi:
 *	Arduino look-a-like Wiring library for the Raspberry Pi
 *	Copyright (c) 2012-2015 Gordon Henderson
 *	Additional code for pwmSetClock by Chris Hall <chris@kchall.plus.com>
 *
 *	Thanks to code samples from Gert Jan van Loo and the
 *	BCM2835 ARM Peripherals manual, however it's missing
 *	the clock section /grr/mutter/
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

/* --------------------------------------------------------------------------- */
/* Config settings */

// NOTE: most config settings are in cw2-config.h

/* --------------------------------------------------------------------------- */
/* Imports */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "cw2-config.h"
#include "cw2-aux.h"
#include "lcd-binary.h"
#include "lcd-fcts.h"

/* --------------------------------------------------------------------------- */
/* Constants (see cw2-config.h for default values) */

// number of possible values at each position in the sequence
static  int digits = DIGITS;
// length of the sequence
static  int seqlen = SEQL;

// SECRET sequence
static int* theSeq = NULL;

// base address of GPIO memory
volatile unsigned int gpiobase ;
volatile uint32_t *gpio ;

// flag to be set in signal handler for interval times
static volatile int timed_out = 0;
static struct sigaction sa;
static struct itimerval timer;
/* --------------------------------------------------------------------------- */
/* external prototypes */

/* *****************************************************************************
   HINT: use this CPP flag to select between a C and an Asm implementation 
         of the Hamming distance.
 ***************************************************************************** */

#ifdef HAMM_ASM
// prototype for the Assembler fct; only needed for an Asm implementation
int hamming(const int *x, const int *y, int seqlen);
#endif

/* --------------------------------------------------------------------------- */
// Timers and signal handlers

// time-stamps for use in signal handlers
// static uint64_t startT, stopT;

/* 
   Get a timestamp in micro-seconds 
*/
uint64_t timeInMicroseconds(void){
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec; // in us
}

/*
  This should be a signal handler for signals issued by the interval timer.
*/
void timer_handler (int signum)
{
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE (replace existing code) */
  timed_out = 1;
  /* ***************************************************************************** */
  //printf("timer_handler: not implemented; should set value of time_out; current value: %d\n", timed_out);
}

/* 
   Initialise the interval timer here.
*/
void initITimer(uint64_t timeout){
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler= &timer_handler;
  sigaction(SIGALRM, &sa, NULL);
  timer.it_value.tv_sec = timeout / 1000000;
  timer.it_value.tv_usec = timeout% 1000000;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  
  /* ***************************************************************************** */

}

/* --------------------------------------------------------------------------- */
/* Helper functions for the main app */

/* 
   Initialise the secret sequence of values (of length seqlen) 
   Uses global variables: @seqlen@ for length of sequence, @digits@ for the possible number of values
*/
void initSeq(int seqlen, int digits) {
  unsigned long value, r;

  if (theSeq==NULL) {
    theSeq = calloc(seqlen, sizeof(int));
    if (theSeq==NULL) {
      failure(true, "calloc failed");
    }
  }

  srand((unsigned int)time(NULL));
  for (int i=0; i<seqlen; i++) {
    r = rand();
    value = (r % digits) + 1;
    theSeq[i] = value;
  }
}

/* 
   Show given sequence @seq@ of length @seqlen@ on the terminal.
*/
void showSeq(const int *seq, int seqlen) {
  printf("Contents of the sequence (of length %d): ", seqlen);
  for (int i=0; i<seqlen; i++) {
    printf(" %d", seq[i]);
  }
  printf("\n");
}

/* 
   Parse an integer value @val@ as a list of digits, and put them into @seq@ 
   Needed for processing command-line with options -s or -u            
*/
void readSeq(int *seq, int seqlen, int val) {
  char valStr[32];
  int i;
  size_t strLen;

  snprintf(valStr, sizeof(valStr), "%d", val);
  strLen = strlen(valStr);
  
  for (i = 0; i < seqlen && i < (int)strLen; i++) {
    seq[i] = valStr[i] - '0';
      if (seq[i] < 1 || seq[i] > digits) {
          seq[i] = 1;
      }
  }

  // pad with 1 values if necessary
  for (; i < seqlen; i++) {
      seq[i] = 1;
  }
}

/* --------------------------------------------------------------------------- */
/* Interface fcts on top of the low-level pin I/O code                         */

/* 
   Blink @led@ @c@ times
*/
void blinkN(volatile uint32_t *gpio, int led, int c) { 
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  for(int j = 0; j < c; j++)
      {
      digital_write(gpio, led, 1);
      usleep(500000);
      digital_write(gpio, led, 0);
      usleep(500000);
        }
  /* ***************************************************************************** */
}

/* 
   Turning LED on/off is just a call to low-level fct digital_write()
 */
static inline
void write_LED(volatile uint32_t *gpio, int pin, int value) {
  digital_write (gpio, pin, value);
}

/* ----------------------------------------------------------------------------- */
/* Helper fcts for this app                                                      */

/*
  HINT: the libc function powl(x, n) computes @x@ to the power of @n@
        and is a useful function for Task~5, using arbitrary sequence length.
*/

/* ***************************************************************************** */
/* NOTE: CPP flag should select Assembler version of Hamming distance            */
/*       Set the flag in the Makefile using -DHAMM_ASM                           */
/* If the flag is NOT set (as below) a C version of the Hamming distance should be selected */
/* ***************************************************************************** */

#ifndef HAMM_ASM
/*
  Computer the Hamming distance between to arrays of ints.
  OPTIONAL: this is a C implementation of Hamming distance; this version is optional
  HINT: implement a C version and test it, before implementing an ARM Assembler version
  NOTE: if you use a C implementation, you may need to modify the Makefile and remove 'hamming.o' from the list of linked files.
  The final version of the code should use the Assembler version in file hamming.s
*/
int hamming(const int *x, const int *y, int seqlen) {
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE (C version is OPTIONAL, but you need an implementation of hamming() */
  /* ***************************************************************************** */
  int hamming = 0;
  for(int i = 0;i < seqlen; i++)
  {
    if (*(x+i) != *(y+i)) hamming++;
    }
  return hamming;
  //fprintf(stderr, "hamming: still needs to be implemented\n");
}
#endif

/*
  Show the Hamming distance (of @seq1@ and @seq2@) in @code@ on the terminal.
*/
void showHamm(int code, const int *seq1, const int *seq2) {
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  printf(" hamming %d \n",code);
  /* ***************************************************************************** */
  //fprintf(stderr, "showHamm: still needs to be implemented\n");
}

/* 
   OPTIONAL: implement an increment in a array of arbitrary length in @seq@
             by one, assuming the max value per element is @digits@.
  HINT: this function is useful for Task 3: arbitrary length of sequence.	     
*/
static inline
int incseq(int *seq, int seqlen,  int digits){
  /* ***************************************************************************** */
  /* OPTIONALLY COMPLETE THIS CODE */
  int carry = 1;
  for(int i = seqlen-1; i>=0; i--)
  {
    seq[i]+= carry;
    if(seq[i] > digits)
    {
      seq[i] = 1;
      carry = 1;
      }else
      {
        carry = 0;
        break;
        }
    
    }
    return carry;
  /* ***************************************************************************** */
}

/* --------------------------------------------------------------------------- */

/* 
   @submit_PIN@: Submit a PIN for checking against a secret pin.  EXPENSIVE!
   @attSeq@ is the attempted sequence, submitted for testing against the secret pin.
   @seqlen@ is the length of the sequence.
   @submitDelay@ is the delay in processing the sequence (can be changed with -S cmdline option).
   NOTE: the secret sequence is in the global variable @theSeq@ which is not an argument
         because it should be hidden to the caller.
   The function tests @attSeq@ against the secret sequence @theSeq@, by computing the Hamming distance.
   If the Hamming distance is 0, both sequences are equal, and the sequence has been found (in @attSeq@)
   The return value is a boolean value whether the sequence has been found.
*/

int submit_PIN(const int *attSeq, int seqlen, int submitDelay) {
  int found = 0;
  
  // debugging only (needs additional arguments!):
  // showSeq(attSeq,seqlen);   showHamm(code, refSeq, attSeq);

  // submits++;         // now done at caller side
  usleep(submitDelay);  // simulating a slow submit action
  found = hamming(theSeq, attSeq, seqlen) == 0;
  return found;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/**
 * @brief Inner recursive. Give pos, gernate all the possible seq
 * @param pos_indices  pos need to modify
 * @param current_idx  point out which pos we are modifying
 * @param m            the num of pos
 */
void fill_values_and_submit(int* pos_indices, int current_idx, int m, 
                            int* attemptSeq, int* currentSeq, 
                            int digits, int seqlen, int submitDelay, 
                            int* found, int* submits, int* found_at, int opt_e) {
    if (*found && !opt_e) return;

    // all pos is assigned
    if (current_idx == m) {
        (*submits)++;
        if (submit_PIN(currentSeq, seqlen, submitDelay)) {
            *found = 1;
           
        }
        return;
    }

    int target_pos = pos_indices[current_idx];
    int original_val = attemptSeq[target_pos];

    // all possible assignment
    for (int val = 1; val <= digits; val++) {
        
        if (val == original_val) continue; 

        currentSeq[target_pos] = val;
        fill_values_and_submit(pos_indices, current_idx + 1, m, 
                               attemptSeq, currentSeq, digits, 
                               seqlen, submitDelay, found, submits, found_at, opt_e);
        
        if (*found && !opt_e) return;
    }
}

/**
 * @brief outer loop: enumerate all the possible positions
 * @param start: the start of domain
 * @param count: the selected pos
 * @param m: the hamming_dist, the num of pos need to be select
 * @param pos_indices: use to store the pos combination
 * @param attemptSeq: input sequence
 * @param currentSeq: the seq used to modify and submit
 * 
 * end: the count == m, means all the pos is selected
 * opt: assign a number at pos
 * step: assign a nuber at the next pos, but domain modify to [num+1, n]
 */
void select_positions(int start, int count, int m, int* pos_indices, 
                      int* attemptSeq, int* currentSeq, 
                      int digits, int seqlen, int submitDelay, 
                      int* found, int* submits, int* found_at, int opt_e) {
    if (*found && !opt_e) return;

    // m pos is selected , enter inner loop
    if (count == m) {
        fill_values_and_submit(pos_indices, 0, m, attemptSeq, currentSeq, 
                               digits, seqlen, submitDelay, found, submits, found_at, opt_e);
        return;
    }

    
    for (int i = start; i < seqlen; i++) {
        pos_indices[count] = i;
        
        select_positions(i + 1, count + 1, m, pos_indices, 
                         attemptSeq, currentSeq, digits, seqlen, 
                         submitDelay, found, submits, found_at, opt_e);
    }
}

int main(int argc, char **argv){
  int found = 0, code = 0, refCode = 0;
  int buttonPressed = 0;

  // use these to count: number of comparisons in total, found after how many attempts, total number of submits
  int attempts = 0, found_at = 0, submits = 0;
  int *attemptSeq = NULL, *refSeq = NULL; 
  double startTime, stopTime;

  // variables holding Pin numbers for LEDs and button
  int pinLED = LED, pinLED2 = LED2, pinButton = BUTTON;
  // int fSel, shift, pin,  clrOff, setOff, off, res;
  int   fd ;

  // strings for temporary usage (e.g. writing to LCD display)
   char str1[32];
   char str2[32];

  // useful for interval timers					    
   struct timeval t1, t2;

  // variables for command-line processing
  // command-line options
  bool opt_e = false, opt_l = false;
  int opt_m = 0, opt_n = 0, opt_S = 0, opt_s = 0, opt_r = 0;
  // variables derived from command line options
  bool verbose = false, help = false, debug = false, unit_test = false;
  int submitDelay = SUBMIT_DELAY;
  
  // -------------------------------------------------------
  // process command-line arguments

  // see: man 3 getopt for docu and an example of command line parsing
  { // see the CW spec for the intended meaning of these options
    int opt;
    while ((opt = getopt(argc, argv, "hvdeluS:s:r:m:n:")) != -1) {
      switch (opt) {
      case 'v':
	verbose = true;
	break;
      case 'h':
	help = true;
	break;
      case 'd':
	debug = true;
	break;
      case 'e':
	opt_e = true;
	break;
      case 'l': // LCD test only
	opt_l = true;
	break;
      case 'u':
	unit_test = true;
	break;
      case 'S':
	opt_S = atoi(optarg);
	submitDelay = opt_S;
	break;
      case 's':
	opt_s = atoi(optarg); 
	break;
      case 'r':
	opt_r = atoi(optarg); 
	break;
      case 'm':
	opt_m = atoi(optarg);
	digits = opt_m;
	break;
      case 'n':
	opt_n = atoi(optarg);
	seqlen = opt_n;
	break;
      default: /* '?' */
	fprintf(stderr, "Usage: %s [-h] [-v] [-d] [-e] [-m <maxval> ] [-n <seqlen>] [-u <seq1> <seq2>] [-s <secret seq>] [-r <reference seq>]  \n", argv[0]);
	exit(EXIT_FAILURE);
      }
    }
  }

  if (help) {
    fprintf(stderr, "pinCrack program, running on a Raspberry Pi, with connected LED, button and LCD display\n"); 
    fprintf(stderr, "Use the button for input of numbers. The LCD display will show the matches with the secret sequence.\n"); 
    fprintf(stderr, "For full specification of the program see: https://www.macs.hw.ac.uk/~hwloidl/Courses/F28HS/F28HS_CW2_2026.pdf\n"); 
    fprintf(stderr, "Usage: %s [-h] [-v] [-d] [-e] [-u <seq1> <seq2>] [-s <secret seq>] [-r <reference seq>]  \n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (verbose) {
    printf("Settings for running the program\n");
    printf("Verbose is %s\n", (verbose ? "ON" : "OFF"));
    printf("Debug is %s\n", (debug ? "ON" : "OFF"));
    printf("Unittest is %s\n", (unit_test ? "ON" : "OFF"));
    printf("Exhaustive search is %s\n", (opt_e ? "ON" : "OFF"));
    printf("Submit delay is %d\n", submitDelay);
    if (opt_s)  printf("Secret sequence set to %d\n", opt_s);
    if (opt_r)  printf("Reference sequence set to %d\n", opt_r);
  }

  if (verbose) {
    printf("Hint: remember to compute the Hamming distance in each iteration and assign it to variable code; current (unused) value: %d\n", code);
    printf("Code style requirement: collect the values of the input sequence in the variable attemptSeq; current (unused) value: %p\n", attemptSeq);
  }  

  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  const char name[] = "Chen";
  int *submitSeq;
  attemptSeq = calloc(seqlen, sizeof(int));
  submitSeq = calloc(seqlen, sizeof(int));
  /* Initialise the sequences that you need here, before using them  */
  /* ***************************************************************************** */

  if (opt_s) { // if -s option is given, use the sequence as SECRET sequence
    if (theSeq==NULL) {
      theSeq = calloc(seqlen, sizeof(int));
      if (theSeq==NULL) {
        failure(true, "calloc failed");
      }
    }
    readSeq(theSeq, seqlen, opt_s);
    if (verbose) {
      fprintf(stderr, "Running program with secret sequence:\n");
      showSeq(theSeq,seqlen);
    }
  }
  
  if (opt_r) { // if -r option is given, use the sequence as REFERENCE sequence
    if (refSeq==NULL) {
      refSeq = calloc(seqlen, sizeof(int));
      if (refSeq==NULL) {
        failure(true, "calloc failed");
      }
    }
    readSeq(refSeq, seqlen, opt_r);
    if (verbose) {
      fprintf(stderr, "Running program with reference sequence:\n");
      showSeq(refSeq,seqlen);
    }
  }
  
  /* --------------------------------------------------------------------------- */
  /* Configuration of the LCD display */
  int bits, rows, cols ;

  // hard-coded: 16x2 display, using a 4-bit connection
  bits = 4; 
  cols = 16; 
  rows = 2; 

  printf ("Raspberry Pi configuration: red LED: %d; green LED: %d; button: %d\n", pinLED2, pinLED, pinButton) ;
  printf ("Raspberry Pi LCD driver for a %dx%d display (%d-bit wiring) \n", cols, rows, bits) ;

  /* --------------------------------------------------------------------------- */
  /* Check for root priveleges (needed for controlling LEDs etc) */

  if (geteuid () != 0) {
    fprintf (stderr, "setup: Must be root. (Did you forget sudo?)\n") ;
    exit(EXIT_FAILURE);
  }
  
  /* --------------------------------------------------------------------------- */
  /* constants for RPi2/3. NOTE: RPi4 needs a different base address */
  // -----------------------------------------------------------------------------
  // RPi2/3
  //gpiobase = 0x3F200000 ;
  // RPi4
   gpiobase = 0xFE200000 ;

  // -----------------------------------------------------------------------------
  // memory mapping 
  // Open the master /dev/memory device

  if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
    return failure (false, "setup: Unable to open /dev/mem: %s\n", strerror (errno)) ;

  // GPIO:
  gpio = mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;
  if ((int32_t)gpio == -1)
    return failure (false, "setup: mmap (GPIO) failed: %s\n", strerror (errno)) ;

  // -----------------------------------------------------------------------------
  // Setting mode of pins

  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  pin_mode(gpio, LED, OUTPUT);
  pin_mode(gpio, LED2, OUTPUT);
  pin_mode(gpio, BUTTON, INPUT);
  /* Set the mode for the pins here, using the low-level functions in lcd-binary.c */
  /* ***************************************************************************** */
  
  // -----------------------------------------------------------------------------
  // Initialise the LCD display

  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
  /* Initialise the LCD display */
  lcd_init(gpio);

 
  /* ***************************************************************************** */


  if (opt_l) { // TESTING only: show some text on the LCD display to demonstrate it's working in principle
    /* ***************************************************************************** */
    /* OPTIONAL CODE to show that the LCD display is working */
    /* ***************************************************************************** */

    lcd_puts(gpio, name);
    exit(2);
  }
  
  // -----------------------------------------------------------------------------
  // App initialisation
  
  /* Initialise the secret sequence */
  if (!opt_s)  initSeq(seqlen, digits);

  /* Use the debugging option like this for extra messages */
  if (debug) {
    printf("Secret sequence is: ");
    showSeq(theSeq,seqlen);
  }

  // -----------------------------------------------------------------------------
  // Unit testing: check the Hamming distance between two given sequences

  if (unit_test) { // unit test: just print the Hamming distance

    if (!opt_r) {
      fprintf(stderr, "Need to use both -s and -r for unit testing (with -u)\n");
      exit(EXIT_FAILURE);
    }

    // output to screen
    refCode = hamming(theSeq, refSeq, seqlen);
    showSeq(theSeq,seqlen);
    showSeq(refSeq,seqlen);
    showHamm(refCode, theSeq, refSeq); 
    sprintf(str1, "%d", refCode);
    lcd_puts(gpio,str1);
    exit(EXIT_SUCCESS);
  }  

  // -----------------------------------------------------------------------------
  
  /* Print Greetings Message on LCD display */
  /* ***************************************************************************** */
  /* COMPLETE THIS CODE */
   lcd_puts(gpio, name);
  /* ***************************************************************************** */

  /* OPTIONAL: wait for ENTER key before continuing */
  waitForEnter() ; // -------------------------------------------------------

  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  /* Main part of the application  */

  // -------------------------------------------------------
  // PHASE 1: sequence input

  // ...........................................................................
  // Iterate over all elements of the sequence
  if(!opt_r)
  {
  initITimer(TIMEOUT);
  for (int i=0; i<seqlen; i++) {
      if(setitimer(ITIMER_REAL, &timer, NULL) != 0)
      {
        fprintf(stderr,"Error, fail to setitimer. ");
        exit(2);
        }
      /*
       * Use to eliminate the button error and read accurate number
       * */
      while(!timed_out)
      {
        if(read_button(gpio,BUTTON)) 
        {
          usleep(20000);
          if(!read_button(gpio,BUTTON)) continue;
          buttonPressed++;
          while(read_button(gpio,BUTTON) && !timed_out);
          usleep(20000);
          }
        }
      if (buttonPressed < 1 || buttonPressed > digits) {
          buttonPressed = 1;
      }
      attemptSeq[i] = buttonPressed;
      write_LED(gpio, LED2, 1);
      sleep(1);
      write_LED(gpio, LED2, 0);
      sleep(1);
      //for(int j = 0; j < buttonPressed; j++)
      //{
      //digital_write(gpio, LED, 1);
      //usleep(500000);
      //digital_write(gpio, LED, 0);
      //usleep(500000);
        //}
      
      //More clear 
      blinkN(gpio, LED,buttonPressed);
      timed_out = 0;
      
      /* ***************************************************************************** */
      /* COMPLETE THIS CODE (replace existing code)  
       *                                   */
      /* Complete the body of the loop, reading button presses to input numbers        */
      /* using interval timers for timeout                                             */
      /* and filling in a sequence with the input values                               */
      /* ***************************************************************************** */
      printf(" value of buttonPressed: current value: %d\n", buttonPressed);
      buttonPressed = 0;
  }
}
    // -------------------------------------------------------
    // PHASE 2: Main Task: full search

    // Print the version of the code this is running; set values in cw2-config.h
    printf("--------------------- \n");
    printf(">> Version %d: %s with %d digits and %d sequence length\n", VERSION, VERSION_STR, digits, seqlen);
#ifdef HAMM_ASM
    printf(">> HAMM_ASM version: Hamming distance in ARM Assembler\n");
#else
    printf(">> Hamming in C version\n");
#endif
    printf("--------------------- \n");

    if (debug) {
      printf("Debug mode\nThe secret sequence is:");
      showSeq(theSeq,seqlen);
    }

    // calculate the total range of possible sequences
    unsigned long bound = powl(digits, seqlen);
    

    // time-stamp
    startTime = clock();

    /* ***************************************************************************** */
    /* COMPLETE THIS CODE                                                            */
    /* implement a check of the Hamming distance of the input and secret sequence    */
    /* then search for the secret sequence and report where it was found             */
    /* with -r option, the commandline sequence should be used as input sequence     */
    /* ***************************************************************************** */
    int hamming_dist = 0;
    if(!opt_r)
    {
      hamming_dist = hamming(theSeq, attemptSeq, seqlen);
      }else
      {
        readSeq(attemptSeq, seqlen, opt_r);
        hamming_dist = hamming(theSeq, attemptSeq, seqlen);
        }

#ifdef TASK4 // This is a task 4 full space implementation
    for(unsigned long i = 0; i < bound; i++)
    {
      //1234 -> [1] [2] [3] [4]
      int temp = i;
      for(int j = seqlen-1; j >=0; j--)
      {
        submitSeq[j] = temp%digits+1;
        temp/=digits;
        }
      attempts++;
      //map from decimal to 3.
      if(hamming(submitSeq,attemptSeq,seqlen)== hamming_dist) 
      {
        int current_found = submit_PIN(submitSeq, seqlen, submitDelay);
        submits++;
        
        if(current_found & !found)
        {
          found = 1;
          found_at = i+1;
          }
        }
      
      if(found & !opt_e) 
      {
      break;
      }
      }
#else // This is task5 two loop implementation
    /**
     * @brief two loop: outer give out all the result of combination
     * inner try all the val in specific position
     * @attention we don't need to check submitSeq's hamming_dist at all now
     * as it is obtained from change hammming_dist digits in attemptSeq
     * @attention we use goto here which is a very dangerous operation here.
     * */
    int n = seqlen;
    int m = hamming_dist; //C_n^m
    int should_stop = 0;
    memcpy(submitSeq, attemptSeq, seqlen*sizeof(int));
    
    if(m==0) //It is the answer
    {
      attempts++;
      submits++;
      if(submit_PIN(submitSeq, seqlen, submitDelay))
      {
        found = 1;
        found_at = attempts;
        }
      }else
      {
        int combo[m];
        for(int i = 0; i < m; i++) combo[i] = i;
        
        while(1)
        {
        memcpy(submitSeq, attemptSeq, seqlen*sizeof(int));
        int vals[m];
        for(int i = 0; i < m; i++) vals[i] = 1; //init the comb
        int val_done = 0;
        
        while(!val_done)
        {
          for(int i=0; i<m; i++)
          {
              submitSeq[combo[i]] = vals[i]; //Modify the sequence
            }
          //for(int i =0 ;i <seqlen;i++)
          //{
            //printf("%d ",submitSeq[i]);
            //}
          //printf("\n");
          /*Try to submit */
          attempts++;
          submits++;
          if(submit_PIN(submitSeq,seqlen,submitDelay)){
            if(!found)
            {
              found = 1;
              found_at = attempts;
              }
            if(!opt_e) 
            {
              should_stop = 1;
              break;
              }
            }
            
            
            if(incseq(vals,m,digits)) val_done = 1; //if detect overflow, then stop loop
          }
          if(should_stop) break;
          /*Generate next combination of loop*/
          int i = m-1; //pointer, point to the first position who reach the maximum.
          /*
           * For each pos, to maintain the rising order , they have upper bound
           * comb[m-1] <= n-1 (The last pos)
           * m- 1 = n-1
           * m -m + i = n-m+i
           * so comb[i] <= n-m+i
           * 
           * */
          while(i >=0 && combo[i] == n-m+i)
          {
            i--;
            }
          if(i < 0) break; //all combination exhauted
          
          combo[i]++;
          for(int j = i+1; j < m; j++)
          {
            combo[j] = combo[j-1] + 1; //reset all the seq. because of rising seq, so the minum 1, 2,3,4 ...
            }
          }
        }
    
        search_done:;
#endif    

    stopTime = clock();

    printf("Runtime; %f secs\n", (stopTime-startTime)/CLOCKS_PER_SEC);
    printf("Sequence %s\n", found ? "found" : "not found");
    printf("%s search finished for %d digits and %d seqlen (expect %ld):\n%d attempts (found at %d i.e. %.2f %%), %d submits\n",
	 (opt_e ? "Exhaustive" : "Non-exhaustive"), digits, seqlen, bound, attempts, found_at, (float)found_at / ((float)bound / 100.0), submits);
    printf("Secret sequence was: ");
    showSeq(theSeq,seqlen);

  /* ***************************************************************************** */
  /* COMPLETE THIS CODE                                                            */
  /* write an exit message to the LCD display                                      */
  /* ***************************************************************************** */
  blinkN(gpio, LED, 2);
  lcd_clear(gpio);
  usleep(2000);
  lcd_write_row(gpio, 1,"PIN");
  lcd_write_row(gpio, 2,"found");
  free(theSeq);
  free(refSeq);
  free(attemptSeq);
  free(submitSeq);

 return 0;
}
