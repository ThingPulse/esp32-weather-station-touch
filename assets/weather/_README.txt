Source: https://bas.dev/work/meteocons

How to convert large PNGs to small BMPs
=======================================

-> use ImageMagick

Due to a ton of unbalanced withspace around the original images I manually cropped them first!

- crop
- remove background and make black instead
- convert to true-color BMP

for f in *.png ; do convert "$f" -gravity Center -crop '100x100+0+0' +repage -background black -alpha remove -flatten -alpha off -type truecolor "../../data/weather/${f%.png}.bmp" ; done

