#!/usr/bin/python

import sys
from PIL import Image
from PIL import ImageOps

image = Image.open(sys.argv[1])
image_gray = ImageOps.grayscale(image)

w, h = image.size
print h, w
for y in range(h):
    for x in range(w):
        print image_gray.getpixel((x,y)),
    print
