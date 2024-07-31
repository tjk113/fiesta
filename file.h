#pragma once

#include <stdint.h>
#include <stdio.h>

#include "str.h"

typedef enum {
    FileBinary       = 0b0000'0001, // Text mode is implied to be 0
    FileTruncate     = 0b0000'0010,
    FileMustNotExist = 0b0000'0100,
    FileRead         = 0b0000'1000,
    FileWrite        = 0b0001'0000,
    FileAppend       = 0b0010'0000,
    FileAll          = 0b0011'1111
} FileAccessModes;

typedef struct {
    FILE* ptr;
    int64_t position;
    FileAccessModes access_modes;
} File;

typedef enum {
    FilePositionStart   = SEEK_SET,
    FilePositionCurrent = SEEK_CUR,
    FilePositionEnd     = SEEK_END
} FilePositionOrigin;

/* file */

// Open a file with the specified file access mode (modes are
// selected by bitwise OR'ing FileAccessModes values together;
// e.g. `FileRead | FileWrite` selects the "r+" access mode).
File    file_open(str filename, FileAccessModes access_modes);
// Check whether a file is open.
bool    file_is_open(File file);
// Close a file.
void    file_close(File* file);
// Seek to an offset in a file.
bool    file_seek(File* file, int64_t offset, FilePositionOrigin origin);
// Rewind a file back to its start.
void    file_rewind(File* file);
// Get a file's length.
int64_t file_get_length(File* file);

// Read a string (up to `size` in length) from a file.
str      file_read_str(File* file, int64_t size);
// Read the lines (each line being up to `max_line_length` in length) from a file.
str_arr  file_read_lines(File* file, int64_t max_line_length);
// Read `count` signed 8-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_i8(File* file, int8_t* buffer, size_t count);
// Read `count` unsigned 8-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_u8(File* file, uint8_t* buffer, size_t count);
// Read `count` signed 16-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_i16(File* file, int16_t* buffer, size_t count);
// Read `count` unsigned 16-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_u16(File* file, uint16_t* buffer, size_t count);
// Read `count` signed 32-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_i32(File* file, int32_t* buffer, size_t count);
// Read `count` unsigned 32-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_u32(File* file, uint32_t* buffer, size_t count);
// Read `count` signed 64-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers.
ssize_t  file_read_i64(File* file, int64_t* buffer, size_t count);
// Read `count` usigned 64-bit integers from a file. This function will return the
// total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read integers..
ssize_t  file_read_u64(File* file, uint64_t* buffer, size_t count);
// Read `count` 32-bit floating point numbers from a file. This function will return
// the total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read numbers.
ssize_t  file_read_f32(File* file, float* buffer, size_t count);
// Read `count` 64-bit floating point numbers from a file. This function will return
// the total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read numbers.
ssize_t  file_read_f64(File* file, double* buffer, size_t count);
// Read `count` integers / floating point numbers from a file. The function will return
// the total number of bytes read if successful, or -1 if there was an error. The caller
// is expected to allocate enough space in `buffer` to hold the read numbers. This is a
// macro that infers the type of the numbers being read.
#define  file_read(file,buffer,count)  \
    _Generic((buffer),                 \
             void*:     file_read_u8,  \
             bool*:     file_read_u8,  \
             int8_t*:   file_read_i8,  \
             uint8_t*:  file_read_u8,  \
             int16_t*:  file_read_i16, \
             uint16_t*: file_read_u16, \
             int32_t*:  file_read_i32, \
             uint32_t*: file_read_u32, \
             int64_t*:  file_read_i64, \
             uint64_t*: file_read_u64, \
             float*:    file_read_f32, \
             double*:   file_read_f64  \
    )(file,buffer,count)

// Write a string to a file.
size_t  file_write_str(File* file, str string);
// Write `count` signed 8-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_i8(File* file, int8_t* data, size_t count);
// Write `count` unsigned 8-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_u8(File* file, uint8_t* data, size_t count);
// Write `count` signed 16-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_i16(File* file, int16_t* data, size_t count);
// Write `count` unsigned 16-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_u16(File* file, uint16_t* data, size_t count);
// Write `count` signed 32-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_i32(File* file, int32_t* data, size_t count);
// Write `count` unsigned 32-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_u32(File* file, uint32_t* data, size_t count);
// Write `count` signed 64-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_i64(File* file, int64_t* data, size_t count);
// Write `count` unsigned 64-bit integers to a file. This function will return the
// total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_u64(File* file, uint64_t* data, size_t count);
// Write `count` 32-bit floating point numbers to a file. This function will return
// the total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_f32(File* file, float* data, size_t count);
// Write `count` 64-bit floating point numbers to a file. This function will return
// the total number of bytes written if successful, or -1 if there was an error.
ssize_t file_write_f64(File* file, double* data, size_t count);
// Write `count` integers / floating point numbers to a file. The function will return
// the total number of bytes written if successful, or -1 if there was an error. This is
// a macro that infers the type of the numbers being written.
#define file_write(file,data,count)     \
    _Generic((data),                    \
             void*:     file_write_u8,  \
             bool:      file_write_u8,  \
             int8_t*:   file_write_i8,  \
             uint8_t*:  file_write_u8,  \
             int16_t*:  file_write_i16, \
             uint16_t*: file_write_u16, \
             int32_t*:  file_write_i32, \
             uint32_t*: file_write_u32, \
             int64_t*:  file_write_i64, \
             uint64_t*: file_write_u64, \
             float*:    file_write_f32, \
             double*:   file_write_f64  \
    )(file,data,count)
