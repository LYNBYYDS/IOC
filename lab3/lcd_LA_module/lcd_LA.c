/*******************************************************************************
 * lcdr_user.c - Driver pour LCd HD44780 ( 20x4 )
 ******************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <mach/platform.h>

// Active the control caractere ASCII or not
#define ASCII_ON 1

// Define the GPIO pins used to connect to the LCD
#define GPIO_BASE_ADR           0x20200000

#define GPIO_RS                 7
#define GPIO_E                  27
#define GPIO_D4                 22
#define GPIO_D5                 23
#define GPIO_D6                 24
#define GPIO_D7                 25

#define GPIO_FSEL_INPUT         0
#define GPIO_FSEL_OUTPUT        1

//LCD's Instructions ( source = doc )
// Ces constantes sont utilisées pour former les mots de commandes

// Commands
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000

// Flags for display entry mode : combine with LCD_ENTRYMODESET
#define LCD_EM_RIGHT            0b00000000
#define LCD_EM_LEFT             0b00000010
#define LCD_EM_DISPLAYSHIFT     0b00000001
#define LCD_EM_DISPLAYNOSHIFT   0b00000000

// Flags for display on/off control : combine with LCD_DISPLAYCONTROL
#define LCD_DC_DISPLAYON        0b00000100
#define LCD_DC_DISPLAYOFF       0b00000000
#define LCD_DC_CURSORON         0b00000010
#define LCD_DC_CURSOROFF        0b00000000
#define LCD_DC_BLINKON          0b00000001
#define LCD_DC_BLINKOFF         0b00000000

// Flags for display/cursor shift : combine with LCD_CURSORSHIFT
#define LCD_CS_DISPLAYMOVE      0b00001000
#define LCD_CS_CURSORMOVE       0b00000000
#define LCD_CS_MOVERIGHT        0b00000100
#define LCD_CS_MOVELEFT         0b00000000

// Flags for function set : combine with LCD_FUNCTIONSET
#define LCD_FS_8BITMODE         0b00010000
#define LCD_FS_4BITMODE         0b00000000
#define LCD_FS_2LINE            0b00001000
#define LCD_FS_1LINE            0b00000000
#define LCD_FS_5x10DOTS         0b00000100
#define LCD_FS_5x8DOTS          0b00000000

// Constants used to distinguish between sending a command and sending data
#define LCD_MODE_COMMAND        0
#define LCD_MODE_DATA           1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module lcd");

struct coord_xy {
    int coordY;
    int coordX;
};

#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct coord_xy)


struct coord_xy cursor = {0, 0};
struct coord_xy cursor_pre;

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
}

// Pointer to the base address of the GPIO I/O
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE_ADR);

// Function to configure the function of a specific GPIO pin
static void 
gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;            // Calculate the register number for the given pin
    uint32_t bit = (pin % 10) * 3;      // Calculate the bit position for the given pin
    uint32_t mask = 0b111 << bit;       //Create a mask for the register
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);  // Update the register field for the given pin
}

// Function to write a value to a specified GPIO pin
static void 
gpio_write(int pin, bool val)
{
    // Check the value to be written to the GPIO pin
    if (val) {
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));     // If the value is true, set the specified pin using the set register
    } else {
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));     // If the value is false, clear the specified pin using the clear register
    }
}

/// LCD's Operations
 
// Function is designed to send a "strobe" signal to an LCD module. 
// Signal is used to indicate to the module that it should read data from its input pins.
void lcd_strobe(void)
{   
    // Set the value of the GPIO pin associated with the E (enable) signal to 1. 
    // Tell the LCD that it should read the current data on its input pins.
    gpio_write(GPIO_E, 1);           
    udelay(1);                  // A brief delay of 1 microsecond 
    // Set the value of the E pin to 0. 
    // Tell the LCD that it should process the data that it just received.
    gpio_write(GPIO_E, 0);           
}

// Write (in four bits mode) a data to an LCD (liquid crystal display) module.
void lcd_write4bits(unsigned char data, int mode) {
    // Indicate whether the data to be written is a command or actual data.
    gpio_write(GPIO_RS, mode);

    // Write the first 4 bits
    gpio_write(GPIO_D4, (data >> 4) & 0x1);
    gpio_write(GPIO_D5, (data >> 5) & 0x1);
    gpio_write(GPIO_D6, (data >> 6) & 0x1);
    gpio_write(GPIO_D7, (data >> 7) & 0x1);

    lcd_strobe();       // Pulse the enable pin to auses the LCD to latch the data on its input pins
    udelay(50);         // Waits for 50 microseconds 

    // Write the second 4 bits
    gpio_write(GPIO_D4, (data >> 0) & 0x1);
    gpio_write(GPIO_D5, (data >> 1) & 0x1);
    gpio_write(GPIO_D6, (data >> 2) & 0x1);
    gpio_write(GPIO_D7, (data >> 3) & 0x1);

    lcd_strobe();       // Pulse the enable pin to auses the LCD to latch the data on its input pins
    udelay(50);         // Waits for 50 microseconds 
}

// Send a command to an LCD (liquid crystal display) module
void lcd_command(int cmd)
{
    lcd_write4bits(cmd, LCD_MODE_COMMAND);          // Function writes the 8 bits of the command to the LCD
    udelay(2000);                                   // Waits for 2000 microseconds
                                                    // 2000 ms is to ensure that the LCD has enough time to process the command before the next operation
}
// Send data to an LCD (liquid crystal display) module
void lcd_data(int character)
{
    lcd_write4bits(character, LCD_MODE_DATA);       // Function writes the 8 bits of the data to the LCD
    udelay(1);                                      // Waits for 1 microseconds
}

// Initialize an LCD (liquid crystal display) module
void lcd_init(void)
{   
    int i;
    // Set all LCD pins to outputs
    gpio_fsel(GPIO_RS, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_E,  GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_D4, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_D5, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_D6, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_D7, GPIO_FSEL_OUTPUT);

    // Initialize LCD with specific sequence of commands
    gpio_write(GPIO_E, 0);

    gpio_write(GPIO_RS, 0);
    gpio_write(GPIO_D7, 0);
    gpio_write(GPIO_D6, 0);
    gpio_write(GPIO_D5, 1);
    gpio_write(GPIO_D4, 1);
    gpio_write(GPIO_E, 1);
    for(i = 0; i < 50; i++){
        udelay(100);                   // Wait for more than 4.1 ms
    }
    gpio_write(GPIO_E, 0);
    gpio_write(GPIO_RS, 0);
    gpio_write(GPIO_D7, 0);
    gpio_write(GPIO_D6, 0);
    gpio_write(GPIO_D5, 1);
    gpio_write(GPIO_D4, 1);
    gpio_write(GPIO_E, 1);
    udelay(1);
    gpio_write(GPIO_E, 0);
    udelay(200);                    // Wait for more than 100 us
    
    lcd_command(0b00110010); 
    lcd_command(LCD_FUNCTIONSET | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS); // Function set: 4-bit interface, 2-line display, 5x8 dot character font
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF);          // Display control: display on, cursor off, blink off
    lcd_command(LCD_ENTRYMODESET | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT);           // Entry mode set: increment mode, no shift
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSORON | LCD_DC_BLINKON);
}

void lcd_terminate(void)
{
    // Set all LCD pins to input
    gpio_fsel(GPIO_RS, GPIO_FSEL_INPUT);
    gpio_fsel(GPIO_E,  GPIO_FSEL_INPUT);
    gpio_fsel(GPIO_D4, GPIO_FSEL_INPUT);
    gpio_fsel(GPIO_D5, GPIO_FSEL_INPUT);
    gpio_fsel(GPIO_D6, GPIO_FSEL_INPUT);
    gpio_fsel(GPIO_D7, GPIO_FSEL_INPUT);
}

// Clears the LCD screen and returns the cursor to the home position
void lcd_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_RETURNHOME);
    cursor.coordX = 0;
    cursor.coordY = 0;
}

void lcd_set_cursor(const int x, const int y) {
    int x_s, j;

    lcd_command(LCD_RETURNHOME);

    /* Deplacement jusqu'a la bonne ligne */
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

    /* Deplacement jusqu'a la bonne colonne */
    for (x_s = 0; x_s < x; ++x_s) {
        lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
    }

    cursor.coordX = x;
    cursor.coordY = y;
}

