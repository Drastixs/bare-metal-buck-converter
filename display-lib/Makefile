#j=== Configuration ===
MCU = atmega328p
F_CPU = 16000000UL
PORT=/dev/ttyUSB0
BAUD=19200
TARGET = main
SRC = main.cpp \
	driver-with-overlay-blocks/i2c.cpp \
	driver-with-overlay-blocks/ssd1306_driver.cpp \
	driver-with-overlay-blocks/canvas.cpp \
	driver-with-overlay-blocks/math_utils.cpp \
	driver-with-overlay-blocks/draw/blocks.cpp \
	driver-with-overlay-blocks/draw/pixel_draw.cpp \
	driver-with-overlay-blocks/draw/line_draw.cpp \
	driver-with-overlay-blocks/draw/rect_draw.cpp \
	driver-with-overlay-blocks/draw/circle_draw.cpp \
	driver-with-overlay-blocks/draw/text_draw.cpp \
	driver-with-overlay-blocks/draw/regions.cpp
OBJS_DIR = ./.obj_files
OBJS = $(patsubst %.cpp,$(OBJS_DIR)/%.o,$(SRC))
# === Tools ===
CXX = avr-g++
CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os 
LDFLAGS = -mmcu=$(MCU)
OBJCOPY = avr-objcopy
SIZE = avr-size

# === Build Targets ===
build: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

#create folder structure before compiling
$(OBJS_DIR)/%.o: %.cpp
	@echo $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) $<

clean:
	@#Remove .elf and .hex files
	rm $(TARGET).elf $(TARGET).hex
	@#Remove .o files in output_files
	find ./ -type f -name *.o -exec rm -f {} \;

flash:
	avrdude -C /etc/avrdude.conf -v -p $(MCU) -c stk500v1 -P $(PORT) -b $(BAUD) -U flash:w:$(TARGET).hex:i
