# SSD1306 Overlay-Block Drawing Library (ATmega328P)

A from-scratch, bare-metal graphics library for SSD1306 OLED displays driven by an
ATmega328P. Its defining feature: **it draws shapes without a full framebuffer.**

## Why it exists

The SSD1306 (128×64) needs 1024 bytes of pixel data. The ATmega328P only has 2048
bytes of RAM, so a conventional full-screen framebuffer eats half your memory before
you've written a single variable.

This library avoids that. Instead of storing pixels, it stores a small list of **draw
commands** (line, rect, circle, pixel, text). To push to the display it walks the
screen in **8×8 tiles ("regions")**, asks each command *"do you touch this tile?"*,
rasterizes only the ones that do into a **single shared 8-byte scratch block**, flushes
those 8 bytes, and reuses the block for the next tile.

Net result: the only pixel buffer that ever exists is **8 bytes** instead of 1024.

- **Wins:** ~8 bytes of pixel RAM vs 1024.
- **Costs:** every redraw re-runs the rasterizers (CPU instead of RAM), and the command
  list is capped (`MAX_STORED_DRAW_COMMANDS`, currently 32).

## Layout

```
driver-example/
├── main.cpp                     # firmware demo (drives the library over I2C)
├── Makefile                     # avr-g++ build + avrdude flash
├── driver-with-overlay-blocks/
│   ├── i2c.cpp/.h               # TWI/I2C transport
│   ├── ssd1306_driver.cpp/.h    # SSD1306 command/data driver (c_ssd1306)
│   ├── canvas.cpp/.h            # user-facing surface (c_canvas) — engine + driver
│   ├── math_utils.cpp/.h        # min/max/abs
│   └── draw/
│       ├── regions.cpp/.h       # the engine: command store + tile-walking loop
│       ├── blocks.cpp/.h        # 8×8 block primitives (pixels, lines, rotate, flip)
│       ├── pixel_draw / line_draw / rect_draw / circle_draw / text_draw
│       └── font5x7.h            # 5×7 ASCII font (0x00–0x7F)
└── viewer/                      # host-side viewer (see below)
```

## Using the library (on hardware)

The easy path is `c_canvas`, which owns both the drawing engine and the SSD1306 driver.

```cpp
#define F_CPU 16000000UL
#include <util/delay.h>
#include "driver-with-overlay-blocks/canvas.h"

int main(void){
    i2c_init();
    c_canvas canvas;                 // defaults to 128×64

    pos_t a{10,8}, b{60,40};
    canvas.draw.line(a, b);          // line segment
    pos_t r{70,5};
    canvas.draw.rect(r, 40, 22);     // rectangle outline (x,y, w,h)
    pos_t c{30,44};
    canvas.draw.circle(c, 14);       // circle outline (centre, radius)
    pos_t p{100,50};
    canvas.draw.pixel(p);            // single pixel
    pos_t t{2,2};
    canvas.draw.text(t, "HELLO");    // 5×7 text

    while (1){
        canvas.update();             // re-render: tile-walk + flush only touched blocks
        _delay_ms(500);
    }
}
```

### Drawing API (`canvas.draw`, type `c_region_drawer`)

| Call | Effect |
|------|--------|
| `pixel(pos_t)` | one pixel |
| `line(pos_t a, pos_t b)` | line segment (Bresenham, clipped to endpoints) |
| `rect(pos_t topLeft, w, h)` | rectangle outline |
| `circle(pos_t centre, r)` | circle outline (midpoint algorithm) |
| `text(pos_t topLeft, const char* s)` | 5×7 text, ASCII 0x00–0x7F |
| `clear()` | drop all queued commands |

`canvas.update()` renders everything currently queued. Other `c_canvas` helpers:
`on()`, `off()`, `clear()`, `contrast(v)`, `brightness(v)`.

> Note: shapes are **outlines** (no filled variants yet). Up to
> `MAX_STORED_DRAW_COMMANDS` (32) commands can be queued at once — raise it in
> `blocks.h` **and** `regions.h` together if you need more.

