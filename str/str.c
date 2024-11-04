#ifndef _WIN32
#include <sys/param.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "str.h"

#define DYN_BASE_SIZE     10
#define DYN_GROWTH_RATE 1.5f
#define NULL_STR   STR("\0")

typedef struct {
    void* data;
    int len;
    int cap;
} dynobj;

static void maybe_realloc(dynobj* obj, int num_new_elements, int element_size) {
    // Allocate more memory if needed
    if (obj->len + num_new_elements >= obj->cap) {
        obj->cap += num_new_elements;
        obj->cap *= DYN_GROWTH_RATE;
        void* new_ptr = realloc(obj->data, obj->cap * element_size);
        if (new_ptr) obj->data = new_ptr;
        memset(new_ptr + obj->len + num_new_elements, 0, num_new_elements * element_size);
    }
    // Reduce memory if over-allocated
    else if (num_new_elements < 0 && num_new_elements != -obj->len) {
        obj->cap -= num_new_elements;
        void* new_ptr = realloc(obj->data, obj->cap * element_size);
        if (new_ptr) obj->data = new_ptr;
        memset(new_ptr + obj->len + num_new_elements, 0, num_new_elements * element_size);
    }
    /* But don't free the dynobj if clearing all of its elements
    (passing 0 to realloc would free the allocation) */
    else if (num_new_elements == -obj->len) {
        obj->cap = DYN_BASE_SIZE;
        void* new_ptr = realloc(obj->data, obj->cap * element_size);
        if (new_ptr) obj->data = new_ptr;
        memset(new_ptr, 0, obj->cap * element_size);
    }
}

double stod(str s) {
    return strtod(s.data, NULL);
}

int64_t stoi(str s) {
    char* end_ptr = s.data + s.len;
#ifdef _WIN32
    return _strtoi64(s.data, &end_ptr, 10);
#else
    return strtol(s.data, &end_ptr, 10);
#endif
}

str ctos(char c) {
    str character = str_create(1);
    character.data[0] = c;
    return character;
}

char stoc(str s) {
    return s.data[0];
}

str str_create(int len) {
    str new_str = {0};

    new_str.data = calloc(len + 1, sizeof(char));
    new_str.data[len] = '\0';
    new_str.len = len;

    return new_str;
}

str str_create_from(char* text) {
    str new_str = {0};

    new_str.len = strlen(text);
    new_str.data = calloc(new_str.len + 1, sizeof(char));
    memcpy(new_str.data, text, new_str.len);
    new_str.data[new_str.len] = '\0';

    return new_str;
}

void str_free(str string) {
    free(string.data);
}

void str_set(str* dst, char* text) {
    int text_len = strlen(text);
    if (text_len <= dst->len)
        dst->len = text_len;
    else
        return; // TODO: error return codes
    memcpy(dst->data, text, dst->len);
}

void str_clear(str* string) {
    memset(string->data, '\0', string->len);
    string->len = 0;
}

str_arr str_split(str src, char delimiter) {
    dynstr cur_split = dynstr_create();
    str_arr split_arr = str_arr_create();
    for (int i = 0; i < src.len; i++) {
        if (src.data[i] == delimiter) {
            /* `dynstr_to_str` frees the memory of its str
            argument, so we can just create a new dynstr */
            str_arr_append(&split_arr, dynstr_to_str(&cur_split));
            cur_split = dynstr_create();
        }
        else
            dynstr_append_char(&cur_split, src.data[i]);
    }
    str_arr_append(&split_arr, dynstr_to_str(&cur_split));
    return split_arr;
}

int str_compare(str* a, str* b) {
    return strcmp(a->data, b->data);
}

int str_compare_n(str* a, str* b, int n) {
    return strncmp(a->data, b->data, n);
}

void str_to_lower(str* string) {
    for (int i = 0; i < string->len; i++) {
        if (string->data[i] >= 'A' && string->data[i] <= 'Z')
            string->data[i] += ' ';
    }
}

void str_to_upper(str* string) {
    for (int i = 0; i < string->len; i++) {
        if (string->data[i] >= 'a' && string->data[i] <= 'z')
            string->data[i] -= ' ';
    }
}

void str_print(str string) {
    printf("%s", string.data);
}

void str_printf(str format, ...) {
    va_list args = {0};
    va_start(args, format);
    vprintf(format.data, args);
    va_end(args);
}

void str_println(str string) {
    printf("%s\n", string.data);
}

dynstr dynstr_create(void) {
    dynstr new_str = {0};

    new_str.data = calloc(DYN_BASE_SIZE, sizeof(char));
    new_str.len = 0;
    new_str.cap = DYN_BASE_SIZE;
    new_str.data[new_str.len + 1] = '\0';

    return new_str;
}

dynstr dynstr_create_from(char* text) {
    dynstr new_str = {0};

    new_str.len = strlen(text);
    /* If `text` is shorter than `DYN_BASE_SIZE`,
    use `DYN_BASE_SIZE` for the cap */
    new_str.cap = fmax(new_str.len, DYN_BASE_SIZE) * DYN_GROWTH_RATE;
    new_str.data = calloc(new_str.cap, sizeof(char));
    memcpy(new_str.data, text, new_str.len);
    new_str.data[new_str.len] = '\0';

    return new_str;
}

