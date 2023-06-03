Origin of the wind-rose design
==============================

-> the wind-rose's mother is https://github.com/basmilius/weather-icons/blob/dev/production/line/svg-static/compass.svg


How to convert large PNGs to small BMPs
=======================================

-> use ImageMagick

- crop
- remove background and make black instead
- resize
- convert to true-color BMP

for f in *.png ; do convert "$f" -gravity Center -crop '160x160+0+0' +repage -background black -alpha remove -flatten -alpha off -resize 50x50 -type truecolor "../../data/wind/${f%.png}.bmp" ; done

