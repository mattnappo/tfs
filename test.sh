#!/bin/sh

make clean
make

for f in bin/tests/*; do
    ./memtest.sh $f
    read -r -p "Next test..." -t 2 -n 1 -s
done

