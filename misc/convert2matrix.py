#!/usr/bin/python

# usage: python convert2matrix.py imagefile > matrix.dat

# requres PIL or Pillow library

import sys
from PIL import Image
from PIL import ImageOps

image = Image.open(sys.argv[1])
image_gray = ImageOps.grayscale(image)

w, h = image.size
sys.stdout.write('{} {}\n'.format(h, w))
for y in range(h):
    for x in range(w):
        sys.stdout.write('{} '.format(image_gray.getpixel((x,y))))
    sys.stdout.write('\n')
