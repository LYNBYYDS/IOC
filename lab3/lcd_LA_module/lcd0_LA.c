/*******************************************************************************
 * lcd_LA.c - Controleur pour LCd HD44780 ( 20x4 )
 ******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <mach/platform.h>

/*******************************************************************************
 * GPIO Pins
 ******************************************************************************/

#define RPI_GPIO_BASE 0x20200000

#define RS                      7
#define E                       27
#define D4                      22
#define D5                      23
#define D6                      24
#define D7                      25

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module lcd");

char buffer[20]; // a buffer to hold the data
unsigned long count_buffer = 20; // number of bytes to copy
unsigned long offset_buffer = 0; // offset in the user buffer

// Structure to represent the layout of the memory-mapped I/O for the Raspberry Pi's GPIO
struct gpio_s
{
    uint32_t gpfsel[7]; // Function select registers
    uint32_t gpset[3]; // Pin output set registers
    uint32_t gpclr[3]; // Pin output clear registers
    uint32_t gplev[3]; // Pin level registers
    uint32_t gpeds[3]; // Pin event detect status registers
    uint32_t gpren[3]; // Pin rising edge detect enable registers
    uint32_t gpfen[3]; // Pin falling edge detect enable registers
    uint32_t gphen[3]; // Pin high detect enable registers
    uint32_t gplen[3]; // Pin low detect enable registers
    uint32_t gparen[3]; // Pin async rising edge detect registers
    uint32_t gpafen[3]; // Pin async falling edge detect registers
    uint32_t gppud[1]; // Pin pull-up/down enable register
    uint32_t gppudclk[3]; // Pin pull-up/down clock register
    uint32_t test[1]; // Test register
}
// Pointer to the base address of the GPIO I/O
volatile *gpio_regs = (struct gpio_s *)__io_address(RPI_GPIO_BASE);


/* gpio_fsel - function to configure the function of a specific GPIO pin */
static void 
gpio_fsel(int pin, int fun)
{
    /* Calculate the register number for the given pin */
    uint32_t reg = pin / 10;

    /* Calculate the bit position for the given pin */
    uint32_t bit = (pin % 10) * 3;

    /* Create a mask for the register */
    uint32_t mask = 0b111 << bit;

    /* Update the register field for the given pin */
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

// Function to write a value to a specified GPIO pin
static void 
gpio_write(int pin, bool val)
{
    // Check the value to be written to the GPIO pin
    if (val) {
        // If the value is true, set the specified pin using the set register
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    } else {
        // If the value is false, clear the specified pin using the clear register
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
    }
}

/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

/* generate E signal */
void lcd_strobe(void)
{   
    gpio_write(E, 1);
    udelay(1);
    gpio_write(E, 0);
}

/* send 4bits to LCD : valable pour les commande et les data */

void lcd_write4bits(unsigned char data, int mode) {
    // set the RS pin based on the mode (0 = command, 1 = data)
    gpio_write(RS, mode);

    // write the first 4 bits
    gpio_write(D4, (data >> 4) & 0x1);
    gpio_write(D5, (data >> 5) & 0x1);
    gpio_write(D6, (data >> 6) & 0x1);
    gpio_write(D7, (data >> 7) & 0x1);

    // pulse the enable pin to latch the data
    lcd_strobe();
    udelay(50);

    // write the second 4 bits
    gpio_write(D4, (data >> 0) & 0x1);
    gpio_write(D5, (data >> 1) & 0x1);
    gpio_write(D6, (data >> 2) & 0x1);
    gpio_write(D7, (data >> 3) & 0x1);

    // pulse the enable pin to latch the data
    lcd_strobe();
    udelay(50);
}

void lcd_command(int cmd)
{
    gpio_write(RS, 0);
    lcd_write4bits(cmd, MODE_COMMAND);
    udelay(2000);
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character, MODE_DATA);
    udelay(1);
}

