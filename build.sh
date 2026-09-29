#!/usr/bin/env bash

g++ src/main.cpp -o robwm -g -Wextra -Wall -Wno-unused-function -pedantic -std=c++11 -fno-rtti -fno-exceptions || exit 1

./robwm
