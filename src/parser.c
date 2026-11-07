#include "../csvparser.h"

#include <stdlib.h>
#include <string.h>

static char **allocateparsedarray(void) {
    char **fields = (char **)calloc(CSV_PARSER_MAX_FIELDS, sizeof(char *));
    if (fields == NULL) {
        return NULL;
    }

    for (int i = 0; i < CSV_PARSER_MAX_FIELDS; i++) {
        fields[i] = (char *)calloc(CSV_PARSER_MAX_FIELD_LEN, sizeof(char));
        if (fields[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(fields[j]);
            }
            free(fields);
            return NULL;
        }
    }
    return fields;
}

char *readline(FILE *file, char *line) {
    int allocated_here = 0;
    if (file == NULL) {
        return NULL;
    }
    if (line == NULL) {
        line = (char *)malloc(CSV_PARSER_MAX_LINE_LENGTH);
        if (line == NULL) {
            return NULL;
        }
        allocated_here = 1;
    }

    if (fgets(line, CSV_PARSER_MAX_LINE_LENGTH, file) == NULL) {
        if (allocated_here) {
            free(line);
        }
        return NULL;
    }

    /* Do not silently return a truncated physical line. */
    size_t length = strlen(line);
    if (length == CSV_PARSER_MAX_LINE_LENGTH - 1 &&
        line[length - 1] != '\n' && !feof(file)) {
        int ch;
        while ((ch = fgetc(file)) != '\n' && ch != EOF) {
            /* Discard the remainder so the next call starts on a new line. */
        }
        if (allocated_here) {
            free(line);
        }
        return NULL;
    }
    return line;
}

void freeparsedline(char **line) {
    if (line == NULL) {
        return;
    }
    for (int i = 0; i < CSV_PARSER_MAX_FIELDS; i++) {
        free(line[i]);
    }
    free(line);
}

char **parseline(char *line, char *col_separator, char **buffer) {
    if (line == NULL) {
        return NULL;
    }

    const char separator = (col_separator == NULL || col_separator[0] == '\0')
        ? ',' : col_separator[0];
    const int owns_buffer = (buffer == NULL);
    char **fields = owns_buffer ? allocateparsedarray() : buffer;
    if (fields == NULL) {
        return NULL;
    }

    if (!owns_buffer) {
        for (int i = 0; i < CSV_PARSER_MAX_FIELDS; i++) {
            if (fields[i] == NULL) {
                return NULL;
            }
            fields[i][0] = '\0';
        }
    }

    int field_index = 0;
    size_t field_length = 0;
    int in_quotes = 0;
    int at_field_start = 1;

    for (size_t i = 0; line[i] != '\0' && line[i] != '\n' && line[i] != '\r'; i++) {
        const char ch = line[i];

        if (ch == '"' && at_field_start && !in_quotes) {
            in_quotes = 1;
            at_field_start = 0;
            continue;
        }

        if (ch == '"' && in_quotes) {
            if (line[i + 1] == '"') {
                if (field_length >= CSV_PARSER_MAX_FIELD_LEN - 1) {
                    if (owns_buffer) freeparsedline(fields);
                    return NULL;
                }
                fields[field_index][field_length++] = '"';
                fields[field_index][field_length] = '\0';
                i++;
            } else {
                in_quotes = 0;
            }
            continue;
        }

        if (ch == separator && !in_quotes) {
            field_index++;
            if (field_index >= CSV_PARSER_MAX_FIELDS) {
                if (owns_buffer) freeparsedline(fields);
                return NULL;
            }
            field_length = 0;
            at_field_start = 1;
            continue;
        }

        if (field_length >= CSV_PARSER_MAX_FIELD_LEN - 1) {
            if (owns_buffer) freeparsedline(fields);
            return NULL;
        }
        fields[field_index][field_length++] = ch;
        fields[field_index][field_length] = '\0';
        at_field_start = 0;
    }

    if (in_quotes) {
        if (owns_buffer) freeparsedline(fields);
        return NULL;
    }

    return fields;
}
