#!/bin/bash

# configures the repo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release &&

# builds the repo
cmake --build build &&

# runs the binary
./build/apps/app
