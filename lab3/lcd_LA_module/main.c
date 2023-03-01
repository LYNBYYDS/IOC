#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h> 

#define MAX_LENGTH 20 // Maximum length of each input string

int main() {
    
    char text0[MAX_LENGTH+1] = "j'en connais un";// Add one extra character for the null terminator
    char text1[MAX_LENGTH+1] = "qui a pas fini ";
    char text2[MAX_LENGTH+1] = "son TP de Multi";
    char text3[MAX_LENGTH+1] = "il a le seum";
/*
    char input_text0[MAX_LENGTH+1]; // Add one extra character for the null terminator
    char input_text1[MAX_LENGTH+1];
    char input_text2[MAX_LENGTH+1];
    char input_text3[MAX_LENGTH+1];
 */  
    // Open the device files
    int fdlcd0 = open("/dev/lcd0_LA", O_RDWR);
    int fdlcd1 = open("/dev/lcd1_LA", O_RDWR);
    int fdlcd2 = open("/dev/lcd2_LA", O_RDWR);
    int fdlcd3 = open("/dev/lcd3_LA", O_RDWR);
    
    int ok = 1;
    char input;


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

/*
    while (ok > 0) {
        printf("Do you want to print something custom? (Y/N)\n");
        fflush(stdin); // clear input buffer
        if (scanf("%c", &input) != 1) {
            // input error handling
            printf("Error reading input.\n");
            continue; // start the loop again
        }
        input = toupper(input); // convert input to uppercase
        switch (input) {
            case 'Y':
                printf("We will only take %d chars for each line!\n", MAX_LENGTH);
                printf("Enter your custom message:\n");
                // code to print custom message

                printf("Enter what you want to display in the first line:\n");
                fgets(input_text0, MAX_LENGTH+1, stdin);    
                printf("Enter what you want to display in the second line:\n");
                fgets(input_text1, MAX_LENGTH+1, stdin);
                printf("Enter what you want to display in the third line:\n");
                fgets(input_text2, MAX_LENGTH+1, stdin);
                printf("Enter what you want to display in the fourth line:\n");
                fgets(input_text3, MAX_LENGTH+1, stdin);
                ok = 0;
                break;
            case 'N':
                ok = 0;
                break;
            default:
                if (isspace(input)) {
                    // handle whitespace input
                    printf("Invalid input. Please enter Y or N.\n");
                } else {
                    // handle unexpected input
                    printf("Unexpected input. Please enter Y or N.\n");
                }
                break;
        }
    }
    getchar();
    if (!ok) {
        // code to write to LCD display
        write(fdlcd0, &input_text0, strlen(input_text0));
        write(fdlcd1, &input_text1, strlen(input_text1));
        write(fdlcd2, &input_text2, strlen(input_text2));
        write(fdlcd3, &input_text3, strlen(input_text3));
    }else{
        write(fdlcd0, &text0, strlen(text0));
        write(fdlcd1, &text1, strlen(text1));
        write(fdlcd2, &text2, strlen(text2));
        write(fdlcd3, &text3, strlen(text3));
    }*/
    return 0;
}