### Build & flash

The Makefile expects `avr-g++` / `avrdude` on `PATH` (e.g. the toolchain bundled with
the Arduino IDE under `~/.arduino15/packages/arduino/tools/avr-gcc/.../bin`).

```bash
make            # -> main.hex  (also prints avr-size)
make flash      # avrdude, stk500v1, /dev/ttyUSB0  (edit PORT/BAUD in the Makefile)
make clean
```

Defaults: `MCU=atmega328p`, `F_CPU=16000000`. For a 128×32 panel construct the
display/canvas with height 32.

## Using the viewer (on your computer — no microcontroller)

`viewer/` lets you **see the library's output as an image** without flashing anything.
It compiles the *exact same* drawing code (`regions.cpp`, `blocks.cpp`, the `*_draw.cpp`
rasterizers) into a shared library and calls it from Python via `ctypes`. Only the
transport differs — instead of pushing bytes over I2C, a thin host wrapper renders into
a plain RAM framebuffer, which Pillow turns into a PNG. So what you see matches what the
panel would show.

Requirements: `g++`, `python3`, and `Pillow` (`pip install pillow`).

```bash
cd viewer
./build.sh                  # compiles libssd1306host.so (re-run after editing the C library)
python3 ssd1306_view.py     # runs the demo -> viewer/demo.png  (+ ASCII preview in terminal)
```

In your own script:

```python
from ssd1306_view import Display
d = Display(128, 64)
d.circle(64, 32, 28)        # circle in the centre of the screen
d.text(2, 2, "SSD1306 LIB") # render text
d.line(0, 0, 127, 63)
d.rect(96, 40, 28, 18)
print(d.ascii())            # quick terminal preview
d.show("out.png")           # write a scaled PNG you can open
```

Workflow: edit a `.cpp`, re-run `./build.sh`, re-run the viewer — you see the change
immediately. Because it runs the real library, anything you verify in the viewer behaves
the same when flashed.

## What I learned

The purpose of this project was simple: to test whether I could actually build a
library from the ground up. It was a self-set challenge. It took about a month to plan
out what I wanted and build it in full.

The main objective was to **avoid keeping a whole framebuffer in the memory of a tiny
microcontroller.** The inspiration was custom fonts: characters take up a significant
amount of memory, so if you wanted a full A–Z glyph set *plus* a framebuffer, you'd fill
the entire memory of the microcontroller. That's obviously not practical — but it made a
good challenge to test my C implementation and to learn more about graphics. I'd built a
project previously in Pygame around graphics and element building, and I wanted to
explore the same ideas lower down and see the practicalities.

Specific things I took away from it:

- **How to break a task down.** Going from "draw shapes without a framebuffer" to an
  engine, tile/region primitives, and per-shape rasterizers.
- **How to build a rendering library focused on memory usage** — the whole point was
  trading RAM for CPU by compositing the screen tile by tile.
- **How to get more speed out of a microcontroller by reading the datasheet** and
  referencing the actual pin numbers on the output. The datasheet was long (and hard to
  find), but understandable. I wanted to get right down to the hardware and understand
  *why* it was built the way it was.
- **I²C vs SPI** — how I²C actually works, and its advantages and disadvantages compared
  to something like SPI.
- **Using the microcontroller's clock properly.** I learned to drive a more well-defined
  clock instead of relying on the internal one (which varies a lot), which made the
  display timings far better — consistent, proper frame rates instead of drift.
- **Hardware configuration via control bits** — the specific bits for configuring the
  hardware at boot: clock speed, charge pump, and other startup settings.
- **Build tooling (`make`).** The compilation here was simple enough, but the raw
  commands for compiling these libraries are painfully long — `make` made that
  manageable.

I already had previous experience in C/C++, so that's where I felt most comfortable
developing.

## Provenance

- **The library itself** — the SSD1306 driver, the overlay-block drawing engine, and the
  block/shape primitives — was built in full in **January 2025**.
- **The host viewer** (`viewer/`) was built with **Claude Code** in **June 2026**.
