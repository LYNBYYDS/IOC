#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

// Define the delay period for each frequency of 1Hz, 2Hz, 3Hz, and 50Hz
#define PERIOD_1HZ   1800
#define PERIOD_2HZ   500
#define PERIOD_3HZ   333
#define PERIOD_50HZ  20

// Function to delay for a specified amount of milliseconds
void delay(unsigned int milisec) {
    struct timespec ts, dummy;
    ts.tv_sec  = (time_t) milisec / 1000;
    ts.tv_nsec = (long) (milisec % 1000) * 1000000;
    nanosleep(&ts, &dummy);
}



int main() {
    
    char* text = "helloLA";
   
    // Open the device files
    int fdlcd = open("/dev/lcd0_LA", O_RDONLY);
   
    // Check if the files were successfully opened
    if (fdlcd < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD0!\n");
        exit(1);
    }

    write(fdlcd, &text, 20);
    while(1){}
        //
    

    return 0;
}