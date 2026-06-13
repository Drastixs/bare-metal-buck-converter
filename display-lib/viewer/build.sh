#!/usr/bin/env bash
# Compile the drawing library + host wrapper into a shared lib for Python ctypes.
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
LIB="$HERE/.."                       # driver-example/
DRV="$LIB/driver-with-overlay-blocks"

g++ -std=c++14 -O2 -fPIC -shared \
    -I "$DRV" -I "$HERE/shim" \
    -o "$HERE/libssd1306host.so" \
    "$HERE/host_render.cpp" \
    "$DRV/draw/blocks.cpp" \
    "$DRV/draw/pixel_draw.cpp" \
    "$DRV/draw/line_draw.cpp" \
    "$DRV/draw/rect_draw.cpp" \
    "$DRV/draw/circle_draw.cpp" \
    "$DRV/draw/text_draw.cpp" \
    "$DRV/draw/regions.cpp" \
    "$DRV/math_utils.cpp"

echo "built $HERE/libssd1306host.so"
