#include "optional.h"
#include "test.h"

int main(int argc, char* argv[]) {
    OptionalPtr(uint8_t) maybe_byte_ptr = SomePtr(uint8_t, 0xDEADBEEF);
    if (!maybe_byte_ptr.is_none)
        printf("0x%X\n", maybe_byte_ptr.val);

    maybe_byte_ptr = NonePtr(uint8_t);
    if (maybe_byte_ptr.is_none)
        puts("none");

    PASS;
}