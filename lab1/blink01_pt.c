//------------------------------------------------------------------------------
// Headers that are required for printf and mmap
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

//------------------------------------------------------------------------------
// GPIO ACCES
//------------------------------------------------------------------------------

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

#define GPIO_LED0   4
#define GPIO_LED1   17
#define GPIO_BP     18

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

#ifndef FREQ
#define FREQ 1
#endif

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
};

struct gpio_s *gpio_regs_virt; 


static void 
gpio_fsel(uint32_t pin, uint32_t fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs_virt->gpfsel[reg] = (gpio_regs_virt->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void 
gpio_write (uint32_t pin, uint32_t val)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    if (val == 1) 
        gpio_regs_virt->gpset[reg] = (1 << bit);
    else
        gpio_regs_virt->gpclr[reg] = (1 << bit);
}

//------------------------------------------------------------------------------
// Access to memory-mapped I/O
//------------------------------------------------------------------------------

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

static int mmap_fd;

static int
gpio_mmap ( void ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = mmap_result;

    return 0;
}

void
gpio_munmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}

//------------------------------------------------------------------------------
// Main Programm
//------------------------------------------------------------------------------

void
delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

// Function to blink led at frequency of 1/period
// ---------------------------------------------
void *blink_LED0(void* period) {
    
    uint32_t val = 0;                           // Val : value to write in the GPIO of LED port
    unsigned int T = *(unsigned int*)period;    // Convert the input period of type void* to unsigned* 
                                                // Stock the value in the T the value stock in the adresse
    //int cpt = 0;                              // Counter for the times that light blink(2x cause one blink is one time 0 and one time 1)
    //int nb_blink = 20;                        // Times the light going to blink
    printf ( "-- info: start blinking.\n" );    // Debug using To tell the light going to blink

    // Blink the light and add 1 to the counter
	//while(cpt < nb_blink*2){
	while(1){
    	gpio_write ( GPIO_LED0, val );          // Write in the port GPIO which LED0 correspond to 
        delay(T);                               // Wait for T(s)
        val = 1 - val;                          // Inverse the value of val '0' to '1' or '1' to '0'
    //        cpt++;                            // add 1 to counter
	}
	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);                 // send the signal to tell the main that the thread is done
}

// Fonction to blink led1 at frequency of 1/period
// ---------------------------------------------
void *blink_LED1(void* period) {
    
    uint32_t val = 0;                           // Val : value to write in the GPIO of LED port
    unsigned int T = *(unsigned int*)period;    // Convert the input period of type void* to unsigned* 
                                                // Stock the value in the T the value stock in the adresse
    //int cpt = 0;                              // Counter for the times that light blink(2x cause one blink is one time 0 and one time 1)
    //int nb_blink = 20;                        // Times the light going to blink
    printf ( "-- info: start blinking.\n" );    // Debug using To tell the light going to blink

    // Blink the light and add 1 to the counter
	//while(cpt < nb_blink*2){
	while(1){
		gpio_write ( GPIO_LED1, val );          // Write in the port GPIO which LED0 correspond to 
        delay(T);                               // Wait for T(s)
        val = 1 - val;                          // Inverse the value of val '0' to '1' or '1' to '0'
    //    cpt++;                                // add 1 to counter
	}
	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);                 // send the signal to tell the main that the thread is done
}

// Main function
int main ( int argc, char **argv )
{
    // Get arguments
    // ---------------------------------------------
    int period, half_period, used_period;  // Inistial the variable period, period/2, period/3 

    period = 1000;                              // Default = 1Hz
    if ( argc > 1 ) {
        period = atoi ( argv[1] );              
    }

    half_period = period / 2;                   // 2Hz

    used_period = period / FREQ;              // customed freqence 1Hz by default

    // uint32_t volatile * gpio_base = 0;
    

    // map GPIO registers
    // ---------------------------------------------

    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);

    
    // Create the pointer where we are going to create our thread
	pthread_t t1,t2;

    printf("Before the creation of threads.\n");                           // Debug using Point before threads created
	
    // Create the threads
	pthread_create(&t1, NULL, blink_LED0, (void *) &half_period); 
	pthread_create(&t2, NULL, blink_LED1, (void *) &used_period);
	printf("After the creation of threads.\n");                           // Debug using Point after threads created

    // Wait for the threads finsih
    pthread_join(t1,NULL);
    printf("thread1 finished.\n");
    pthread_join(t2,NULL);      
    printf("thread2 finished.\n"); 
    
    return 0;
}




