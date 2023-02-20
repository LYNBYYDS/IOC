#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
/*
#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct cord_xy)
*/

struct coord_xy {
    int coordY;
    int coordX;
};

int main() {
    char text[] = "j'en connais un qui a pas fini son TP de Multi il a le seum!          ";
    char text_test[] = "NAASDHTBSavfBS32LFGGNULLOK?   ";



    size_t nbbit = strlen(text);
    struct coord_xy pos = {2, 3}; // set cursor to row 2, column 3
   
    // Open the device files
    int fdlcd = open("/dev/lcd_LA", O_WRONLY);
    
   
    // Check if the files were successfully opened
    if (fdlcd < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD!\n");
        exit(1);
    }

while(1){

    write(fdlcd, &text, nbbit);
}


/*
   
    int ret = ioctl(fd, LCDIOCT_CLEAR, 0);
    if (ret == -1) {
        // handle error
    }

    
    int ret = ioctl(fd, LCDIOCT_SETXY, &pos);
    if (ret == -1) {
        // handle error
    }
*/

}