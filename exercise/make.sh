#!/bin/sh

PDF="exercise-1-0.pdf exercise-1-1.pdf exercise-1-2.pdf exercise-1-3.pdf exercise-1-4.pdf \
  exercise-2-1.pdf exercise-2-2.pdf exercise-2-3.pdf exercise-2-4.pdf"

for p in ${PDF}; do
    latexmk $(basename $p .pdf)
done
