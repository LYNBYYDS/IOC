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

#include "read_bp.h"
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

// Two kinds of functions :
//                              Set COND to 1, the LED1 start blinking when the BP is pressed once and stop blinking when the BP is pressed again
//                              Set COND to 2, the LED1 blinks with the same rithme of LED0 when the BP is pressed once
//                                                  the LED1 blinks with the oposite rithme of LED0 when the BP is pressed again
//                                                  the LED1 stop blinking when the BP is pressed again


#ifndef COND
#define COND 2
#endif

#ifndef FREQ
#define FREQ 3
#endif

// variable global pour le partage de l'information du bouton poussoir

int BP_ON;   // mis a 1 si le bouton a ete appuye, mis a 0 quand la tache qui attend l'appui a vu l'appui
int BP_OFF;  // mis a 1 si le bouton a ete relache, mis a 0 quand la tache qui attend le relachement a vu le relachement

int val_global = 0; // the val going to write in the port GPIO of LED

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

static uint32_t gpio_read(uint32_t pin){

    uint32_t reg = pin/32;
    uint32_t bit = pin%32;
    return (gpio_regs_virt->gplev[reg]&(1<<bit)) != 0;
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


// We use the function to detect the rising edge / falling edge 
// You need to enter period = 0.02*1000 to have a period of 20ms
// ---------------------------------------------
void *bouton(void* period) {  
    
    unsigned int T = *(unsigned int*)period;    // change the type of input to unsigned int

    int val_pre = 1;                           // memory of the previous state
    int val_new = 1;                           // memory of the new state

    printf ( "-- info: start reading.\n" );
	while(1){
        delay(T);                               // test every T/1000 second
        val_new = gpio_read ( GPIO_BP );
        if (val_pre != val_new){              // if the state changes
            if(val_new == 0){                  // if the new state is '0'
                BP_ON = 1;
            }else{                              // if the new state is '1'
                BP_OFF = 1;
            }
            val_pre = val_new;                // save the new state
        }

    }

	// Arret propre du thread
	pthread_exit(EXIT_SUCCESS);
}

// Function to blink LED0 at frequency of 1/period
// ---------------------------------------------
void *blink_LED0(void* period) {
    
    uint32_t val = 0;                           // Val : value to write in the GPIO of LED port
    unsigned int T = *(unsigned int*)period;    // Convert the input period of type void* to unsigned* 
                                                // Stock the value in the T the value stock in the adresse
                                                
    printf ( "-- info: start blinking LED0.\n" );    // Debug using To tell the light going to blink

    // Blink the light
	while(1){
		gpio_write ( GPIO_LED0, val );          // Write in the port GPIO which LED0 correspond to 
        	delay(T);                           // Wait for T(s)
        	val = 1 - val;                      // Inverse the value of val '0' to '1' or '1' to '0'
	}
    

	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);                 // send the signal to tell the main that the thread is done
}


// Function to blink LED0 at frequency of 1/period and using global value for val
// ---------------------------------------------
void *blink_LED0_global_value(void* period) {
    
    unsigned int T = *(unsigned int*)period;    // Convert the input period of type void* to unsigned* 
                                                // Stock the value in the T the value stock in the adresse
                                                
    printf ( "-- info: start blinking LED0.\n" );    // Debug using To tell the light going to blink

    // Blink the light
	while(1){
		gpio_write ( GPIO_LED0, val_global );   // Write in the port GPIO which LED0 correspond to 
        	delay(T);                           // Wait for T(s)
        	val_global = 1 - val_global;        // Inverse the value of val_global '0' to '1' or '1' to '0'
	}
    

	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);                 // send the signal to tell the main that the thread is done
}


