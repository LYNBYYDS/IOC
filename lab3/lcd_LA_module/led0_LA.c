/*******************************************************************************
 * lcd_LA.c - Controleur pour LCd HD44780 ( 20x4 )
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>

/*******************************************************************************
 * GPIO Pins
 ******************************************************************************/
#define RPI_PERIPH_BASE     0x20000000
#define RPI_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )
#define RPI_BLOCK_SIZE          0xB4

#define RS                      7
#define E                       27
#define D4                      22
#define D5                      23
#define D6                      24
#define D7                      25

#define GPIO_LED0               4
#define GPIO_LED1               17
#define GPIO_BP                 18

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module lcd");

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
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

void gpio_teardown(void)
{
    munmap((void *) gpio_regs, RPI_BLOCK_SIZE);
}

/* gpio_config - function to configure the function of a specific GPIO pin */
static void 
gpio_config(int pin, int fun)
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

/* major - variable to store the major number of the device */
static int major;


//This function is called when the device file is opened with open system call
static int 
open_led0_LA(struct inode *inode, struct file *file) {

    //Print a debug message indicating that the LED0 has been initialized
    printk(KERN_DEBUG "LED0 initialised!\n");
    
    // Setup the GPIO pin for LED0 as an output pin
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);

    //Print a debug message indicating that the LED0 is ready to be written on
    printk(KERN_DEBUG "LED0 can be write on!\n");

    //Return 0 to indicate successful completion
    return 0;
}

//This function is called when the device file is read from with read system call
static ssize_t 
read_led0_LA(struct file *file, char *buf, size_t count, loff_t *ppos) {
    //Print a debug message indicating that reading from the LED0 is not supported
    printk(KERN_DEBUG "ERROR: Can not read on led0!\n");

    //Return the count of bytes read, which is 0 in this case
    return count;
}

//This function is called when the device file is written to with write system call
static ssize_t 
write_led0_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    //Print a debug message indicating the value that is being written to the LED0
    printk(KERN_DEBUG "Writing on led0! Value input : %d\n", buf[0]);

    //If the value being written is '0', turn off the LED0
    if (buf[0] == '0') {
        //Print a debug message indicating that the LED0 has been turned off
        printk(KERN_DEBUG "Light off!\n");

        //Turn off the LED0 by writing 0 to the GPIO pin
        gpio_write(GPIO_LED0, 0);
    }
    //If the value being written is '1', turn on the LED0
    else if (buf[0] == '1') {
        //Print a debug message indicating that the LED0 has been turned on
        printk(KERN_DEBUG "Light on!\n");

        //Turn on the LED0 by writing 1 to the GPIO pin
        gpio_write(GPIO_LED0, 1);
    }

    //Return the count of bytes written, which is equal to the count parameter
    return count;
}

//This function is called when the device file is closed with close system call
static int 
release_led0_LA(struct inode *inode, struct file *file) {
    //Print a debug message indicating that the LED0 has stopped working
    printk(KERN_DEBUG "LED stop work!\n");

    //Return 0 to indicate successful completion
    return 0;
}
// Definition of file operations for LED0 character device driver
struct file_operations fops_led0_LA = {
    .open       = open_led0_LA,  // function to be called when the device is opened
    .read       = read_led0_LA,  // function to be called when the device is read
    .write      = write_led0_LA, // function to be called when the device is written to
    .release    = release_led0_LA // function to be called when the device is closed
};

// Initialization function for the module
static int __init mon_module_init(void)
{
    // Registering the character device driver
    major = register_chrdev(0, "led0_LA", &fops_led0_LA);  // 0 To let linux choose the major number
    printk(KERN_DEBUG "led0_LA connected!\n");
}

// Cleanup function for the module
static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led0_LA"); // Function to unload the driver
    printk(KERN_DEBUG "led0_LA deconnected!\n"); // Indicates the end of the module
}

// Register the init and cleanup functions with the kernel
module_init(mon_module_init);
module_exit(mon_module_cleanup);
