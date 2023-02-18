CARD_NUMB       = 23
ROUTER          = peri
LOGIN           = li_authier
LAB             = lab2/module_test

MODULE          = module
MODULE_1para	= module_un_parametre
MODULE_nparas	= module_plus_parametre

CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE).o
obj-m           += $(MODULE_1para).o
obj-m           += $(MODULE_nparas).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules
clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
upload:
	scp -P622$(CARD_NUMB) $(MODULE).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_1para).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_nparas).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
