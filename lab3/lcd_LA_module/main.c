#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main() {
    
    char text0[20] = "j'en connais un";
    char text1[20] = "qui a pas fini ";
    char text2[20] = "son TP de Multi";
    char text3[20] = "il a le seum";
   
    // Open the device files
    int fdlcd0 = open("/dev/lcd0_LA", O_RDWR);
    int fdlcd1 = open("/dev/lcd1_LA", O_RDWR);
    int fdlcd2 = open("/dev/lcd2_LA", O_RDWR);
    int fdlcd3 = open("/dev/lcd3_LA", O_RDWR);
    
   
    // Check if the files were successfully opened
    if (fdlcd0 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD0!\n");
        exit(1);
    }
    if (fdlcd1 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD1!\n");
        exit(1);
    }
    if (fdlcd2 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD2!\n");
        exit(1);
    }
    if (fdlcd3 < 0) {
        fprintf(stderr, "Error: Can not open the driver for LCD3!\n");
        exit(1);
    }

    write(fdlcd0, &text0, strlen(text0));
    write(fdlcd1, &text1, strlen(text1));
    write(fdlcd2, &text2, strlen(text2));
    write(fdlcd3, &text3, strlen(text3));


    while(1){}   

    return 0;
}