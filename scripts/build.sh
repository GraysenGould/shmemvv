#!/bin/bash

# --- Clean build
./scripts/clean.sh

mkdir build ; cd build

OSHCC=$(which oshcc)

cmake \
    -DCMAKE_C_COMPILER=$OSHCC \
    -DCMAKE_C_FLAGS="-lm -ldl" \
    ..

# --- Compile
make -j $(( $(nproc) - 1 ))

echo ; echo
