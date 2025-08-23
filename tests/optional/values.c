#include "optional.h"
#include "test.h"

typedef struct {
    int num_somethings;
    bool is_this_true;
} MyType;

DEFINE_OPTIONAL(MyType)

int main(int argc, char* argv[]) {
    Optional(int) maybe_num = None(int);
    if (maybe_num.is_none)
        puts("none");

    maybe_num = Some(int, 5);
    if (!maybe_num.is_none)
        printf("%d\n", maybe_num.val);

    Optional(MyType) maybe_my_type =
        Some(MyType, ((MyType){.num_somethings = 0, .is_this_true = true}));

    if (!maybe_my_type.is_none)
        printf(
            "%d %s\n",
            maybe_my_type.val.num_somethings,
            maybe_my_type.val.is_this_true ? "true" : "false"
        );

    PASS;
}