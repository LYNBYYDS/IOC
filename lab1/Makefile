NAME = li_authier # nom1_nom2
RPI ?= 23 # card_number

CROSS_COMPILE ?= bcm2708hardfp-
cond ?= 2
CFLAGS=-Wall -Wfatal-errors -O2 -Dcondition=$(cond)
LDFLAGS=-static  -lpthread 

help:
	@echo ""
	@echo "	make [option] :"	
	@echo ""
	@echo " 		all					clean all and recompile"
	@echo ""
	@echo "		source					include source"
	@echo ""
	@echo " 		LED_BLINK				compile the file for one LED blink"
	@echo " 		LED_thread				compile the file for one LED blink controled by thread"
	@echo " 		LEDs_threads				compile the file for two LED blink controled by threads"
	@echo " 		LED_BP					compile the file for LED controled by BP"
	@echo "							option [-B] [cond=1] : Telerupteur" 
	@echo "							option [-B] [cond=2] or default :  Last task"
	@echo ""
	@echo " 		clean					clean all except sources"
	@echo ""
	@echo " 		copy					compile then copy to card $(RPI) in directory $(NAME)"
	@echo ""

all: clean LED_BLINK LED_thread LEDs_threads LED_BP

LED_BLINK :  blink0.x 

LED_thread : blink0_pt.x

LEDs_threads : blink01_pt.x

LED_BP : blink01_bp_pt.x

copy: *.x
	scp -P 622$(RPI) $^ pi@peri:$(NAME)

%.x: %.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS) $(CFLAGS)

clean:
	rm -f *.o *.x *~ *.a
