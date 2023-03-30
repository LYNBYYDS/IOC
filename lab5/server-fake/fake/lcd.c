/*******************************************************************************
 * lcdr_user.c - Controleur pour LCd HD44780 ( 20x4 )
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// Define the GPIO pins used to connect to the LCD
#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1

#define RPI_BLOCK_SIZE  0xB4
#define RPI_GPIO_BASE   0x20200000

/*******************************************************************************
 * LCD's Instructions ( source = doc )
 * Ces constantes sont utilisées pour former les mots de commandes
 * par exemple dans la fonction lcd_init()
 ******************************************************************************/

/* commands */
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            0b00000000
#define LCD_EM_LEFT             0b00000010
#define LCD_EM_DISPLAYSHIFT     0b00000001
#define LCD_EM_DISPLAYNOSHIFT   0b00000000

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        0b00000100
#define LCD_DC_DISPLAYOFF       0b00000000
#define LCD_DC_CURSORON         0b00000010
#define LCD_DC_CURSOROFF        0b00000000
#define LCD_DC_BLINKON          0b00000001
#define LCD_DC_BLINKOFF         0b00000000

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      0b00001000
#define LCD_CS_CURSORMOVE       0b00000000
#define LCD_CS_MOVERIGHT        0b00000100
#define LCD_CS_MOVELEFT         0b00000000

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         0b00010000
#define LCD_FS_4BITMODE         0b00000000
#define LCD_FS_2LINE            0b00001000
#define LCD_FS_1LINE            0b00000000
#define LCD_FS_5x10DOTS         0b00000100
#define LCD_FS_5x8DOTS          0b00000000

#define MODE_COMMAND    0
#define MODE_DATA       1

// Structure to represent the layout of the memory-mapped I/O for the Raspberry Pi's GPIO
struct gpio_s
{
    uint32_t gpfsel[7];             // Function select registers
    uint32_t gpset[3];              // Pin output set registers
    uint32_t gpclr[3];              // Pin output clear registers
    uint32_t gplev[3];              // Pin level registers
    uint32_t gpeds[3];              // Pin event detect status registers
    uint32_t gpren[3];              // Pin rising edge detect enable registers
    uint32_t gpfen[3];              // Pin falling edge detect enable registers
    uint32_t gphen[3];              // Pin high detect enable registers
    uint32_t gplen[3];              // Pin low detect enable registers
    uint32_t gparen[3];             // Pin async rising edge detect registers
    uint32_t gpafen[3];             // Pin async falling edge detect registers
    uint32_t gppud[1];              // Pin pull-up/down enable register
    uint32_t gppudclk[3];           // Pin pull-up/down clock register
    uint32_t test[1];               // Test register
};

// Pointer to the base address of the GPIO I/O
volatile struct gpio_s *gpio_regs;

// Function to configure the function of a specific GPIO pin
int gpio_setup(void)
{

    int mmap_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mmap_fd < 0) {
        return -1;
    }
    gpio_regs = mmap(NULL, RPI_BLOCK_SIZE,
                     PROT_READ | PROT_WRITE, MAP_SHARED,
                     mmap_fd,
                     RPI_GPIO_BASE);
    if (gpio_regs == MAP_FAILED) {
        close(mmap_fd);
        return -1;
    }
    return 0;
}


void gpio_teardown(void)
{
    munmap((void *) gpio_regs, RPI_BLOCK_SIZE);
}

void gpio_config(int gpio, int value)
{
    int regnum = gpio / 10;
    int offset = (gpio % 10) * 3;
    gpio_regs->gpfsel[regnum] &= ~(0x7 << offset);
    gpio_regs->gpfsel[regnum] |= ((value & 0x7) << offset);
}

void gpio_write(int gpio, int value)
{
    int regnum = gpio / 32;
    int offset = gpio % 32;
    if (value&1)
        gpio_regs->gpset[regnum] = (0x1 << offset);
    else
        gpio_regs->gpclr[regnum] = (0x1 << offset);
}

/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

// Function is designed to send a "strobe" signal to an LCD module. 
void lcd_strobe(void)
{
    gpio_write(E, 1);
    usleep(1);
    gpio_write(E, 0);
}

// Write (in four bits mode) a data to an LCD (liquid crystal display) module.
void lcd_write4bits(unsigned char data, int mode) {
    // Indicate whether the data to be written is a command or actual data(0 = command, 1 = data)
    gpio_write(RS, mode);

    // write the first 4 bits
    gpio_write(D4, (data >> 4) & 0x1);
    gpio_write(D5, (data >> 5) & 0x1);
    gpio_write(D6, (data >> 6) & 0x1);
    gpio_write(D7, (data >> 7) & 0x1);

    lcd_strobe();       // Pulse the enable pin to auses the LCD to latch the data on its input pins
    usleep(50);         // Waits for 50 microseconds 

    // write the second 4 bits
    gpio_write(D4, (data >> 0) & 0x1);
    gpio_write(D5, (data >> 1) & 0x1);
    gpio_write(D6, (data >> 2) & 0x1);
    gpio_write(D7, (data >> 3) & 0x1);

    lcd_strobe();       // Pulse the enable pin to auses the LCD to latch the data on its input pins
    usleep(50);         // Waits for 50 microseconds 
}

