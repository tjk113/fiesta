#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#define _FILE_OFFSET_BITS 64
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "file.h"
#include "str.h"

#define _FILE_NOT_OPEN_POS -1

File file_open(str filename, FileAccessModes access_modes) {
    File file = {0};
    // Create file access mode string
    dynstr access_modes_str = dynstr_create();
    if ((access_modes & FileWrite) && !(access_modes & FileRead)) {
        dynstr_append(&access_modes_str, "w");
    }
    else if (access_modes & FileRead) {
        if (access_modes & FileWrite) {
            /* Files opened with `FileRead | FileWrite` 
            access modes will not be truncated unless
            FileTruncate is also provided. FileTruncate
            does nothing for any other access mode. */
            if (access_modes & FileTruncate)
                dynstr_append(&access_modes_str, "w+");
            else
                dynstr_append(&access_modes_str, "r+");
        }
        else
            dynstr_append_char(&access_modes_str, 'r');
    }
    else if (access_modes & FileAppend) {
        dynstr_append_char(&access_modes_str, 'a');
        if (access_modes & FileRead)
            dynstr_append_char(&access_modes_str, '+');
    }
    if (access_modes & FileBinary)
        dynstr_append_char(&access_modes_str, 'b');
    /* 'x' can be appended to any 'w' mode to force
    opening to fail if the file already exists */
    if ((access_modes & FileWrite) && (access_modes & FileMustNotExist))
        dynstr_append_char(&access_modes_str, 'x');

    file.ptr = fopen(filename.data, access_modes_str.data);
    // Magic value to indicate that opening failed
    if (file.ptr == NULL)
        file.position = _FILE_NOT_OPEN_POS;
    else
        file.position = file_get_position(file);

    file.access_modes = access_modes;

    return file;
}

bool file_is_open(File file) {
    return file.position != _FILE_NOT_OPEN_POS;
}

void file_close(File* file) {
    fclose(file->ptr);
    file->position = _FILE_NOT_OPEN_POS;
}

bool file_seek(File* file, int64_t offset, FilePositionOrigin origin) {
    bool result;
#ifdef __linux__
    result = !fseeko(file->ptr, offset, origin);
#elifdef _WIN32
    result = !_fseeki64(file->ptr, offset, origin);
#endif
    file->position = _ftelli64(file->ptr);
    return result;
}

void file_rewind(File* file) {
    rewind(file->ptr);
    file->position = file_get_position(*file);
}

int64_t file_get_length(File* file) {
    int64_t current_position = file_get_position(*file);
    file_seek(file, 0, FilePositionEnd);
    int64_t length = file_get_position(*file);
    file_seek(file, current_position, SEEK_SET);
    return length;
}

int64_t file_get_position(File file) {
#ifdef __linux__
    return ftello(file.ptr);
#elifdef _WIN32
    return _ftelli64(file.ptr);
#endif
}

str file_read_str(File* file, int64_t size) {
    char buf[8192] = {};
    fread(buf, sizeof(uint8_t), size, file->ptr);
    file->position = file_get_position(*file);
    dynstr tmp = DSTR(buf);
    return dynstr_to_str(&tmp);
}

str_arr file_read_lines(File* file, int64_t max_line_length) {
    str_arr lines = str_arr_create();
    char line[max_line_length];
    while (fgets(line, max_line_length, file->ptr)) {
        dynstr tmp = DSTR(line);
        str line_str = dynstr_to_str(&tmp);
        // Remove the newlines
        if (line_str.data[line_str.len - 1] == '\n')
            line_str.data[line_str.len - 1] = '\0';
        str_arr_append(&lines, line_str);
        memset(line, 0, max_line_length);
    }
    file->position = file_get_position(*file);
    return lines;
}

#define FILE_READ_GENERATOR(suffix, type)                                    \
    ssize_t file_read_##suffix(File* file, type* buffer, size_t count) {     \
        size_t bytes_read = 0;                                               \
        for (size_t i = 0; i < count; i++) {                                 \
            size_t last_bytes_read = 0;                                      \
            last_bytes_read = fread(&buffer[i], sizeof(type), 1, file->ptr); \
            bytes_read += last_bytes_read;                                   \
            if (last_bytes_read < 1) {                                       \
                if (feof(file->ptr))                                         \
                    break;                                                   \
                else if (ferror(file->ptr))                                  \
                    return -1;                                               \
            }                                                                \
        }                                                                    \
        file->position = file_get_position(*file);                           \
        return bytes_read;                                                   \
    }                                                                        \

FILE_READ_GENERATOR(i8, int8_t)
FILE_READ_GENERATOR(u8, uint8_t)
FILE_READ_GENERATOR(i16, int16_t)
FILE_READ_GENERATOR(u16, uint16_t)
FILE_READ_GENERATOR(i32, int32_t)
FILE_READ_GENERATOR(u32, uint32_t)
FILE_READ_GENERATOR(i64, int64_t)
FILE_READ_GENERATOR(u64, uint64_t)
FILE_READ_GENERATOR(f32, float)
FILE_READ_GENERATOR(f64, double)

size_t file_write_str(File* file, str string) {
    size_t bytes_written = fwrite(string.data, sizeof(uint8_t), string.len, file->ptr);
    file->position = file_get_position(*file);
    return bytes_written;
}

#define FILE_WRITE_GENERATOR(suffix, type)                                     \
    ssize_t file_write_##suffix(File* file, type* data, size_t count) {        \
        size_t bytes_written = 0;                                              \
        for (size_t i = 0; i < count; i++) {                                   \
            size_t last_bytes_written = 0;                                     \
            last_bytes_written = fwrite(&data[i], sizeof(type), 1, file->ptr); \
            bytes_written += last_bytes_written;                               \
            if (last_bytes_written < 1) {                                      \
                if (feof(file->ptr))                                           \
                    break;                                                     \
                else if (ferror(file->ptr))                                    \
                    return -1;                                                 \
            }                                                                  \
        }                                                                      \
        file->position = file_get_position(*file);                             \
        return bytes_written;                                                  \
    }                                                                          \

FILE_WRITE_GENERATOR(i8, int8_t)
FILE_WRITE_GENERATOR(u8, uint8_t)
FILE_WRITE_GENERATOR(i16, int16_t)
FILE_WRITE_GENERATOR(u16, uint16_t)
FILE_WRITE_GENERATOR(i32, int32_t)
FILE_WRITE_GENERATOR(u32, uint32_t)
FILE_WRITE_GENERATOR(i64, int64_t)
FILE_WRITE_GENERATOR(u64, uint64_t)
FILE_WRITE_GENERATOR(f32, float)
FILE_WRITE_GENERATOR(f64, double)