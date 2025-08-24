#pragma once

#ifndef _WIN32
#include <sys/types.h>
#endif
#include <stdint.h>
#include <stddef.h>

/* Optional */

// Optional type wrapping a value of `type` (must be defined using `DEFINE_OPTIONAL`).
#define Optional(type) Optional_##type
// Optional type wrapping a pointer to `type` (must be defined using `DEFINE_OPTIONAL`).
#define OptionalPtr(type) Optional_##type##_Ptr

// Define optional value and pointer types for `type`.
#define DEFINE_OPTIONAL(type) \
    typedef struct {          \
        type val;             \
        bool is_none;         \
    } Optional(type);         \
    typedef struct {          \
        type* val;            \
        bool is_none;         \
    } OptionalPtr(type);

//\ Special case for void*, because void
//\ is always a pointer, never a value.
typedef struct {
    void* val;
    bool is_none;
} OptionalPtr(void);

DEFINE_OPTIONAL(bool)
DEFINE_OPTIONAL(short)
DEFINE_OPTIONAL(char)
DEFINE_OPTIONAL(long)
DEFINE_OPTIONAL(int)
DEFINE_OPTIONAL(size_t)
DEFINE_OPTIONAL(ssize_t)
DEFINE_OPTIONAL(int8_t)
DEFINE_OPTIONAL(uint8_t)
DEFINE_OPTIONAL(int16_t)
DEFINE_OPTIONAL(uint16_t)
DEFINE_OPTIONAL(int32_t)
DEFINE_OPTIONAL(uint32_t)
DEFINE_OPTIONAL(int64_t)
DEFINE_OPTIONAL(uint64_t)
DEFINE_OPTIONAL(float)
DEFINE_OPTIONAL(double)

// An optional value of None.
#define None(type) (Optional(type)){.is_none = true}
// An optional pointer to None.
#define NonePtr(type) (OptionalPtr(type)){.is_none = true}
// An optional value of `type`.
#define Some(type,value) (Optional(type)){.val = (type)value, .is_none = false}
// An optional pointer to `type`.
#define SomePtr(type,value) (OptionalPtr(type)){.val = (type*)value, .is_none = false}
