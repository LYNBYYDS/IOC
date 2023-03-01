// Driver for LCd HD44780 ( 20x4 )

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <mach/platform.h>

// Define the GPIO pins used to connect to the LCD
#define GPIO_BASE_ADR           0x20200000

#define GPIO_RS                      7
#define GPIO_E                       27
#define GPIO_D4                      22
#define GPIO_D5                      23
#define GPIO_D6                      24
#define GPIO_D7                      25

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

// Buffer to stock data copy from user space
char buffer[20];                    // a buffer to hold the data
unsigned long count_buffer = 20;    // number of bytes to copy
unsigned long offset_buffer = 0;    // offset in the user buffer

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
}

// Displays a string of characters on the LCD screen
void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };          // array of offset values for the LCD's DDRAM address
    int len = 20;                               // maximum number of characters per line
    int i = 0, l = 1;

    // iterate over each line of the LCD and write the corresponding text
    for (; (l < 4) && (i < strlen(txt)); l++) {
        lcd_command(LCD_SETDDRAMADDR + a[l]);   // set the DDRAM address for the current line
        for (; (i < (l + 1) * len) && (i < strlen(txt)); i++) {
            printk(KERN_DEBUG "%d", txt[i]);    // print the current character to the kernel log
            lcd_data(txt[i]);                   // write the current character to the LCD
        }
    }
}

// variable to store the major number of the device
static int major;


// This function is called when the device file is opened with open system call
static int 
open_lcd1_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "LCD1 initialised!\n");               //Print a debug message indicating that the LCD1 has been initialized
    printk(KERN_DEBUG "LCD1 can be write on!\n");           //Print a debug message indicating that the LCD1 is ready to be written on
    return 0;                                               //Return 0 to indicate successful completion
}

//This function is called when the device file is read from with read system call
static ssize_t 
read_lcd1_LA(struct file *file, char *buf, size_t count, loff_t *ppos) 
{
    printk(KERN_DEBUG "ERROR: Can not read on lcd1!\n");    //Print a debug message indicating that reading from the LCD1 is not supported
    return count;                                           //Return the count of bytes read, which is 0 in this case
}

//This function is called when the device file is written to with write system call
static ssize_t 
write_lcd1_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    
    printk(KERN_DEBUG "Writing on lcd1!\n");                //Print a debug message indicating the value that is being written to the LCD1
    // Copy the data from user space to kernel space, and write it to the LCD
    if (copy_from_user(buffer, buf + offset_buffer, count_buffer)) {
            // If there was an error copying the data, print a debug message
            printk(KERN_DEBUG "Error : can't copy from userspace to karnelspace\n");
        } else {
            lcd_message(buffer);                        // Otherwise, write the data to the LCD
    }
    return count;                                       //Return the count of bytes written, which is equal to the count parameter
}

//This function is called when the device file is closed with close system call
static int 
release_lcd1_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "LCD file closed!\n");            //Print a debug message indicating that the LCD1 has stopped working
    return 0;                                           //Return 0 to indicate successful completion
}

// Definition of file operations for LCD1 character device driver
struct file_operations fops_lcd1_LA = {
    .open       = open_lcd1_LA,                         // function to be called when the device is opened
    .read       = read_lcd1_LA,                         // function to be called when the device is read
    .write      = write_lcd1_LA,                        // function to be called when the device is written to
    .release    = release_lcd1_LA                       // function to be called when the device is closed
};

// Initialization function for the module
static int __init mon_module_init(void)
{
    // Registering the character device driver
    major = register_chrdev(0, "lcd1_LA", &fops_lcd1_LA);   // 0 To let linux choose the major number
    printk(KERN_DEBUG "lcd1_LA connected!\n");              // Indicates the start of the module
    lcd_init();                                             // Initialize an LCD (liquid crystal display) module
    lcd_clear();                                            // Clears the LCD screen and returns the cursor to the home position
    return 0;
}

// Cleanup function for the module
static void __exit mon_module_cleanup(void)
{
    lcd_terminate();                                        // Set all GPIO in unput => safer
    unregister_chrdev(major, "lcd1_LA");                    // Function to unload the driver
    printk(KERN_DEBUG "lcd1_LA deconnected!\n");            // Indicates the end of the module
}

// Register the init and cleanup functions with the kernel
module_init(mon_module_init);
module_exit(mon_module_cleanup);