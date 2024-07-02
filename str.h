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

str     str_create(int len);
str     str_create_from(char* text);
void    str_free(str string);
void    str_set(str* dst, str text);
void    str_clear(str* string);
str     str_strip(str string);
str_arr str_split(str src, char delimiter);
bool    str_compare(str* a, str* b);
void    str_print(str string);
void    str_printf(str format, ...);
void    str_println(str string);
#define STR(string) str_create_from(string)

int64_t stoi(str s);
double  stod(str s);
str     ctos(char c);
char    stoc(str s);

dynstr dynstr_create(void);
dynstr dynstr_create_from(char* text);
void   dynstr_free(dynstr string);
void   dynstr_append(dynstr* string, char* text);
void   dynstr_append_str(dynstr* string, str text);
void   dynstr_append_char(dynstr* string, char c);
void   dynstr_remove(dynstr* string, int start, int end);
void   dynstr_clear(dynstr* string);
bool   dynstr_compare(dynstr* a, dynstr* b);
void   dynstr_print(dynstr string);
void   dynstr_printf(dynstr format, ...);
void   dynstr_println(dynstr string);
str    dynstr_to_str(dynstr* src);
#define DSTR(string) dynstr_create_from(string)

str_arr str_arr_create(void);
str_arr str_arr_create_from(str* arr);
void    str_arr_free(str_arr arr);
str     str_arr_get(str_arr arr, int index);
void    str_arr_append(str_arr* arr, str new_str);
str     str_arr_remove(str_arr* arr, int index);
void    str_arr_print(str_arr arr);
#define STRARR(string_array) str_arr_create_from(string_array)
