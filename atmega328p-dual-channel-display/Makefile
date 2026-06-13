# === Configuration ===
MCU   = atmega328p
F_CPU = 8000000UL
PORT  = /dev/ttyUSB0
BAUD  = 19200
TARGET = main

# Graphics library lives one level up in display-lib/
LIB = ../display-lib/driver-with-overlay-blocks

SRC = main.cpp \
	$(LIB)/i2c.cpp \
	$(LIB)/ssd1306_driver.cpp \
	$(LIB)/canvas.cpp \
	$(LIB)/math_utils.cpp \
	$(LIB)/draw/blocks.cpp \
	$(LIB)/draw/pixel_draw.cpp \
	$(LIB)/draw/line_draw.cpp \
	$(LIB)/draw/rect_draw.cpp \
	$(LIB)/draw/circle_draw.cpp \
	$(LIB)/draw/text_draw.cpp \
	$(LIB)/draw/regions.cpp

OBJS_DIR = ./.obj_files
OBJS = $(patsubst %.cpp,$(OBJS_DIR)/%.o,$(notdir $(SRC)))

# === Tools ===
CXX = avr-g++
CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DWITH_DISPLAY -Os -I../display-lib -I$(LIB)
LDFLAGS = -mmcu=$(MCU)
OBJCOPY = avr-objcopy
SIZE = avr-size

VPATH = $(dir $(SRC))

# === Build Targets ===
build: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) $<

flash: $(TARGET).hex
	avrdude -C /etc/avrdude.conf -v -p $(MCU) -c stk500v1 -P $(PORT) -b $(BAUD) -U flash:w:$(TARGET).hex:i

clean:
	rm -f $(TARGET).elf $(TARGET).hex
	find ./ -type f -name '*.o' -exec rm -f {} \;
