#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/io.h>
#include <mach/platform.h>

#define GPIO_BASE_ADR       0x20200000
#define GPIO_BP             18
#define GPIO_FSEL_INPUT     0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li_authier, 2023");
MODULE_DESCRIPTION("Module bp");

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
    uint32_t reg = pin / 10;                                // Calculate the register number for the given pin
    uint32_t bit = (pin % 10) * 3;                          // Calculate the bit position for the given pin
    uint32_t mask = 0b111 << bit;                           //Create a mask for the register
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);  // Update the register field for the given pin
}

// Function to read the value of a specific GPIO pin 
static int 
gpio_read(int pin)
{
    uint32_t reg = pin/32;                                  // Calculate the register number for the given pin
    uint32_t bit = pin%32;                                  // Calculate the bit position for the given pin
    return (gpio_regs->gplev[reg]&(1<<bit)) != 0;           // Read the value of the given pin and return the result
}

// variable to store the major number of the device
static int major;

// This function is called when the device file is opened with open system call
static int 
open_bp_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "bp initialised!\n");                 // Print a debug message indicating that the bp has been initialized
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);                    // Setup the GPIO pin for bp as an input pin
    printk(KERN_DEBUG "bp can be read!\n");                 // Print a debug message indicating that the bp is ready to be read
    return 0;
}

// Definition of the read function for the bp driver
static ssize_t 
read_bp_LA(struct file *file, char *buf, size_t count, loff_t *ppos) 
{
    printk(KERN_DEBUG "Reading bp!\n");                     // Log message indicating that the bp is being read
    // Check the value of the bp
    if (gpio_read(GPIO_BP)&0x1) {
        printk(KERN_DEBUG "bp : 1\n");                      // Log message indicating a high value for the bp
        buf[0] = '1';                                       // Store the high value in the buffer
    }else if (!(gpio_read(GPIO_BP)&0x1)) {
        printk(KERN_DEBUG "bp : 0\n");                      // Log message indicating a low value for the bp
        buf[0] = '0';                                       // Store the low value in the buffer
    }
    return count;                                           // Return the count to indicate successful completion of the read function
}

// Definition of the write function for the bp driver
static ssize_t 
write_bp_LA(struct file *file, const char *buf, size_t count, loff_t *ppos) 
{
    printk(KERN_DEBUG "ERROR: Can not write to bp!\n");     // Log message indicating that writing to the bp is not allowed
    return count;                                           // Return the count to indicate successful completion of the write function
}

// Definition of the release function for the bp driver
static int 
release_bp_LA(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "bp stop work!\n");                   // Log message indicating that the bp has stopped working
    return 0;                                               // Return 0 to indicate successful completion of the release function
}

struct file_operations fops_bp =
{
    .open       = open_bp_LA,                               // Open function is called when a user-space process opens the device file.
    .read       = read_bp_LA,                               // Read function is called when a user-space process reads the device file. 
    .write      = write_bp_LA,                              // Write function is called when a user-space process writes to the device file. 
    .release    = release_bp_LA                             // Release function is called when a user-space process closes the device file. 
};

// This function is called when the module is loaded into the kernel.
static int __init mon_module_init(void)
{
    // Register the character device driver using the register_chrdev function.
    // The first argument specifies the major number of the device file. 
    // A value of 0 means to let the kernel choose a free major number.
    // The second argument specifies the name of the device file, which will be "bp_LA".
    // The third argument is a pointer to the struct file_operations that define the functions 
    // to be called when the device file is opened, read, written, and closed.
    major = register_chrdev(0, "bp_LA", &fops_bp);        // 0 To let linux chose the major number
    printk(KERN_DEBUG "bp_LA connected!\n");
    return 0;
}

// This function is called when the module is unloaded from the kernel.
static void __exit mon_module_cleanup(void)
{
    // Unregister the character device driver using the unregister_chrdev function.
    // The first argument specifies the major number of the device file.
    // The second argument specifies the name of the device file.
    unregister_chrdev(major, "bp_LA");                     // Fonction pour dechatger le driver
    printk(KERN_DEBUG "bp_LA deconnected!\n");              // Indique la fin du module
}

// Macro that specifies the functions to be called when the module is loaded and unloaded.
module_init(mon_module_init);
module_exit(mon_module_cleanup);
