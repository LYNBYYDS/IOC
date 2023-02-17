#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

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
    // Declare variables to hold the state of the LED and button
    char led = '1';
    char led_inv = '0';
    char bp;
   
    // Open the device files for the LED and button
    int fdled0 = open("/dev/led0_LA", O_WRONLY);
    int fdled1 = open("/dev/led1_LA", O_WRONLY);
    int fdbp = open("/dev/bp_LA", O_RDONLY);
   
    // Check if the files were successfully opened
    if (fdled0 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LED0!\n");
        exit(1);
    }
    if (fdbp < 0) {
        fprintf(stderr, "Error: Can not open the driver for button!\n");
        exit(1);
    }
   
    // Loop indefinitely to toggle the LED and read the state of the button
    while (1) {
        // Read the state of the button 
        //read(fdbp, &bp, 1);
         bp = (bp == '0') ? '1' : '0';

        // If the button is pressed, turn the LED off, otherwise turn it on
        if (bp == '0') {
            led = '0';
        } else {
            led = '1';
        }
        led_inv = (led == '0') ? '1' : '0';
      
        // Write the LED state to the LED device file
        write(fdled0, &led, 1);
        write(fdled1, &led_inv, 1);

        // Wait for 20 milliseconds
        delay(PERIOD_2HZ);
    }

    return 0;
}
