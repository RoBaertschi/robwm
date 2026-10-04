#!/usr/bin/env bash

set -e
set -o pipefail

WAYLAND_CLIENT_FLAGS="$(pkgconf --libs --cflags wayland-client)"
XKBCOMMON_FLAGS="$(pkgconf --libs --cflags xkbcommon)"

OPT_FLAGS="-g"
RUN=0

for arg in $@; do
    case "$arg" in
	release)
		OPT_FLAGS="-O2"
		;;
	run)
	    RUN=1
		;;
	*)
	    echo "Invalid flag $arg"
		;;
    esac
done

g++ src/main.cpp -o robwm $OPT_FLAGS $XKBCOMMON_FLAGS $WAYLAND_CLIENT_FLAGS -Warith-conversion -Wsign-conversion -Wextra -Wall -Wno-unused-function -std=gnu++11 -fno-rtti -fno-exceptions || exit 1

if [[ $RUN = 1 ]]; then
    echo "Running robwm"
    ./robwm
fi
