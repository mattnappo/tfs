#!/bin/sh

mkdir -p bin/
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./$1

         # --log-file=bin/valgrind-$1.log \