void lcd_message(const char *txt, const long count)
{
    int i;
    
    // iterate over each line of the LCD and write the corresponding text
    for (i = 0; i < count; ) {
        int j, k, toprint = 1;

        printk(KERN_DEBUG "%d", txt[i]);    // print the current character to the kernel log
        
        // if we test ASCII control char
        #if ASCII_ON == 1
            if (txt[i] == '\0') {                                                       // NUL
                i = count;                                                              // pass all the rest char
                toprint = 0;                                                            // char no need print
            }else if (txt[i] == '\t') {                                                 // HT
                cursor_pre.coordX = cursor.coordX;
                cursor_pre.coordY = cursor.coordY;
                i++;                                                                    // we skip the HT
                cursor.coordX = cursor.coordX%4 == 0 ? cursor.coordX + 4 : ((cursor.coordX / 4) + 1) * 4; // pass to next tab spot
                if (cursor.coordX >= 20) {                                              // if it pass the capacity of the line 
                    cursor.coordY = cursor.coordY < 3 ? cursor.coordY + 1 : 0;          // pass coordY to next
                    cursor.coordX = 0;                                                  // set to the first bit
                }
                lcd_set_cursor(cursor.coordX, cursor.coordY);                           // set the cursor to next line 
                toprint = 0;                                                            // char no need print
            }else if (txt[i] == '\b') {                                                 // BS
                i++;                                                              // we skip the BS
                if(i > 0 && (txt[i-1] == '\t' || txt[i-1] == '\n' )){
                    cursor.coordX = cursor_pre.coordX;
                    cursor.coordY = cursor_pre.coordY;
                    lcd_set_cursor(cursor.coordX, cursor.coordY);                           // set the cursor to right place
                }else {
                    cursor.coordX = cursor.coordX != 0 ? cursor.coordX - 1 : 19;
                    if (cursor.coordX == 19) {                                          // if we go back to the last line                                                                    
                        cursor.coordY = cursor.coordY != 0 ? cursor.coordY - 1 : 3;     // pass coordY to last line
                    }
                    lcd_set_cursor(cursor.coordX, cursor.coordY);                       // set the cursor to last plce
                    lcd_data(' ');                                                      // erase the data
                    lcd_set_cursor(cursor.coordX, cursor.coordY);                       // set the cursor to right place
                }
                toprint = 0;                                                            // char no need print
            }else if (txt[i] == '\n') {                                      // LF
                cursor_pre.coordX = cursor.coordX;
                cursor_pre.coordY = cursor.coordY;
                i++;                                                              // we skip the LF
                cursor.coordX = 0;
                cursor.coordY = cursor.coordY < 3 ? cursor.coordY + 1 : 0;              // pass coordY to next
                lcd_set_cursor(cursor.coordX, cursor.coordY);                           // set the cursor to right place
                toprint = 0;                                                            // char no need print
            }else if (txt[i] == '\r') {                                      // CR
                i++;                                                              // we skip the LF
                cursor.coordX = 0;
                lcd_set_cursor(cursor.coordX, cursor.coordY);                           // set the cursor to right place
                toprint = 0;                                                            // char no need print
            }
            //https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 ESC
            /*
            else if(i + 2 < count && txt[i] == 'E' && txt[i+1] == 'S' && txt[i+2] == 'C'){
                i = i + 3;
                // Cursor Controls
                if(i + 1 < count && txt[i] == '[' && txt[i+1] == 'H'){                                          // ESC[H moves cursor to home position
                    i = i + 2;
                    cursor.coordX = 0;
                    cursor.coordY = 0;
                    lcd_set_cursor(cursor.coordX, cursor.coordY);                           // set the cursor to right place
                }else if(i + 1 < count && txt[i] == '[' && txt[i+1] == 'H'){                                          // ESC[H moves cursor to home position
                    i = i + 2;
                    cursor.coordX = 0;
                    cursor.coordY = 0;
                    lcd_set_cursor(cursor.coordX, cursor.coordY);                           // set the cursor to right place
                }else if (i + 8 < count && txt[i] == '[' && txt[i+1] == '{' && int(txt[i+2]) < 2 && int(txt[i+2]) > 0 && txt[i+3] == '}' && txt[i+4] == ';' && txt[i+5] = '{' && int(txt[i+6]) < 10 && int(txt[i+6]) > 0 && txt[i+7] == '}' && (txt[i+8] == 'H' || txt[i+8] == 'f'))
                toprint = 0;                                                            // char no need print
            }*/

        #endif
        
        
        // if we are not at the end of the input and char need to print
        if (i < count && toprint == 1){
            lcd_data(txt[i]);                                                       // write the current character to the LCD
            i++;                                                                    // pass to next bit of the char
            cursor.coordX = cursor.coordX < 19 ? cursor.coordX + 1 : 0;             // pass coordX to next 
            
            // if pass to next line
            if (cursor.coordX == 0){                                                
                for(k = 0; k < 1000; k++){                                          // Wait for 2 s when finish one line
                    udelay(2000);
                }
                cursor.coordY = cursor.coordY < 3 ? cursor.coordY + 1 : 0;          // pass coordY to next
                lcd_set_cursor(cursor.coordX, cursor.coordY);                       // set the cursor to next line
            }
        }

        for(j = 0; j < 100; j++){
            udelay(2000);
        }
    }
}

