#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct cord_xy)

struct cord_xy {
    int line;
    int row;
} cord_xy;

int main() {
    size_t nbbit = 70;
    char text[] = "j'en connais un qui a pas fini son TP de Multi il a le seum!          ";

    struct cord_xy pos = {2, 3}; // set cursor to row 2, column 3
   
    // Open the device files
    int fdlcd = open("/dev/lcd_LA", O_WRONLY);
    
   
    // Check if the files were successfully opened
    if (fdlcd < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD!\n");
        exit(1);
    }
while(1){
    write(fdlcd, &text, nbbit);
    printf("finish!");
    sleep(60);
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