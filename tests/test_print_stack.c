#include <stdio.h>
#include <stdint.h>

#include "../src/stack.h"

int main(void) {
    struct data_stack stack;
    init_data_stack(&stack);
    uint8_t elems[] = {0xde, 0xad, 0xbe, 0xef};
    for (int i = 0; i < sizeof elems / sizeof elems[0]; ++i) {
        push_data_stack(&stack, elems[i]);
    }
    print_stack(&stack, stdout);
    printf("\n");
}
