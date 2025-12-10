MCU = atmega328p
F_CPU = 16000000UL

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra -std=c99

SRC = main.c oled.c i2c.c bh1750.c bmp280.c adc.c dht11.c guva.c sgp.c
OBJ = $(SRC:.c=.o)

TARGET = main
ELF = $(TARGET).elf
HEX = $(TARGET).hex

all: $(HEX)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(HEX)
	$(AVRDUDE) -c arduino -p $(MCU) -P /dev/ttyUSB0 -b 115200 -U flash:w:$<:i

clean:
	rm -f $(OBJ) $(ELF) $(HEX) *.map

.PHONY: all flash clean

