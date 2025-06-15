#!/bin/bash

# configures the repo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Test &&

# builds the repo
cmake --build build &&

# runs all the tests
cmake --build build --target test