// Fonction to télérupteur LED1 at frequency of 1/period when press the button
// ---------------------------------------------
void *blink_LED1_when_press_button(void* period) {
    
    uint32_t val = 0;                           // Val : value to write in the GPIO of LED port
    unsigned int T = *(unsigned int*)period;    // Convert the input period of type void* to unsigned* 
                                                // Stock the value in the T the value stock in the adresse
     
    int clignote = 0;                           // variable to tell the Led blink or not 
                                                // inisial to '0' for not blink                     
    int cpt =  0;                               // counter to slow down the frequence of blinking

    printf ( "-- info: start LED1 .\n" );
	while(1){
        if (BP_ON == 1){                            // press the button
            clignote = 1 - clignote;                // change the LED to blink
            BP_ON = 0;                              // reinsital the value global
        }
        
        if (clignote == 1){
            gpio_write ( GPIO_LED1, val );          // affich the value val with the LED1
            cpt++;                                  // add 1 to counter
            if(cpt == 4){                           // freaquence of the light blink is 1/(period*5)
                val = 1 - val;                      // Inverse the value of val '0' to '1' or '1' to '0'
                cpt = 0;                            // reset counter
            }
        }else{
            gpio_write ( GPIO_LED1, 0 );            // close LED1 when clignote is unable
        }
        delay(T);                                   // Wait for T(s)
	}
    
	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);                 // send the signal to tell the main that the thread is done
}

// Fonction to télérupteur LED1 at frequency of 1/period when press the button
// ---------------------------------------------
void *blink_LED1_global_value(void* period) {
    
    unsigned int T = *(unsigned int*)period;    // Convert the input period of type void* to unsigned* 
                                                // Stock the value in the T the value stock in the adresse
     
    int etat =  0;                              // etat == 0 : LED1 close
                                                // etat == 1 : LED1 blink en phase with LED0
                                                // etat == 2 : LED1 blink en phase oppsite with LED0

    printf ( "-- info: start LED1 .\n" );
	while(1){
        if (BP_ON == 1){                            // press the button
            etat = (etat+1)%3;                     // pass to next etat
            BP_ON = 0;                              // reinsital the value global
        }
        
        if (etat == 0){
            gpio_write ( GPIO_LED1, 0 );            // close LED1 when etat == 0
        }else if (etat == 1){
            gpio_write ( GPIO_LED1, val_global );   // affich the value val_global with the LED1
        }else{
            gpio_write ( GPIO_LED1, 1-val_global ); // affich the value 1-val_global with the LED1   
        }
        delay(T);                                   // Wait for T(s)
	}
    
	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);                 // send the signal to tell the main that the thread is done
}

int main ( int argc, char **argv )
{
    // Get args
    // ---------------------------------------------
    int period, used_period, bouton_periode;   // , half_period

    period = 1000; /* default = 1Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    
    used_period = period / FREQ;
    bouton_periode = period/50; 

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
    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);
    
    
    // Setup GPIO of BP to input
    // ---------------------------------------------
    
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    

    
	// Create the pointer where we are going to create our thread
	pthread_t t1,t2,t3;

	printf("Before the creation of threads.\n");                           // Debug using Point before threads created
	// Create the threads
	
    #if COND == 1
    pthread_create(&t1, NULL, blink_LED1_when_press_button, (void *) &bouton_periode);
    pthread_create(&t2, NULL, blink_LED0, (void *) &used_period);
    #elif COND == 2
    pthread_create(&t1, NULL, blink_LED1_global_value, (void *) &bouton_periode);
    pthread_create(&t2, NULL, blink_LED0_global_value, (void *) &used_period);
    #endif

	
    pthread_create(&t3, NULL, bouton, (void *) &bouton_periode);
	printf("After the creation of threads.\n");                           // Debug using Point after threads created

    // Wait for the threads finsih
    pthread_join(t1,NULL); 
    printf("thread1 finished.\n"); 
    pthread_join(t2,NULL);
    printf("thread2 finished.\n");
    pthread_join(t3,NULL);         
    printf("thread3 finished.\n");
   
    return 0;
}