#!/usr/bin/env bash

set -e
set -o pipefail

WAYLAND_SCANNER="$(pkgconf --variable=wayland_scanner wayland-scanner)"
RIVER_PROTOCOLS_DIR="$(pkgconf --variable=pkgdatadir river-protocols)"
WAYLAND_CLIENT_DATA_DIR="$(pkgconf --variable=pkgdatadir wayland-client)"

PROTOCOLS=(
    "${RIVER_PROTOCOLS_DIR}/stable/river-window-management-v1.xml"
    "${WAYLAND_CLIENT_DATA_DIR}/wayland.xml"
)

for protocol in ${PROTOCOLS[@]}; do
    $WAYLAND_SCANNER client-header "$protocol" "src/wayland/protocols/$(basename "$protocol" ".xml").h"
    $WAYLAND_SCANNER private-code "$protocol" "src/wayland/protocols/$(basename "$protocol" ".xml").c"
done
