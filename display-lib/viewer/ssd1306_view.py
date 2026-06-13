#!/usr/bin/env python3
"""
View the SSD1306 drawing library's output on your computer.

It loads the real C/C++ drawing library (compiled to libssd1306host.so) via
ctypes and calls the same draw functions the firmware uses, then renders the
resulting framebuffer to a PNG with Pillow. No microcontroller needed.

Usage:
    ./build.sh                       # compile the .so first (once, or after edits)
    python3 ssd1306_view.py          # runs the demo -> demo.png
    # or, in your own script:
    from ssd1306_view import Display
    d = Display(128, 64)
    d.circle(64, 32, 20)
    d.text(4, 4, "HELLO")
    d.show("out.png")
"""

import ctypes
import os
from PIL import Image

_HERE = os.path.dirname(os.path.abspath(__file__))
_LIB = os.path.join(_HERE, "libssd1306host.so")


class Display:
    def __init__(self, width=128, height=64):
        if not os.path.exists(_LIB):
            raise FileNotFoundError(
                f"{_LIB} not found. Run ./build.sh in {_HERE} first."
            )
        self.w, self.h = width, height
        self._lib = ctypes.CDLL(_LIB)
        self._lib.canvas_new.restype = ctypes.c_void_p
        self._lib.canvas_new.argtypes = [ctypes.c_int, ctypes.c_int]
        self._lib.canvas_text.argtypes = [
            ctypes.c_void_p,
            ctypes.c_int,
            ctypes.c_int,
            ctypes.c_char_p,
        ]
        self._c = self._lib.canvas_new(width, height)

    # --- library drawing calls (mirror the C++ API) ---
    def clear(self):
        self._lib.canvas_clear(ctypes.c_void_p(self._c))

    def pixel(self, x, y):
        self._lib.canvas_pixel(ctypes.c_void_p(self._c), x, y)

    def line(self, x0, y0, x1, y1):
        self._lib.canvas_line(ctypes.c_void_p(self._c), x0, y0, x1, y1)

    def rect(self, x, y, w, h):
        self._lib.canvas_rect(ctypes.c_void_p(self._c), x, y, w, h)

    def circle(self, cx, cy, r):
        self._lib.canvas_circle(ctypes.c_void_p(self._c), cx, cy, r)

    def text(self, x, y, s):
        self._lib.canvas_text(
            ctypes.c_void_p(self._c), x, y, s.encode("ascii", "replace")
        )

    # --- render the library's framebuffer ---
    def buffer(self):
        buf = (ctypes.c_uint8 * (self.w * self.h))()
        self._lib.canvas_render(ctypes.c_void_p(self._c), buf)
        return buf

    def image(self, scale=6, on=(255, 255, 255), off=(0, 0, 0)):
        buf = self.buffer()
        img = Image.new("RGB", (self.w, self.h))
        px = img.load()
        for y in range(self.h):
            row = y * self.w
            for x in range(self.w):
                px[x, y] = on if buf[row + x] else off
        if scale != 1:
            img = img.resize((self.w * scale, self.h * scale), Image.NEAREST)
        return img

    def show(self, path="out.png", scale=6):
        img = self.image(scale=scale)
        img.save(path)
        print(f"saved {path} ({img.width}x{img.height})")
        return path

    def ascii(self):
        """Quick terminal preview (2 rows per char line)."""
        buf = self.buffer()
        lines = []
        for y in range(0, self.h, 2):
            r1, r2 = y * self.w, (y + 1) * self.w if y + 1 < self.h else y * self.w
            lines.append(
                "".join(
                    "#" if (buf[r1 + x] or buf[r2 + x]) else "." for x in range(self.w)
                )
            )
        return "\n".join(lines)


def _demo():
    d = Display(128, 64)
    # circle in the centre of the screen
    d.circle(64, 32, 28)
    # some text
    d.text(2, 2, "SSD1306 LIB")
    d.text(2, 56, "circle + text")
    # a line and a rectangle to show the other primitives
    d.line(0, 0, 127, 63)
    d.rect(96, 40, 28, 18)
    d.pixel(64, 32)
    print(d.ascii())
    d.show(os.path.join(_HERE, "demo.png"))


if __name__ == "__main__":
    _demo()
