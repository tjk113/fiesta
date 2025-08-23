#include <stdlib.h>

#include "test.h"
#include "file.h"

int main() {
    File file = file_open(STR("tests/file/test_lines.txt"), FileRead | FileText);
    ASSERT(file_is_open(file), "File open failed");

    str_arr lines = file_read_lines(&file, 512);
    for (size_t i = 0; i < 3; i++)
        str_println(str_arr_get(lines, i));
        
    str_arr_free_elements(lines);
    file_close(&file);
    PASS;
}