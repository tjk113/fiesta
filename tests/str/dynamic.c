#include "test.h"
#include "str.h"

int main() {
    dynstr string = DSTR("UPPERCASE");
    dynstr_println(string);

    dynstr_to_lower(string);
    dynstr_println(string);

    dynstr_free(string);
    PASS;
}