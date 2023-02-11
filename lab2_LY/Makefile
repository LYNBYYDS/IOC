CARD_NUMB       = 23
ROUTER          = peri
LOGIN           = li_authier
LAB             = lab2

MODULE_LED0		= led0_LA
MODULE_bp		= bp_LA

CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE_LED0).o
obj-m           += $(MODULE_bp).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules
clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
upload:
	scp -P622$(CARD_NUMB) $(MODULE_LED0).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_LED0).ko pi@$(ROUTER):$(LOGIN)/$(LAB)

uploadsh:
	scp -P622$(CARD_NUMB) insdev.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) rmdev.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
