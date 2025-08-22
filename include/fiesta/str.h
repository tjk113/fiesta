#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char* data;
    int len;
} str;

typedef struct {
    char* data;
    int len;
    int cap;
} dynstr;

typedef struct {
    str* data;
    int len;
    int cap;
} str_arr;

/* str */

// Create a fixed-length string from a null-terminated source.
str     str_create_from(char* text);
// Wrapper around `str_create_from`.
#define STR(string) str_create_from(string)
// Split a string at a delimiter, returning an array of the resulting strings.
str_arr str_split(str src, char delimiter);
// Wrapper around strcmp.
int     str_compare(str* a, str* b);
// Wrapper around strncmp.
int     str_compare_n(str* a, str* b, int n);
// Print a string.
void    str_print(str string);
// Print a string with a terminating newline.
void    str_println(str string);

// Convert a string to a 64-bit integer.
int64_t stoi(str s);
// Convert a string to a double.
double  stod(str s);
// Convert a character to a string.
#define ctos(c) (str){.data = (char[]){c, '\0'}, .len = 1}
// Convert a string to a character (a length of 1 is assumed).
char    stoc(str s);

/* dynstr */

// Create a dynamic-length string.
dynstr dynstr_create();
// Create a dynamic-length string from a null-terminated source.
dynstr dynstr_create_from(char* text);
// Wrapper around `dynstr_create_from`.
#define DSTR(string) dynstr_create_from(string)
// Free a dynamic string's data.
void   dynstr_free(dynstr string);
// Append data to a dynamic string from a null-terminated source.
void   dynstr_append(dynstr* string, char* text);
// Append data to a dynamic string from another dynamic string.
void   dynstr_append_str(dynstr* string, str text);
// Append a character to a dynamic-length string.
void   dynstr_append_char(dynstr* string, char c);
// Remove characters from the range [start, end) in a dynamic string.
void   dynstr_remove(dynstr* string, int start, int end);
// Clear a dynamic string's data.
void   dynstr_clear(dynstr* string);
// Wrapper around strcmp.
int    dynstr_compare(dynstr* a, dynstr* b);
// Wrapper around strncmp.
int    dynstr_compare_n(dynstr* a, dynstr* b, int n);
// Replace all uppercase characters in a dynamic string with lowercase characters.
void   dynstr_to_lower(dynstr* string);
// Replace all lowercase characters in a dynamic string with uppercase characters.
void   dynstr_to_upper(dynstr* string);
// Print a dynamic string.
void   dynstr_print(dynstr string);
// Print a dynamic string with a terminating newline.
void   dynstr_println(dynstr string);
// Convert a dynamic string to a fixed-length string. The dynamic string's memory must still be freed.
str    dynstr_to_str(dynstr* src);

/* str_arr */

// Create a dynamic array of strings.
str_arr str_arr_create();
// Create a dynamic array of strings from a null-terminated array.
str_arr str_arr_create_from(str* arr);
// Wrapper around `str_arr_create_from`.
#define STRARR(string_array) str_arr_create_from(string_array)
// Free a string array's data.
void    str_arr_free(str_arr arr);
// Get a string from an index into a string array.
str     str_arr_get(str_arr arr, int index);
// Append a string to a string array.
void    str_arr_append(str_arr* arr, str new_str);
// Remove the string at an index in a string array.
str     str_arr_remove(str_arr* arr, int index);
// Print the contents of a string array.
void    str_arr_print(str_arr arr);
// Join together a string array's elements into one string, with an optional separator between elements. Pass `NULL` as the separator if one is not desired. The string array's memory will be freed.
str     str_arr_to_str(str_arr* arr, str* separator);