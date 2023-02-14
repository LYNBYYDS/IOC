# Set the compiler to use
CC=bcm2708hardfp-gcc

# Set the compiler flags
CFLAGS=-O2 -static

# Define the card number and user name
CARD=23
NAME=li_authier

# Override the CFLAGS variable to include additional warning flags
CFLAGS+=-W -Wall -Wextra -Wfatal-errors -O2

# Define the name of the executable to be generated
APP=helloworld

# Define the default target to build the executable and copy it to a remote host
all: $(APP)
	scp -P 622$(CARD) $^ pi@peri:$(NAME)

# Define the rule for building the executable from the source file
helloworld.x: helloworld.c
	$(CC) -o $@ $< $(CFLAGS)

# Define the target for cleaning up generated files
clean:
	rm $(APP)

