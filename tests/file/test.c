#include <stdlib.h>

#include "file.h"
#include "str.h"

int main(int argc, char* argv[]) {
    File file = file_open(STR("tests/file/test_str.txt"), FileRead);
    if (!file_is_open(file)) {
        printf("failed to open file\n");
        return 1;
    }
    str file_text = file_read_str(&file, file_get_length(&file));
    str_println(file_text);
    file_close(&file);

    file = file_open(STR("tests/file/test_i8"), FileRead | FileBinary);
    if (!file_is_open(file)) {
        printf("failed to open file\n");
        return 1;
    }
    int8_t* nums = malloc(10);
    file_read(&file, nums, 10);
    for (size_t i = 0; i < 10; i++)
        printf("%X\n", nums[i]);
    free(nums);
    file_close(&file);

    file = file_open(STR("tests/file/test_lines.txt"), FileRead);
    str_arr lines = file_read_lines(&file, 20);
    str_arr_print(lines);
    str_arr_free(lines);
    file_close(&file);

    return 0;
}