#!/usr/bin/env bash

set -e
set -o pipefail

WAYLAND_CLIENT_FLAGS="$(pkgconf --libs --cflags wayland-client)"
XKBCOMMON_FLAGS="$(pkgconf --libs --cflags xkbcommon)"

g++ src/main.cpp -o robwm -g $XKBCOMMON_FLAGS $WAYLAND_CLIENT_FLAGS -Warith-conversion -Wsign-conversion -Wextra -Wall -Wno-unused-function -std=gnu++11 -fno-rtti -fno-exceptions || exit 1

if [[ $1 = "run" ]]; then
    echo "Running robwm"
    ./robwm
fi
