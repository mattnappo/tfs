#!/bin/bash

make clean

make                && ./memtest.sh ./bin/tfs.out
echo tfs
read -p "Press enter to continue"

make test_main      && ./memtest.sh ./bin/tests/test_main.out
echo main
read -p "Press enter to continue"

make test_serialize && ./memtest.sh ./bin/tests/test_serialize.out
echo serialize
read -p "Press enter to continue"

make test_protocol  && ./memtest.sh ./bin/tests/test_protocol.out
echo protocol

rm vgcore*

