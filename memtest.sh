#!/bin/sh

mkdir -p bin/
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./$@

         # --log-file=bin/valgrind-$1.log \
