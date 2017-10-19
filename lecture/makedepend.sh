#!/bin/sh

INPUTS=$(grep '\\input' *.tex | sed 's/[{:}]/ /g' | sed 's/ \\input /:/')

for i in $INPUTS; do
    TARGET=$(echo $i | awk -F: '{print $1}' | sed 's/.tex/.pdf/')
    SOURCE=$(echo $i | awk -F: '{print $2}')
    echo "$TARGET: $SOURCE"
done