void dynstr_free(dynstr string) {
    free(string.data);
}

void dynstr_append(dynstr* string, char* text) {
    int text_len = strlen(text);
    maybe_realloc((dynobj*)string, text_len, sizeof(char));

    memcpy(&string->data[string->len], text, text_len);
    string->len += text_len;
}

void dynstr_append_str(dynstr* string, str text) {
    maybe_realloc((dynobj*)string, text.len, sizeof(char));
    memcpy(&string->data[string->len], text.data, text.len);
    string->len += text.len;
}

void dynstr_append_char(dynstr* string, char c) {
    maybe_realloc((dynobj*)string, 1, sizeof(char));

    string->data[string->len] = c;
    string->len++;
    string->data[string->len] = '\0';
}

// [start, end)
void dynstr_remove(dynstr* string, int start, int end) {
    if (start < 0 || end > string->len) return;

    int num_removed = end - start;
    /* `adjust` is the offset to be subtracted from
    the characters that will be moved left after the
    `end` index */
    int adjust = 0;
    for (int i = start; i < string->len; i++) {
        if (i < end) adjust++;
        else string->data[i-adjust] = string->data[i];
    }
    string->len -= num_removed;
    string->data[string->len] = '\0';
    // TODO: heuristic for reducing allocation size
    // maybe_realloc((dynobj*)string, -(end - start), sizeof(char));
}

void dynstr_clear(dynstr* string) {
    maybe_realloc((dynobj*)string, -string->len, sizeof(char));
    string->len = 0;
}

int dynstr_compare(dynstr* a, dynstr* b) {
    return strcmp(a->data, b->data);
}

int dynstr_compare_n(dynstr* a, dynstr* b, int n) {
    return strncmp(a->data, b->data, n);
}

void dynstr_to_lower(dynstr* string) {
    for (int i = 0; i < string->len; i++) {
        if (string->data[i] >= 'A' && string->data[i] <= 'Z')
            string->data[i] += ' ';
    }
}

void dynstr_to_upper(dynstr* string) {
    for (int i = 0; i < string->len; i++) {
        if (string->data[i] >= 'a' && string->data[i] <= 'z')
            string->data[i] -= ' ';
    }
}

void dynstr_print(dynstr string) {
    printf("%s", string.data);
}

void dynstr_printf(dynstr format, ...) {
    va_list args = {0};
    va_start(args, format);
    vprintf(format.data, args);
    va_end(args);
}

void dynstr_println(dynstr string) {
    printf("%s\n", string.data);
}

str dynstr_to_str(dynstr* src) {
    str new_str = str_create_from(src->data);
    free(src->data);
    return new_str;
}

str_arr str_arr_create(void) {
    str_arr new_arr;

    new_arr.cap = DYN_BASE_SIZE;
    new_arr.len = 0;
    new_arr.data = malloc(sizeof(str) * new_arr.cap);

    return new_arr;
}

str_arr str_arr_create_from(str* arr) {
    str_arr new_arr = {0};

    // Get array length (`arr` must be null-terminated)    
    str* ptr = &arr[0];
    int arr_len = 0;
    while (ptr) {
        arr_len++;
        ptr++;
    }

    new_arr.cap = arr_len * DYN_GROWTH_RATE;
    new_arr.len = arr_len;
    new_arr.data = malloc(sizeof(str) * new_arr.cap);
    memcpy(new_arr.data, arr, sizeof(str) * arr_len);

    return new_arr;
}

void str_arr_free(str_arr arr) {
    for (int i = 0; i < arr.len; i++)
        str_free(arr.data[i]);
    free(arr.data);
}

str str_arr_get(str_arr arr, int index) {
    if (index >= arr.len || index < 0)
        return NULL_STR;
    return arr.data[index];
}

void str_arr_append(str_arr* arr, str new_str) {
    maybe_realloc((dynobj*)arr, 1, sizeof(str));

    memcpy(&arr->data[arr->len], &new_str, sizeof(str));
    arr->len++;
}

str str_arr_remove(str_arr* arr, int index) {
    if (index < 0 || index >= arr->len)
        return NULL_STR;
    // Remove the str from the array
    str removed = str_create_from(arr->data[index].data);
    arr->data[index] = NULL_STR;
    // Move every following item back by one spot
    for (int i = index; i < arr->len; i++) {
        arr->data[i] = arr->data[i+1];
    }
    arr->len--;
    maybe_realloc((dynobj*)arr, -1, sizeof(str));

    return removed;
}

void str_arr_print(str_arr arr) {
    printf("[");
    for (int i = 0; i < arr.len; i++) {
        printf("\"%s\"", arr.data[i].data);
        if (i != arr.len - 1) printf(", ");
    }
    printf("]\n");
}

str str_arr_to_str(str_arr* arr, str* separator) {
    dynstr joined = dynstr_create();
    for (int i = 0; i < arr->len; i++) {
        dynstr_append_str(&joined, arr->data[i]);
        if (separator != NULL)
            dynstr_append_str(&joined, *separator);
    }
    str_arr_free(*arr);
    return dynstr_to_str(&joined);
}