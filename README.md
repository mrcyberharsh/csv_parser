# Aria's CSV Parser

A small C library for reading physical lines and parsing CSV records with a configurable one-character field separator.

## Features

- Read lines from a FILE stream.
- Parse fields using comma or a custom separator.
- Support quoted fields containing separators.
- Decode escaped double quotes inside quoted fields.
- Preserve empty and trailing fields.
- Enforce compile-time limits for line length, field count, and field length.
- Reuse a previously allocated parser result buffer.
- Return NULL for malformed or oversized input instead of writing beyond fixed buffers.

## Limits and behavior

Defaults in csvparser.h:

| Macro | Default |
|---|---:|
| CSV_PARSER_MAX_LINE_LENGTH | 1024 bytes |
| CSV_PARSER_MAX_FIELDS | 50 fields |
| CSV_PARSER_MAX_FIELD_LEN | 1024 bytes |

The parser handles one physical line at a time; multi-line quoted CSV fields are not supported. The separator uses its first character. Surrounding quotes are removed and doubled quotes are converted to one quote.

If readline receives a NULL buffer, it allocates one and the caller must free it. If a caller supplies a buffer, that buffer remains caller-owned. parseline allocates its output when buffer == NULL; release it with freeparsedline. For reuse, pass the previously returned array back into parseline.

## Build

Build the shared library:

```bash
make
```

Run the test suite with AddressSanitizer and UndefinedBehaviorSanitizer:

```bash
make test
# or
./test.sh
```

## Install

Install under /usr/local by default:

```bash
sudo make install
```

Choose another prefix if needed:

```bash
make install PREFIX="$HOME/.local"
```

The install script copies the shared library to PREFIX/lib and the header to PREFIX/include.

## Example

```c
#include "csvparser.h"
#include <stdio.h>

int main(void) {
    char line[] = "\"first, value\",second,third";
    char **fields = parseline(line, NULL, NULL);
    if (fields == NULL) {
        fprintf(stderr, "Invalid or oversized CSV line\n");
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        printf("%s\n", fields[i]);
    }

    freeparsedline(fields);
    return 0;
}
```

## License

MIT License. See LICENSE.txt.
