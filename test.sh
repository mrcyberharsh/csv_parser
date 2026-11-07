#!/bin/sh
set -eu

OUTPUT="tests"
cleanup() { rm -f "$OUTPUT"; }
trap cleanup EXIT

cc -std=c11 -Wall -Wextra -Werror -g -fsanitize=address,undefined -I. \
  src/parser.c test/test_parser.c -o "$OUTPUT"
"./$OUTPUT"
