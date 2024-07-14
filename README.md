# fiesta 🎉
A utility libraries for making C more fun to write!

## Utilities
### str
Heap-allocated strings, dynamic strings, and string arrays

## Building
For now, just run `make` to build all of the utilities. In the future, support may be added for building only specified parts of the library.

## Usage
In order to help keep track of this library's external status to your codebase, this library's directories are structured with the intention that the individual utility header files will be prefixed like this: `#include "fiesta/utility.h"`. This is done by including the *parent directory* of the fiesta folder in your build system / command line arguments. If you don't want to prefix the header files like this, include the fiesta folder itself instead. Here is an example of linking the library using gcc, assuming the fiesta directory exists on the same level as the directory in which `example.c` is being compiled: `gcc -o example example.o -L../fiesta -lfiesta -I..`