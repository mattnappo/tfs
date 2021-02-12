#!/bin/sh

make clean
make

for f in bin/tests/*; do
    ./memtest.sh $f
    wait 2
done

