// vector.c
// Very primitive, generic vector type with a similar functionality to C++'s
// std::vector, in plain C. Note that this vector type has pointer-based
// interface, but can be effectively utilized using wrapper functions for
// individual types if needed (see demonstration).
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <stdbool.h>
#include <stddef.h>

typedef struct vector vector;
typedef void (*vector_element_destructor_fn)(void *);

vector * vector_construct(size_t element_size, vector_element_destructor_fn element_destructor);
void vector_destruct(vector *self);

void * vector_at(vector *self, size_t position);
const void * vector_const_at(const vector *self, size_t position);
void * vector_front(vector *self);
const void * vector_const_front(const vector *self);
void * vector_back(vector *self);
const void * vector_const_back(const vector *self);
void * vector_data(vector *self);
const void * vector_const_data(const vector *self);

void * vector_begin(vector *self);
void * vector_end(vector *self);
const void * vector_cbegin(const vector *self);
const void * vector_cend(const vector *self);

bool vector_empty(const vector *self);
size_t vector_size(const vector *self);
size_t vector_element_size(const vector *self);
size_t vector_capacity(const vector *self);
void vector_reserve(vector *self, size_t new_capacity);
void vector_shrink_to_fit(vector *self);

void vector_clear(vector *self);
void vector_insert(vector *self, size_t position, void *element);
void vector_erase(vector *self, size_t position);
void vector_erase_range(vector *self, size_t begin, size_t end);
void vector_push_back(vector *self, void *element);
void vector_pop_back(vector *self);
void vector_resize(vector *self, size_t new_size);

#define vector_construct_for(type, element_destructor) \
    vector_construct(sizeof(type), element_destructor)

#define vector_at_as(self, position, type) \
    ((type *) vector_at((self), (position)))

#define vector_const_at_as(self, position, type) \
    ((const type *) vector_const_at((self), (position)))

#define vector_front_as(self, type) \
    ((type *) vector_front((self)))

#define vector_const_front_as(self, type) \
    ((const type *) vector_const_front((self)))

#define vector_back_as(self, type) \
    ((type *) vector_back((self)))

#define vector_const_back_as(self, type) \
    ((const type *) vector_const_back((self)))

#define vector_data_as(self, type) \
    ((type *) vector_data((self)))

#define vector_const_data_as(self, type) \
    ((const type *) vector_const_data((self)))

#define vector_begin_as(self, type) \
    ((type *) vector_begin(self))

#define vector_end_as(self, type) \
    ((type *) vector_end(self))

#define vector_cbegin_as(self, type) \
    ((const type *) vector_cbegin(self))

#define vector_cend_as(self, type) \
    ((const type *) vector_cend(self))

#define vector_push_back_as(self, value, type)  \
    do {                                        \
        type __v = (value);                     \
        vector_push_back(self, &__v);           \
    } while (0)

#define vector_insert_as(self, position, value, type)   \
    do {                                                \
        type __v = (value);                             \
        vector_insert(self, position, &__v);            \
    } while (0)

// Implementation

#include <stdlib.h>
#include <string.h>

struct vector {
    size_t element_size;
    size_t capacity;
    void *begin;
    void *end;
    vector_element_destructor_fn element_destructor;
};

vector * vector_construct(size_t element_size, vector_element_destructor_fn element_destructor) {
    vector *result = malloc(sizeof(vector));
    if (result == NULL)
        exit(1);

    result->element_size = element_size;
    result->begin = result->end = NULL;
    result->capacity = 0;
    result->element_destructor = element_destructor;
    return result;
}

void vector_destruct(vector *self) {
    vector_clear(self);
    free(self);
}

void * vector_at(vector *self, size_t position) {
    return (void *)(((char *) self->begin) + (position * self->element_size));
}

const void * vector_const_at(const vector *self, size_t position) {
    return (void *)(((char *) self->begin) + (position * self->element_size));
}

void * vector_front(vector *self) {
    return self->begin;
}

const void * vector_const_front(const vector *self) {
    return self->begin;
}

void * vector_back(vector *self) {
    return (void *)(((char *) self->end) - self->element_size);
}

const void * vector_const_back(const vector *self) {
    return (void *)(((char *) self->end) - self->element_size);
}

void * vector_data(vector *self) {
    return self->begin;
}

const void * vector_const_data(const vector *self) {
    return self->begin;
}

void * vector_begin(vector *self) {
    return self->begin;
}

void * vector_end(vector *self) {
    return self->end;
}

const void * vector_cbegin(const vector *self) {
    return self->begin;
}
const void * vector_cend(const vector *self) {
    return self->end;
}

bool vector_empty(const vector *self) {
    return self->begin == self->end;
}

size_t vector_size(const vector *self) {
    return (((char *) self->end) - ((char *) self->begin)) / self->element_size;
}

size_t vector_element_size(const vector *self) {
    return self->element_size;
}

size_t vector_capacity(const vector *self) {
    return self->capacity;
}

void vector_reserve(vector *self, size_t new_capacity) {
    if (self->capacity >= new_capacity)
        return;

    void *new_begin = malloc(new_capacity * self->element_size);
    if (new_begin == NULL)
        exit(1);

    void *new_end = (void *)(((char *) new_begin) + vector_size(self) * self->element_size);
    memcpy(new_begin, self->begin, vector_size(self) * self->element_size);
    free(self->begin);
    self->begin = new_begin;
    self->end = new_end;
    self->capacity = new_capacity;
}

