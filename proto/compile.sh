#!/bin/sh

out=./build
rm -rf $out
mkdir -p $out
protoc-c --c_out=$out tfs.proto

