#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
   // Declare variables to hold the state of the LED and button
   char led = '1';
   char bp;
   
   // Open the device files for the LED and button
   printf("open");
   int fdled0 = open("/dev/led0_LA", O_WRONLY);
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
   printf("Before join while");
   // Loop indefinitely to toggle the LED and read the state of the button
   while(1) {
      // Toggle the LED state
      //led = (led == '0') ? '1' : '0';
      //printf("%d\n", led);
      // Write the LED state to the LED device file
      //write(fdled0, &led, 1);
      
      read(fdbp, &bp, 1);
      write(fdled0, &bp, 1);

      // Wait for 1 second
      sleep(1);
      // Read the state of the button 
      //read(fdbp, &bp, 1);
   }

   return 0;
}
