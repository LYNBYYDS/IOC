#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/io.h>
#include <mach/platform.h>

#define GPIO_BASE_ADR 0x20200000
#define GPIO_LED1   17
#define GPIO_FSEL_OUTPUT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module led1");
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

// variable to store the major number of the device
static int major;


// This function is called when the device file is opened with open system call
static int 
open_led1_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "LED1 initialised!\n");               // Print a debug message indicating that the LED1 has been initialized
    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);                 // Setup the GPIO pin for LED1 as an output pin
    printk(KERN_DEBUG "LED1 can be write on!\n");           // Print a debug message indicating that the LED1 is ready to be written on
    return 0;
}

// This function is called when the device file is read from with read system call
static ssize_t 
read_led1_LA(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    printk(KERN_DEBUG "ERROR: Can not read on led1!\n");    // Print a debug message indicating that reading from the LED1 is not supported
    return count;                                           // Return the count of bytes read, which is 0 in this case
}

// This function is called when the device file is written to with write system call
static ssize_t 
write_led1_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) 
{    
    printk(KERN_DEBUG "Writing on led1! Value input : %d\n", buf[0]);       // Print a debug message indicating the value that is being written to the LED1

    // If the value being written is '0', turn off the LED1
    if (buf[0] == '0') {
        printk(KERN_DEBUG "Light off!\n");          // Print a debug message indicating that the LED1 has been turned off
        gpio_write(GPIO_LED1, 0);                   // Turn off the LED1 by writing 0 to the GPIO pin
    }   
    // If the value being written is '1', turn on the LED1
    else if (buf[0] == '1') {
        printk(KERN_DEBUG "Light on!\n");           // Print a debug message indicating that the LED1 has been turned on
        gpio_write(GPIO_LED1, 1);                   // Turn on the LED1 by writing 1 to the GPIO pin
    }
    return count;                                   // Return the count of bytes written, which is equal to the count parameter
}

// This function is called when the device file is closed with close system call
static int 
release_led1_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "LED stop work!\n");          // Print a debug message indicating that the LED1 has stopped working
    return 0;                                       // Return 0 to indicate successful completion
}
// Definition of file operations for LED1 character device driver
struct file_operations fops_led1_LA = 
{
    .open       = open_led1_LA,                     // function to be called when the device is opened
    .read       = read_led1_LA,                     // function to be called when the device is read
    .write      = write_led1_LA,                    // function to be called when the device is written to
    .release    = release_led1_LA                   // function to be called when the device is closed
};

// Initialization function for the module
static int __init mon_module_init(void)
{
    // Registering the character device driver
    major = register_chrdev(0, "led1_LA", &fops_led1_LA);   // 0 To let linux choose the major number
    printk(KERN_DEBUG "led1_LA connected!\n");              // Indicates the start of the module
    return 0;
}

// Cleanup function for the module
static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led1_LA");                    // Function to unload the driver
    printk(KERN_DEBUG "led1_LA deconnected!\n");            // Indicates the end of the module
}

// Register the init and cleanup functions with the kernel
module_init(mon_module_init);
module_exit(mon_module_cleanup);
