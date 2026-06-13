#ifndef SSD1306_DRIVER_COMMANDS_H
#define SSD1306_DRIVER_COMMANDS_H

#define DISPLAY_OFF         0XAE
#define DISPLAY_ON          0xAF
#define SET_MEM_ADDR_MODE   0x20
#define PAGE_START_ADDR  0xB0
#define COM_OUTPUT_SCAN_DIRECTION_NORMAL    0xC0
#define COM_OUTPUT_SCAN_DIRECTION_INVERT    0xC8
#define LOW_COLUMN_START_ADDR   0x00
#define HIGH_COLUMN_START_ADDR  0x10
#define START_LINE_START_ADDR   0x40
#define SET_CONTRAST        0x81
#define SEGMENT_REMAP_NORMAL    0xA0
#define SEGMENT_REMAP_INVERT    0xA1
#define DISPLAY_COLOR_NORMAL    0xA6
#define DISPLAY_COLOR_INVERT    0xA7
#define SET_MULTIPLEX_RATIO     0xA8
#define SET_DISPLAY_OFFSET      0xD3
#define DISPLAY_CLOCK       0xD5
#define SET_DISPLAY_PRECHARGE   0xD9
#define COM_PIN_CONFIG          0xDA
#define VCOMH_CONFIG        0xDB
#define SET_CHARGE_PUMP     0x8D //sets charge pump so Vcc can be boosted to 7V

#define HORIZONTAL_MEM_ADDR_MODE    0x00
#define VERTICAL_MEM_ADDR_MODE      0x01
#define DEFAULT_CONTRAST            0x7F
#define MULTIPLEX_32_PX_DISPLAY     0x1F
#define MUTLIPLEX_64_PX_DISPLAY     0x3F
#define FAST_CLOCK                  0xF0
#define MEDIUM_CLOCK                0x80
#define SLOW_CLOCK                  0x83
#define CHARGE                      0x04
#define DISCHARGE                   0x00
#define DEFAULT_COM_CONFIG_PINS_32PX     0x02
#define DEFAULT_COM_CONFIG_PINS_64PX    0x12
#define DEFAULT_VCOMH               0x40
#define CHARGE_PUMP_ENABLE          0x14
#define CHARGE_PUMP_DISABLE         0x10



#endif