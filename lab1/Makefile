# Define variables that can be modified

NAME = li_authier # Name of the user
RPI ?= 23 # Card number

CROSS_COMPILE ?= bcm2708hardfp-
cond ?= 2 # Condition variable for LED_BP program
freq ?= 1 # Condition variable for all program
CFLAGS = -Wall -Wfatal-errors -O2 -DCOND=$(cond) -DFREQ=$(freq)
LDFLAGS = -static -lpthread

# Define phony targets

.PHONY: all help clean copy

# Define help message for Makefile

help:
	@echo ""
	@echo "	make [option] :"	
	@echo ""
	@echo "		all         clean all and recompile"
	@echo ""
	@echo "		source      include source"
	@echo ""
	@echo "		LED_BLINK   compile program for one LED blink"
	@echo "		LED_thread  compile program for one LED blink controlled by thread"
	@echo "		LEDs_threads compile program for two LED blink controlled by threads"
	@echo "		LED_BP      compile program for LED controlled by button"
	@echo ""
	@echo "		            option [-B] [-Dcond=1]: control using a teleruptor"
	@echo "		            option [-B] [-Dcond=2] or default: control using push button"
	@echo "		            option [-B] [-Dfreq=1]: frquence customized to freq Hz"
	@echo ""
	@echo "		clean       clean all except sources"
	@echo ""
	@echo "		copy        compile programs and copy to card $(RPI) in directory $(NAME)"
	@echo ""

# Define targets for building and copying programs

all: clean LED_BLINK LED_thread LEDs_threads LED_BP

LED_BLINK: blink0.x

LED_thread: blink0_pt.x

LEDs_threads: blink01_pt.x

LED_BP: blink01_bp_pt.x

copy: all
	scp -P 	622$(RPI) *.x pi@peri:$(NAME)

# Define pattern rule for building programs from C source files

%.x: %.c
	$(CROSS_COMPILE)gcc -o $@ $< $(CFLAGS) $(LDFLAGS)

# Define clean target for removing built files

clean:
	rm -f *.o *.x *~ *.a

# Define targets for building the different programs and add comments

blink0.x: blink0.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

# blink0.c: program for blinking a single LED

blink0_pt.x: blink0_pt.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

# blink0_pt.c: program for blinking a single LED using a thread

blink01_pt.x: blink01_pt.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

# blink01_pt.c: program for blinking two LEDs using threads

blink01_bp_pt.x: blink01_bp_pt.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

# blink01_bp_pt.c: program for controlling an LED using a push button or teleruptor using threads
