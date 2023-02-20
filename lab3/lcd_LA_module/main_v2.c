#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/ioctl.h>


#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct coord_xy)


struct coord_xy {
    int coordY;
    int coordX;
};

int main() {
    char text[] = "j'en connais un qui a pas fini son TP de Multi il a le seum!          ";
    char text_test[] = "NAASDHTBSavfBS32LFGGNULLOK?   ";
    size_t nbbit_text = strlen(text);
    size_t nb_text_test = strlen(text_test);
    struct coord_xy pos;
    pos.coordY = 2;
    pos.coordX = 5;
    
   
    // Open the device files
    int fdlcd = open("/dev/lcd_LA", O_WRONLY);
    
   
    // Check if the files were successfully opened
    if (fdlcd < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD!\n");
        exit(1);
    }

    write(fdlcd, &text_test, nb_text_test);

     // Send an ioctl command to clear the LCD display
    if (ioctl(fdlcd, LCDIOCT_CLEAR) < 0) {
        printf("Failed to send ioctl command: LCDIOCT_CLEAR\n");
        exit(1);
    }

    // Set the cursor position on the LCD display

    if (ioctl(fdlcd, LCDIOCT_SETXY, &pos) < 0) {
        printf("Failed to send ioctl command: LCDIOCT_SETXY\n");
        exit(1);
    }

    // Close the device file
    close(fdlcd);

}