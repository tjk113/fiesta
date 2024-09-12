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
#ifdef __linux__
        file.position = ftello(file.ptr);
#endif
#ifdef _WIN32
        file.position = _ftelli64(file.ptr);
#endif

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
#ifdef __linux__
    return fseeko(file->ptr, offset, origin);
#endif
#ifdef _WIN32
    return _fseeki64(file->ptr, offset, origin);
#endif
}

void file_rewind(File* file) {
    rewind(file->ptr);
    file->position = 0;
}

int64_t file_get_length(File* file) {
    file_seek(file, 0, FilePositionEnd);
#ifdef __linux__
    int64_t length = ftello(file->ptr);
#endif
#ifdef _WIN32
    int64_t length = _ftelli64(file.ptr);
#endif
    file_seek(file, file->position, SEEK_SET);
    return length;
}

str file_read_str(File* file, int64_t size) {
    str string = str_create(size);
    int bytes_read = 0;
    bytes_read = fread(string.data, sizeof(uint8_t), size, file->ptr);
    return string;
}

str_arr file_read_lines(File* file, int64_t max_line_length) {
    str_arr lines = str_arr_create();
    char line[max_line_length];
    while (fgets(line, max_line_length, file->ptr)) {
        str line_str = STR(line);
        // Remove the newlines
        if (line_str.data[line_str.len - 1] == '\n')
            line_str.data[line_str.len - 1] = '\0';
        str_arr_append(&lines, line_str);
        memset(line, 0, max_line_length);
    }
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
    return fwrite(string.data, sizeof(uint8_t), string.len, file->ptr);
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
