# fiesta 🎉
A utility library for making C more fun to write!

## Utilities
### str
Strings, dynamic strings, and string arrays
### file
Convenient wrappers around file IO

## Building
Here are the available Makefile targets:
- `lib`: Build the library (**Default**)
  - `dbg`, `opt`, or `dbgopt` can be used instead of `lib` to create debug, optimized, or optimized debug builds, respectively
- `tests`: Build the library tests
- `docs`: Build the documentation

## Usage
Library headers should be prefixed with `fiesta/` when included in your project: `#include "fiesta/str.h"`. Here is an example of how to link with the library using gcc: `gcc -o example example.o -L<fiesta_path>/lib -lfiesta -I<fiesta_path>/include`