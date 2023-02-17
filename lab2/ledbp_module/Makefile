# Define constant variables
CARD_NUMB       = 23
ROUTER          = peri
LOGIN           = li_authier
LAB             = lab2

# Define the names of the kernel modules
MODULE_LED0		= led0_LA
MODULE_LED1		= led1_LA
MODULE_bp		= bp_LA

# Define paths for the cross compiler and kernel source
CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

# Add the modules to the list of objects to be built
obj-m           += $(MODULE_LED0).o
obj-m           += $(MODULE_LED1).o
obj-m           += $(MODULE_bp).o

all : clean default main.x upload uploadsh

# Default target to build the modules
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

# Target to clean up built files
clean:
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm *.x	

# Target to upload the kernel modules to the Raspberry Pi
upload:
	scp -P622$(CARD_NUMB) $(MODULE_LED0).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_LED1).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_bp).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) main.x pi@$(ROUTER):$(LOGIN)/$(LAB)

# Target to upload shell scripts to the Raspberry Pi
uploadsh:
	scp -P622$(CARD_NUMB) insdev.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) rmdev.sh pi@$(ROUTER):$(LOGIN)/$(LAB)

main.x: main.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS) $(CFLAGS)
