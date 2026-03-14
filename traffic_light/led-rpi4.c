#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
/* The base address of the GPIO peripheral (ARM Physical Address) varies
 * between Raspberry Pi versions. This is for the Raspberry Pi 4. */
#define GPIO_BASE       0xFE200000UL
#define GPIO_LENGTH     (4*1024)

#define GPIO_GPFSEL0    0
#define GPIO_GPFSEL1    1
#define GPIO_GPFSEL2    2
#define GPIO_GPFSEL3    3
#define GPIO_GPFSEL4    4
#define GPIO_GPFSEL5    5

#define GPIO_GPSET0     7
#define GPIO_GPSET1     8

#define GPIO_GPCLR0     10
#define GPIO_GPCLR1     11

#define LED_GPFSEL      GPIO_GPFSEL1
#define BUTTON_GPFSEL   GPIO_GPFSEL2

//#define LED_GPFBIT      6
//#define LED_GPSET       GPIO_GPSET0
//#define LED_GPCLR       GPIO_GPCLR0
//#define LED_GPIO_BIT    12

/* Pointer to the mapped GPIO registers */
volatile uint32_t *gpio;

void cleanup(int sig)
{
    printf("\nCleaning up and existing... \n");
    if(gpio != (void *)-1)
    {
        *(gpio + 10) = 0x00002C00; // close all the light
        *(gpio) = 0x00000000;
        *(gpio+1) = 0x00000000; // All pin back to input mode
        }
    exit(0);
    
    }

int main(int argc, char *argv[])
{
    signal(SIGINT, cleanup);
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        printf("can't open /dev/mem\n");
        return 1;
    }

    gpio = mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, GPIO_BASE);
    if (gpio == (void *) -1) {
        printf("can't mmap\n");
        return 1;
    }

   /*
    * Clean and set GPIO10 11 12 13 as output
    * */
    *(gpio+1) = 0x00000249;

    /*
     * Set the GPIO7 as input
     * */
    *gpio = 0x0;
    
    /* This loop runs forever */
    while (1) {
        //Light up the red light
        *(gpio+7) = 0x00000400;
        *(gpio+10) = 0x00002800;
        sleep(2);
        
        //Light up the red and the yellow
        *(gpio+7) = 0x00000C00;
        *(gpio+10) = 0x00002000;
        sleep(2);
        
        //Light up the green
        *(gpio+10) = 0x00000C00;
        *(gpio+7) = 0x00002000;
        
        int timeout_ms = 5000;
        int elapsed_ms = 0;
        while(elapsed_ms < timeout_ms)
        {
            if(*(gpio+13) & 0x00000080)
            {
                usleep(10000);
                break;
                }
                usleep(10000);
                elapsed_ms += 10;
            }
        *(gpio+10) = 0x00002000;
        for(int i =0; i<3; i++)
        {
            *(gpio + 7) = 0x00000800;
            sleep(1);
            *(gpio + 10) = 0x00000800;
            sleep(1);
            }
    
    }
    return 0;
}
