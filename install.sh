#!/bin/sh
set -eu

PREFIX="${PREFIX:-/usr/local}"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT

printf '%s\n' "Building CSV parser..."
cc -std=c11 -O2 -Wall -Wextra -Werror -fPIC -I. -c src/parser.c -o "$BUILD_DIR/parser.o"
cc -shared -o "$BUILD_DIR/libcsvparser.so" "$BUILD_DIR/parser.o"

printf '%s\n' "Installing under $PREFIX..."
install -d "$PREFIX/lib" "$PREFIX/include"
install -m 755 "$BUILD_DIR/libcsvparser.so" "$PREFIX/lib/libcsvparser.so"
install -m 644 csvparser.h "$PREFIX/include/csvparser.h"

printf '%s\n' "Installation complete. Set your platform library path if needed."