// variable to store the major number of the device
static int major;


// This function is called when the device file is opened with open system call
static int 
open_lcd_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "LCD initialised!\n");               //Print a debug message indicating that the LCD has been initialized
    printk(KERN_DEBUG "LCD can be write on!\n");           //Print a debug message indicating that the LCD is ready to be written on
    return 0;                                               //Return 0 to indicate successful completion
}

//This function is called when the device file is read from with read system call
static ssize_t 
read_lcd_LA(struct file *file, char *buf, size_t count, loff_t *ppos) 
{
    printk(KERN_DEBUG "ERROR: Can not read on lcd!\n");    //Print a debug message indicating that reading from the LCD is not supported
    return count;                                           //Return the count of bytes read, which is 0 in this case
}

//This function is called when the device file is written to with write system call
static ssize_t 
write_lcd_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    char buffer[count];                                     // a buffer to hold the data
    unsigned long count_buffer = count;                     // number of bytes to copy
    unsigned long offset_buffer = 0;                        // offset in the user buffer
    printk(KERN_DEBUG "Writing on lcd!\n");                //Print a debug message indicating the value that is being written to the LCD
    // Copy the data from user space to kernel space, and write it to the LCD
    // Buffer to stock data copy from user space
    if (copy_from_user(buffer, buf + offset_buffer, count_buffer)) {
            // If there was an error copying the data, print a debug message
            printk(KERN_DEBUG "Error : can't copy from userspace to karnelspace\n");
        } else {
            lcd_message(buffer, count_buffer);                            // Otherwise, write the data to the LCD
    }
    return count;                                           //Return the count of bytes written, which is equal to the count parameter
}

