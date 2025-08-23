#pragma once

#include <stdio.h>

#define FAIL(message)                     \
    do {                                  \
        fputs(message, stderr);           \
        return 1;                         \
    } while (0)
#define ASSERT(condition, message) if (!(condition)) FAIL(message)
#define PASS return 0