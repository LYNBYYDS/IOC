#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

// Define the delay period for each frequency of 1Hz, 2Hz, 3Hz, and 50Hz
#define PERIOD_1HZ   1800
#define PERIOD_2HZ   500
#define PERIOD_3HZ   333
#define PERIOD_50HZ  20

// Define LED and button state constants
#define LED_ON  '1'
#define LED_OFF '0'
#define BUTTON_PRESSED '0'

// Function to delay for a specified amount of milliseconds
void delay(unsigned int milisec) {
    struct timespec ts, dummy;
    ts.tv_sec  = (time_t) milisec / 1000;
    ts.tv_nsec = (long) (milisec % 1000) * 1000000;
    nanosleep(&ts, &dummy);
}

int main() {
    // Declare variables to hold the state of the LED and button
    char led_state = LED_ON;
    char led_inv_state = LED_OFF;
    char button_state;
   
    // Open the device files for the LED and button
    int fdled0 = open("/dev/led0_LA", O_WRONLY);
    int fdled1 = open("/dev/led1_LA", O_WRONLY);
    int fdbp = open("/dev/bp_LA", O_RDONLY);
   
    // Check if the files were successfully opened
    if (fdled0 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LED0!\n");
        return 0;
    }
    if (fdled1 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LED1!\n");
        return 0;
    }
    if (fdbp < 0) {
        fprintf(stderr, "Error: Can not o   pen the driver for button!\n");
        return 0;
    }

    // Loop indefinitely to toggle the LED and read the state of the button
    while (1) {
        // Read the state of the button 
        read(fdbp, &button_state, 1);
        printf("%c\n", button_state);

        // If the button is pressed, turn the LED off, otherwise turn it on
        if (button_state == BUTTON_PRESSED) {
            led_state = LED_OFF;
        } else {
            led_state = LED_ON;
        }
        led_inv_state = (led_state == LED_OFF) ? LED_ON : LED_OFF;
      
        // Write the LED state to the LED device file
        if (write(fdled0, &led_state, 1) < 0) {
            fprintf(stderr, "Error: Failed to write to LED0 device file!\n");
            return 0;
        }
        if (write(fdled1, &led_inv_state, 1) < 0) {
            fprintf(stderr, "Error: Failed to write to LED1 device file!\n");
            return 0;
        }

        // Wait for 20 milliseconds
        delay(PERIOD_50HZ);
    }

    return 0;
}
