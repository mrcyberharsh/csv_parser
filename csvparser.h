#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <stdio.h>

#ifndef CSV_PARSER_MAX_LINE_LENGTH
#define CSV_PARSER_MAX_LINE_LENGTH 1024
#endif

#ifndef CSV_PARSER_MAX_FIELDS
#define CSV_PARSER_MAX_FIELDS 50
#endif

#ifndef CSV_PARSER_MAX_FIELD_LEN
#define CSV_PARSER_MAX_FIELD_LEN 1024
#endif

/*
 * Read one physical line into a buffer of CSV_PARSER_MAX_LINE_LENGTH bytes.
 * If line is NULL, the function allocates the buffer; the caller must free it.
 * If line is supplied, it must point to a writable buffer of the configured
 * maximum size and remains owned by the caller.
 *
 * Returns NULL on EOF, invalid input, or an I/O error.
 */
char *readline(FILE *file, char *line);

/*
 * Parse one CSV physical line. The first character of col_separator is used;
 * NULL or an empty separator selects comma.
 *
 * Quoted fields may contain separators and doubled quotes (""). Surrounding
 * quotes are removed and doubled quotes are decoded. Multi-line quoted fields
 * are not supported by this line-based API.
 *
 * If buffer is NULL, a field array is allocated. Otherwise buffer must be a
 * previously returned array from parseline and is cleared/reused.
 * Returns NULL for malformed, oversized, or over-capacity input.
 */
char **parseline(char *line, char *col_separator, char **buffer);

/* Release an array returned by parseline. NULL is accepted. */
void freeparsedline(char **line);

#endif
