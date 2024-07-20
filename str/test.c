#include "fiesta/str.h"

int main(int argc, char* argv[]) {
    // The `STR` macro constructs a str from a null-terminated source
    str normal_str = STR("Hello, Fiesta!");
    str_println(normal_str);

    dynstr dynamic_str = DSTR("Horse!");
    dynstr_println(dynamic_str);
    dynstr_remove(&dynamic_str, 2, 3);
    dynstr_println(dynamic_str);

    str_arr split = str_split(normal_str, ' ');
    str_arr_print(split);

    str_free(normal_str);
    dynstr_free(dynamic_str);
    str_arr_free(split);
    return 0;
}