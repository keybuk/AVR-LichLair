AVRPROG = usbtiny
AVRCHIP = attiny85

F_CPU = 8000000UL

CC = avr-gcc
OBJCOPY = avr-objcopy

CFLAGS = -Os -mmcu=$(AVRCHIP) -Wall -Wno-maybe-uninitialized
LDFLAGS = -mmcu=$(AVRCHIP)
DEFINES = -DF_CPU=$(F_CPU)
LIBS = -Wl,-u,vfprintf -lprintf_flt -lm

all: blink.hex flame.hex magic_circle.hex


.c.o:
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c $<

.o.elf:
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $^

.elf.hex:
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.SUFFIXES: .hex .elf


clean:
	-rm *.hex *.elf *.o

flash_%: %.hex
	avrdude -c $(AVRPROG) -p $(AVRCHIP) -U flash:w:$<

.PHONY: all clean
