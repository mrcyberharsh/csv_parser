#include "../csvparser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_readline_non_empty_file(void) {
    FILE *file = fopen("test/files/non_empty_file.txt", "r");
    assert(file != NULL);
    char *line = readline(file, NULL);
    assert(line != NULL);
    assert(strcmp(line, "line1\n") == 0);
    free(line);
    fclose(file);
}

static void test_readline_empty_file(void) {
    FILE *file = fopen("test/files/empty_file.txt", "r");
    assert(file != NULL);
    char *line = readline(file, NULL);
    assert(line == NULL);
    fclose(file);
}

static void test_readline_null_file_pointer(void) {
    char *line = readline(NULL, NULL);
    assert(line == NULL);
}

static void test_readline_does_not_free_caller_buffer(void) {
    FILE *file = fopen("test/files/empty_file.txt", "r");
    assert(file != NULL);
    char buffer[CSV_PARSER_MAX_LINE_LENGTH] = {0};
    assert(readline(file, buffer) == NULL);
    buffer[0] = 'x'; /* Buffer remains caller-owned and writable. */
    assert(buffer[0] == 'x');
    fclose(file);
}

static void test_parseline_null_input(void) {
    assert(parseline(NULL, NULL, NULL) == NULL);
}

static void test_parseline_default_separator(void) {
    char line[] = "field1,field2,field3";
    char **result = parseline(line, NULL, NULL);
    assert(result != NULL);
    assert(strcmp(result[0], "field1") == 0);
    assert(strcmp(result[1], "field2") == 0);
    assert(strcmp(result[2], "field3") == 0);
    freeparsedline(result);
}

static void test_parseline_custom_separator(void) {
    char line[] = "field1;field2;field3";
    char **result = parseline(line, ";", NULL);
    assert(result != NULL);
    assert(strcmp(result[0], "field1") == 0);
    assert(strcmp(result[1], "field2") == 0);
    assert(strcmp(result[2], "field3") == 0);
    freeparsedline(result);
}

static void test_parseline_quoted_separator_and_quotes(void) {
    char line[] = "\"some, words\",\"He said \"\"Hi\"\"\"";
    char **result = parseline(line, NULL, NULL);
    assert(result != NULL);
    assert(strcmp(result[0], "some, words") == 0);
    assert(strcmp(result[1], "He said \"Hi\"") == 0);
    freeparsedline(result);
}

static void test_parseline_empty_and_trailing_fields(void) {
    char line[] = "first,,third,";
    char **result = parseline(line, NULL, NULL);
    assert(result != NULL);
    assert(strcmp(result[0], "first") == 0);
    assert(strcmp(result[1], "") == 0);
    assert(strcmp(result[2], "third") == 0);
    assert(strcmp(result[3], "") == 0);
    freeparsedline(result);
}

static void test_parseline_malformed_quote(void) {
    char line[] = "\"unfinished";
    assert(parseline(line, NULL, NULL) == NULL);
}

static void test_parseline_field_limit(void) {
    char line[CSV_PARSER_MAX_FIELD_LEN + 2];
    memset(line, 'x', sizeof(line) - 1);
    line[sizeof(line) - 1] = '\0';
    assert(parseline(line, NULL, NULL) == NULL);
}

static void test_parseline_field_count_limit(void) {
    char line[CSV_PARSER_MAX_FIELDS * 2 + 4];
    line[0] = '\0';
    for (int i = 0; i < CSV_PARSER_MAX_FIELDS; i++) {
        strcat(line, "x,");
    }
    strcat(line, "x");
    assert(parseline(line, NULL, NULL) == NULL);
}

static void test_buffer_reuse(void) {
    char first[] = "one,two";
    char second[] = "three,four";
    char **result = parseline(first, NULL, NULL);
    assert(result != NULL);
    result = parseline(second, NULL, result);
    assert(result != NULL);
    assert(strcmp(result[0], "three") == 0);
    assert(strcmp(result[1], "four") == 0);
    freeparsedline(result);
}

int main(void) {
    test_readline_non_empty_file();
    test_readline_empty_file();
    test_readline_null_file_pointer();
    test_readline_does_not_free_caller_buffer();
    test_parseline_null_input();
    test_parseline_default_separator();
    test_parseline_custom_separator();
    test_parseline_quoted_separator_and_quotes();
    test_parseline_empty_and_trailing_fields();
    test_parseline_malformed_quote();
    test_parseline_field_limit();
    test_parseline_field_count_limit();
    test_buffer_reuse();
    puts("All CSV parser tests passed.");
    return 0;
}
