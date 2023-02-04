CC=bcm2708hardfp-gcc
CFLAGS=-O2 -static

CARD=23
NAME=LI-AUTHIER
CFLAGS=-W -Wall -Wextra -Wfatal-errors -O2

APP=helloworld

all: $(APP)
	scp -P 622$(CARD) $^ pi@peri:$(NAME)

clean:
	rm $(APP)
