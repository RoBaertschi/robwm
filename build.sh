#!/usr/bin/env bash

set -e
set -o pipefail

WAYLAND_CLIENT_FLAGS="$(pkgconf --libs --cflags wayland-client)"

g++ src/main.cpp -o robwm -g $WAYLAND_CLIENT_FLAGS -Wextra -Wall -Wno-unused-function -pedantic -std=c++11 -fno-rtti -fno-exceptions || exit 1

./robwm
