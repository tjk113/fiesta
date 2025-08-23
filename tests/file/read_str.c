#include <stdlib.h>

#include "test.h"
#include "file.h"

int main() {
    File file = file_open(STR("tests/file/test_str.txt"), FileRead | FileText);
    ASSERT(file_is_open(file), "File open failed");

    str text = file_read_str(&file, 512);
    str_println(text);
    free(text.data);

    file_close(&file);
    PASS;
}