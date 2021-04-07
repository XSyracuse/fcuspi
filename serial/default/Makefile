###############################################################################
# Makefile for the project serialtest
###############################################################################

## General Flags
PROJECT = serialtest
MCU = atmega128
TARGET = serialtest.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99     -DF_CPU=7372800UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=serialtest.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\m\avr\serial\..\timers" -I"C:\m\avr\serial\..\pwm" 

## Objects that must be built in order to link
OBJECTS = serial.o testserialmain.o timer0.o pwm3.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) serialtest.hex serialtest.eep serialtest.lss size

## Compile
serial.o: ../serial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

testserialmain.o: ../testserialmain.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer0.o: ../../timers/timer0.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwm3.o: ../../pwm/pwm3.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) serialtest.elf dep/* serialtest.hex serialtest.eep serialtest.lss serialtest.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

