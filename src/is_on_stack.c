/* is_on_stack.c
 * A small utility function for x86-64 Linux, that returns true, if the
 * variable, whose address was given resides on the stack. Compliant with the
 * ANSI C standard, and was tested with the x86-64 GCC, Clang, and ICC
 * compilers.
 * Author: Dénes Fintha
 * Year: 2022
 * -------------------------------------------------------------------------- */

/* Interface */

int is_on_stack(const volatile void *address);

/* Implementation */

#include <stdint.h>

int is_on_stack(const volatile void *address) {
    volatile char top;
    uintptr_t top_address = (uintptr_t) &top;
    return (top_address - 8) < ((uintptr_t) address);
}

/* Demonstration */

#include <stdio.h>
#include <stdlib.h>

static const char * int2tf(int boolean) {
    return boolean ? "true" : "false";
}

int main() {
    int stack;
    int *heap_ptr = (int *) malloc(sizeof(int));
    int *stack_ptr = &stack;
    printf("is_on_stack(stack_ptr) == %s\n", int2tf(is_on_stack(stack_ptr)));
    printf("is_on_stack(heap_ptr) == %s\n", int2tf(is_on_stack(heap_ptr)));
    free(heap_ptr);
    return EXIT_SUCCESS;
}
