Source: https://lunaf.com/lunar-calendar/

Convert to BMP
for f in *.png ; do convert "$f" -background black -alpha remove -flatten -alpha off -resize 75x75 -type truecolor "../../data/moon/${f%.png}.bmp" ; done

