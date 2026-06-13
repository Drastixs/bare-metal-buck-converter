#!/usr/bin/env bash
# Build the code-defined schematics from the CircuitikZ (LaTeX) sources.
# Outputs, written one level up into images/:
#   schematic-<panel>.svg   vector, linked individually from the README
#   schematic-combined.png  2x2 montage embedded in the README
#
# Requires: pdflatex (texlive + circuitikz), pdftocairo (poppler), ImageMagick.
set -euo pipefail
cd "$(dirname "$0")"
panels=(power current voltage pot)
out=..

for p in "${panels[@]}"; do
    pdflatex -interaction=nonstopmode -halt-on-error "$p.tex" >/dev/null
    pdftocairo -svg "$p.pdf" "$out/schematic-$p.svg"          # vector for the README links
    pdftoppm -png -r 150 "$p.pdf" "$p" >/dev/null             # raster for the montage
done

montage power-1.png current-1.png voltage-1.png pot-1.png \
    -tile 2x2 -geometry +20+20 -background white \
    -bordercolor "#cccccc" -border 1 "$out/schematic-combined.png"

rm -f ./*.aux ./*.log ./*-1.png
echo "wrote schematic-{power,current,voltage,pot}.svg and schematic-combined.png"