// This function is called when the device file is closed with close system call
static int 
release_lcd_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "LCD file closed!\n");                //Print a debug message indicating that the LCD has stopped working
    return 0;                                               //Return 0 to indicate successful completion
}

// This function handles the ioctl calls for an LCD device driver

static long 
ioctl_lcd_LA(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct coord_xy coord;
    printk(KERN_DEBUG "Ioctl_lcd ! \n");

    // Check the magic number of the device
    if(_IOC_TYPE(cmd) != IOC_MAGIC)
        return -EINVAL;
    
    // Handle each ioctl command
    switch(cmd){
        // Clear the LCD display and reset file position to 0
        case LCDIOCT_CLEAR:
            file->f_pos = 0; // Reset file position
            lcd_clear();     // Clear LCD display
            break;

        // Set the cursor position on the LCD display
        case LCDIOCT_SETXY:
            // Copy the X and Y coordinates from user space
            if(copy_from_user(&coord, (void*)arg, _IOC_SIZE(cmd)) != 0)
                return -EINVAL;
            // Update the cursor position and set it on the LCD display
            cursor.coordX = coord.coordX;
            cursor.coordY = coord.coordY;
            lcd_set_cursor(cursor.coordX, cursor.coordY);
            break;

        // If the ioctl command is not recognized, return an error
        default: 
            return -EINVAL;
    }

    // Return 0 to indicate success
    return 0;
}


// Definition of file operations for LCD character device driver
struct file_operations fops_lcd_LA = {
    .open           = open_lcd_LA,                          // function to be called when the device is opened
    .read           = read_lcd_LA,                          // function to be called when the device is read
    .write          = write_lcd_LA,                         // function to be called when the device is written to
    .unlocked_ioctl = ioctl_lcd_LA,                         // Pointer to function to handle ioctl calls        
    .release        = release_lcd_LA                        // function to be called when the device is closed
};

// Initialization function for the module
static int __init mon_module_init(void)
{
    // Registering the character device driver
    major = register_chrdev(0, "lcd_LA", &fops_lcd_LA);   // 0 To let linux choose the major number
    printk(KERN_DEBUG "lcd_LA connected!\n");              // Indicates the start of the module
    lcd_init();                                             // Initialize an LCD (liquid crystal display) module
    lcd_clear();                                            // Clears the LCD screen and returns the cursor to the home position
    return 0;
}

// Cleanup function for the module
static void __exit mon_module_cleanup(void)
{
    lcd_terminate();                                        // Set all GPIO in unput => safer
    unregister_chrdev(major, "lcd_LA");                    // Function to unload the driver
    printk(KERN_DEBUG "lcd_LA deconnected!\n");            // Indicates the end of the module
}

// Register the init and cleanup functions with the kernel
module_init(mon_module_init);
module_exit(mon_module_cleanup);