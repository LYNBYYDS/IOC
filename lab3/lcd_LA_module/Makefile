# Define constant variables
CARD_NUMB       = 23
ROUTER          = peri
LOGIN           = li_authier
LAB             = lab3/lcd_module

# Define the names of the kernel modules
MODULE_LCD0		= lcd0_LA
MODULE_LCD1		= lcd1_LA
MODULE_LCD2		= lcd2_LA
MODULE_LCD3		= lcd3_LA

# Define paths for the cross compiler and kernel source
CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

# Add the modules to the list of objects to be built
obj-m           += $(MODULE_LCD0).o
obj-m           += $(MODULE_LCD1).o
obj-m           += $(MODULE_LCD2).o
obj-m           += $(MODULE_LCD3).o

all : clean default main.x upload uploadsh

# Default target to build the modules
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

# Target to clean up built files
clean:
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm -f *.o *.x *~

# Target to upload the kernel modules to the Raspberry Pi
upload:
	scp -P622$(CARD_NUMB) $(MODULE_LCD0).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_LCD1).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_LCD2).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_LCD3).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) main.x pi@$(ROUTER):$(LOGIN)/$(LAB)

# Target to upload shell scripts to the Raspberry Pi
uploadsh:
	scp -P622$(CARD_NUMB) insdev.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) rmdev.sh pi@$(ROUTER):$(LOGIN)/$(LAB)

%.x: %.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS) $(CFLAGS)