void vector_shrink_to_fit(vector *self) {
    const size_t size = vector_size(self);
    if (self->capacity == size)
        return;

    void *new_begin = malloc(size * self->element_size);
    if (new_begin == NULL)
        exit(1);

    void *new_end = (void *)(((char *) new_begin) + vector_size(self) * self->element_size);
    memcpy(new_begin, self->begin, size * self->element_size);
    free(self->begin);
    self->begin = new_begin;
    self->end = new_end;
    self->capacity = size;
}

void vector_clear(vector *self) {
    if (self->element_destructor != NULL) {
        void *it = self->begin;
        while (it != self->end) {
            self->element_destructor(it);
            it = ((char *) it) + self->element_size;
        }
    }
    self->end = self->begin;
}

void vector_insert(vector *self, size_t position, void *element) {
    vector_reserve(self, vector_size(self) + 1);
    void *where = ((char *) self->begin) + position * self->element_size;
    void *next = ((char *) where) + self->element_size;
    memcpy(next, where, self->element_size * (vector_size(self) - position));
    memcpy(where, element, self->element_size);
    self->end = (char *) self->end + self->element_size;
}

void vector_erase(vector *self, size_t position) {
    vector_erase_range(self, position, position + 1);
}

void vector_erase_range(vector *self, size_t begin, size_t end) {
    const size_t erase_size = (end - begin) * self->element_size;
    const size_t rest_size = (vector_size(self) - begin) * self->element_size - erase_size;
    void *destination = ((char *) self->begin) + begin * self->element_size;
    void *source = ((char *) self->begin) + end * self->element_size;
    memcpy(destination, source, rest_size);
    self->end = ((char *) self->end) - erase_size;
}

void vector_push_back(vector *self, void *element) {
    vector_reserve(self, vector_size(self) + 1);
    memcpy(self->end, element, self->element_size);
    self->end = (void *)(((char *) self->end) + self->element_size);
}

void vector_pop_back(vector *self) {
    self->end = (void *) (((char *) self->end) - self->element_size);
    self->element_destructor(self->end);
}

void vector_resize(vector *self, size_t new_size) {
    const size_t current_size = vector_size(self);

    if (new_size == current_size) {
        return;
    } if (new_size > current_size) {
        const size_t difference = (new_size - current_size) * self->element_size;
        vector_reserve(self, new_size);
        memset(self->end, 0x00, difference);
        self->end = ((char *) self->end) + difference;
    } else {
        const size_t difference = (current_size - new_size) * self->element_size;
        self->end = ((char *) self->end) - difference;
    }
}

// Demonstration

#include <stdint.h>
#include <stdio.h>

static void print_vector_data(const vector *vec) {
    const size_t size = vector_size(vec);
    printf("data: %p to %p\n", vector_cbegin(vec), vector_cend(vec));
    printf("capacity: %ld, empty ? %s\n", vector_capacity(vec), vector_empty(vec) ? "yes" : "no");

    printf("content: (%ld) [", size);
    size_t i = 0;
    for (const int *it = vector_cbegin_as(vec, int); it != vector_cend_as(vec, int); ++it, ++i) {
        const char *format = (i == (size - 1)) ? "%d" : "%d, ";
        printf(format, (i % 2 == 0) ? *it : *vector_const_at_as(vec, i, int));
    }
    printf("]\n\n");
}

static void print_single_int(void *i) {
    printf("'destructed' element with value %d\n", *(int *) i);
}

int main(void) {
    printf("construct()\n");
    vector *vec = vector_construct_for(int, print_single_int);
    print_vector_data(vec);

    printf("push_back(1), push_back(2), push_back(3), push_back(4), push_back(5)\n");
    vector_push_back_as(vec, 1, int);
    vector_push_back_as(vec, 2, int);
    vector_push_back_as(vec, 3, int);
    vector_push_back_as(vec, 4, int);
    vector_push_back_as(vec, 5, int);
    print_vector_data(vec);

    printf("insert(2, 6)\n");
    vector_insert_as(vec, 2, 6, int);
    print_vector_data(vec);

    printf("erase_range(2, 4)\n");
    vector_erase_range(vec, 2, 4);
    print_vector_data(vec);

    printf("erase(size() - 1)\n");
    vector_erase(vec, vector_size(vec) - 1);
    print_vector_data(vec);

    printf("first element: %d\n", *vector_const_front_as(vec, int));
    printf("last element: %d\n", *vector_const_back_as(vec, int));
    printf("data pointer: %p\n\n", vector_const_data(vec));

    printf("resize(6)\n");
    vector_resize(vec, 6);
    print_vector_data(vec);

    printf("pop_back()\n");
    vector_pop_back(vec);
    print_vector_data(vec);

    printf("shrink_to_fit()\n");
    vector_shrink_to_fit(vec);
    print_vector_data(vec);

    printf("clear()\n");
    vector_clear(vec);
    print_vector_data(vec);

    printf("push_back(7), push_back(8)\n");
    vector_push_back_as(vec, 7, int);
    vector_push_back_as(vec, 8, int);
    print_vector_data(vec);

    printf("resize(1)\n");
    vector_resize(vec, 1);
    print_vector_data(vec);

    printf("destruct()\n");
    vector_destruct(vec);

    return 0;
}
