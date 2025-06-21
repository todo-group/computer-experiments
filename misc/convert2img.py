#!/usr/bin/python

# about: convert matrix data (cmatrix.h format) to an image
# usage: python convert2img.py <input_file> <output_file>

# requires matplotlib and numpy libraries

import sys

import matplotlib.pyplot as plt
import numpy as np

if len(sys.argv) != 3:
    print("Usage: python convert2img.py <input_file> <output_file>")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]

f = open(input_file, "r")
line = f.readlines()
m, n = np.uint(line[0].split())
A = np.zeros((m, n))
for i in range(m):
    A[i, :] = np.float64(line[i + 1].split())
print(A)

plt.imsave(output_file, A, cmap="gray")
