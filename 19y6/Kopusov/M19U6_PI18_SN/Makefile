###############################################################################
# Makefile for the project M19U6Ind
###############################################################################

## General Flags
PROJECT = M19U6Ind
MCU = atmega8535
TARGET = M19U6Ind.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2                                          -DF_CPU=8000000UL -O1 -funsigned-char -funsigned-bitfields
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=M19U6Ind.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\avr" -I"d:\kopusov\pgm\19y6\Kopusov\M19U6_PI18_SN\." 

## Objects that must be built in order to link
OBJECTS = M19U6Ind.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) M19U6Ind.hex M19U6Ind.eep M19U6Ind.lss## Compile
M19U6Ind.o: M19U6Ind.c
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
	C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe -C --mcu=$(MCU) d:\kopusov\pgm\19y6\Kopusov\M19U6_PI18_SN\default\$(TARGET)

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) M19U6Ind.elf dep/* M19U6Ind.hex M19U6Ind.eep M19U6Ind.lss M19U6Ind.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

