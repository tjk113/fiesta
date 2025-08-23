#include <stdlib.h>

#include "test.h"
#include "file.h"

int main() {
    File file = file_open(STR("tests/file/test_i8"), FileRead | FileBinary);
    ASSERT(file_is_open(file), "File open failed");
        
    file_seek(&file, 0, FilePositionEnd);
    ASSERT(file_get_position(file) == 10, "File seek failed");

    file_rewind(&file);
    ASSERT(file_get_position(file) == 0, "File rewind failed");

    int8_t nums[10] = {};
    for (size_t i = 0; i < 10; i++) {
        file_read(&file, &nums[i], 1);
        printf("%d: %X\n", file_get_position(file), nums[i]);
    }

    file_close(&file);
    PASS;
}