void lcd_command(int cmd)
{
    gpio_write(RS, 0);
    lcd_write4bits(cmd, MODE_COMMAND);              // Function writes the 8 bits of the command to the LCD
    usleep(2000);                                   // Waits for 2000 microseconds
                                                    // 2000 ms is to ensure that the LCD has enough time to process the command before the next operation
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character, MODE_DATA);            // Function writes the 8 bits of the data to the LCD
    usleep(1);                                      // Waits for 1 microseconds
}

/* initialization : pour comprendre la séquence, il faut regarder le cours */
// Q4: Expliquer le rôle des masques : LCD_FUNCTIONSET, LCD_FS_4BITMODE, etc.
void lcd_init(void)
{
    gpio_write(E, 0);
    usleep(15000);
    lcd_command(0b00110011);    /* initialization */
    lcd_command(0b00110010);    /* initialization */
    lcd_command(LCD_FUNCTIONSET | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS);
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF);
    lcd_command(LCD_ENTRYMODESET | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT);
}

void lcd_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_RETURNHOME);
}

void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };
    int len = 20;
    int i, l;

    for (i = 0, l = 0; (l < 4) && (i < strlen(txt)); l++) {
        lcd_command(LCD_SETDDRAMADDR + a[l]);
        for (; (i < (l + 1) * len) && (i < strlen(txt)); i++) {
            lcd_data(txt[i]);
        }
    }
}
// This function sets the position of the cursor on an LCD display
void lcd_set_cursor(const int x, const int y)
{
    int x_s, j, i;
    lcd_command(LCD_RETURNHOME); // Return the cursor to the home position (upper-left corner)
    
    // Move the cursor to the desired row (y)
    if (y == 1){
        for ( j = 0; j < 40; ++j) {
            lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
        }
    }else if (y == 2){
        for ( j = 0; j < 20; ++j) {
            lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
        }
    }else if (y == 3){
        for ( j = 0; j < 60; ++j) {
            lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
        }
    }
    
    // Move the cursor to the desired column (x)
    for (x_s = 0; x_s < x; ++x_s) {
        lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
    }
}

// This is an alternative function to set the position of the cursor on an LCD display
void lcd_set_cursor_V2(const int x, const int y)
{
    int a[] = { 0, 0x40, 0x14, 0x54 }; // Array to hold the addresses of the start of each row

    // Move the cursor to the desired row and column using the DDRAM address and the values in the array a[]
    lcd_command(LCD_SETDDRAMADDR + a[y] + x);
}

// This function writes a message on the LCD display, split into four rows
void lcd_message_bis(const char *txt){
    int len = 20; // Maximum length of each row
    int i, y;
    for(y = 0; y < 4; y++){ // Loop through each row
        lcd_set_cursor_V2(0, y); // Move the cursor to the start of the row
        for (i = 0; i < strlen(txt) && i < len; i++) {
            lcd_data(txt[i]); // Write the characters of the message one by one
        }   
    }
}

#define MAXServerResquest 1024



int main()
{


    int     f2s, s2f;                                       // fifo file descriptors
    char    *f2sName = "/tmp/f2s_fw";                       // fifo names
    char    *s2fName = "/tmp/s2f_fw";                       //
    char    serverRequest[MAXServerResquest];               // buffer for the request
    char    message[MAXServerResquest];
    char    message_precedent;
    fd_set  rfds;                                           // flag for select
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;                                         //

    mkfifo(s2fName, 0666);                                  // fifo creation
    mkfifo(f2sName, 0666);

    /* open both fifos */
    s2f = open(s2fName, O_RDWR);                            // fifo openning
    f2s = open(f2sName, O_RDWR);


    int i;
    /* arg */

    /* Retreive the mapped GPIO memory */
    if (gpio_setup() == -1) {
        perror("ERROR: gpio_setup\n");
        exit(1);
    }

    /* Setting up GPIOs to output */
    gpio_config(RS, GPIO_OUTPUT);
    gpio_config(E,  GPIO_OUTPUT);
    gpio_config(D4, GPIO_OUTPUT);
    gpio_config(D5, GPIO_OUTPUT);
    gpio_config(D6, GPIO_OUTPUT);
    gpio_config(D7, GPIO_OUTPUT);

    /* initialization */
    lcd_init();
    lcd_clear();

    /* change the place of the mouse*/
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSORON | LCD_DC_BLINKON);
    lcd_set_cursor(1,3);


    do {
        FD_ZERO(&rfds);                                     // erase all flags
        FD_SET(s2f, &rfds);                                 // wait for s2f

        if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
            if (FD_ISSET(s2f, &rfds)) {                     // something to read
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
                
                strncpy(message, serverRequest, MAXServerResquest);  // On copie dans message la valeur obtenu dans la fifo
                message[nbchar-1]=0;                                 // On retire le dernier caractere qui ne fait pas partie du texte
                
                if (message_precedent != message){          // On verifie que le nouveau message est different
                    lcd_message(message + 2);               // On affiche le message sans les 2 premiers caracteres qui sont toujour "w "
                    message_precedent = message;            // On met a jour la valeur de message_precedent
                }

                fprintf(stderr,"%s", serverRequest);
                write(f2s, serverRequest, nbchar);
            }
        }
    }
    while (1);

    close(f2s);
    close(s2f);

    return 0;
}
