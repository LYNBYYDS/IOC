#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>



// Define the delay period for each frequency of 1Hz, 2Hz, 3Hz, and 50Hz
#define PERIOD_1HZ   1800
#define PERIOD_2HZ   500
#define PERIOD_3HZ   333
#define PERIOD_50HZ  20


int main() {
    
    char text[2] = "LA";
   
    // Open the device files
    int fdlcd = open("/dev/lcd0_LA", O_RDWR);
   
    // Check if the files were successfully opened
    if (fdlcd < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD0!\n");
        exit(1);
    }

    write(fdlcd, &text, 2);
    while(1){}   

    return 0;
}