void lcd_init(void)
{
    gpio_write(E, 0);

    gpio_write(RS, 0);
    gpio_write(D7, 0);
    gpio_write(D6, 0);
    gpio_write(D5, 1);
    gpio_write(D4, 1);
    gpio_write(E, 1);
    udelay(5);
    gpio_write(E, 0);
    gpio_write(RS, 0);
    gpio_write(D7, 0);
    gpio_write(D6, 0);
    gpio_write(D5, 1);
    gpio_write(D4, 1);
    gpio_write(E, 1);
    udelay(1);
    gpio_write(E, 0);
    udelay(2000);
    //lcd_command(0b00110011);    /* initialization */
    
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

// Q5: Expliquez comment fonctionne cette fonction 
void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };
    int len = 20;
    int i, l;

    for (i = 0, l = 0; (l < 4) && (i < strlen(txt)); l++) {
        lcd_command(LCD_SETDDRAMADDR + a[l]);
        for (; (i < (l + 1) * len) && (i < strlen(txt)); i++) {
            printk(KERN_DEBUG "%d", txt[i]);
            lcd_data(txt[i]);
        }
    }
}

/* major - variable to store the major number of the device */
static int major;


//This function is called when the device file is opened with open system call
static int 
open_lcd0_LA(struct inode *inode, struct file *file) {

    //Print a debug message indicating that the LCD0 has been initialized
    printk(KERN_DEBUG "LCD0 initialised!\n");
    
    

    //Print a debug message indicating that the LCD0 is ready to be written on
    printk(KERN_DEBUG "LCD0 can be write on!\n");

    //Return 0 to indicate successful completion
    return 0;
}

//This function is called when the device file is read from with read system call
static ssize_t 
read_lcd0_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    //Print a debug message indicating that reading from the LCD0 is not supported
    printk(KERN_DEBUG "ERROR: Can not read on lcd0!\n");

    //Return the count of bytes read, which is 0 in this case
    return count;
}

//This function is called when the device file is written to with write system call
static ssize_t 
write_lcd0_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    //Print a debug message indicating the value that is being written to the LCD0
    printk(KERN_DEBUG "Writing on lcd0!\n");

    if (copy_from_user(buffer, buf + offset_buffer, count_buffer)) {
        printk(KERN_DEBUG "Error : can't copy from userspace to karnelspace\n");
    } else {
        lcd_message(&buffer[0]);
    }
   

    
    //Return the count of bytes written, which is equal to the count parameter
    return count;
}

//This function is called when the device file is closed with close system call
static int 
release_lcd0_LA(struct inode *inode, struct file *file) {
    //Print a debug message indicating that the LCD0 has stopped working
    printk(KERN_DEBUG "LCD file closed!\n");

    //Return 0 to indicate successful completion
    return 0;
}
// Definition of file operations for LCD0 character device driver
struct file_operations fops_lcd0_LA = {
    .open       = open_lcd0_LA,  // function to be called when the device is opened
    .read       = read_lcd0_LA,  // function to be called when the device is read
    .write      = write_lcd0_LA, // function to be called when the device is written to
    .release    = release_lcd0_LA // function to be called when the device is closed
};

// Initialization function for the module
static int __init mon_module_init(void)
{
    // Registering the character device driver
    major = register_chrdev(0, "lcd0_LA", &fops_lcd0_LA);  // 0 To let linux choose the major number
    printk(KERN_DEBUG "lcd0_LA connected!\n");

    /* Setting up GPIOs to output */
    gpio_fsel(RS, GPIO_FSEL_OUTPUT);
    gpio_fsel(E,  GPIO_FSEL_OUTPUT);
    gpio_fsel(D4, GPIO_FSEL_OUTPUT);
    gpio_fsel(D5, GPIO_FSEL_OUTPUT);
    gpio_fsel(D6, GPIO_FSEL_OUTPUT);
    gpio_fsel(D7, GPIO_FSEL_OUTPUT);

    /* initialization */
    lcd_init();
    lcd_clear();

    return 0;
}

// Cleanup function for the module
static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "lcd0_LA"); // Function to unload the driver
    printk(KERN_DEBUG "lcd0_LA deconnected!\n"); // Indicates the end of the module
}

// Register the init and cleanup functions with the kernel
module_init(mon_module_init);
module_exit(mon_module_cleanup);
