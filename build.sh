#!/bin/bash
rm build-x86_64 -rf
cmake -S . -B build-x86_64 -G Ninja
cmake --build build-x86_64 -v