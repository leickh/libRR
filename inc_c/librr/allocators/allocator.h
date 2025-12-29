
#ifndef LIBRR_UTILITY_ALLOCATOR_H
#define LIBRR_UTILITY_ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct rr_allocator rr_allocator_s;
typedef struct rr_allocator_vtable rr_allocator_vtable_s;

struct rr_allocator_vtable
{
    bool (*fn_cleanup)(rr_allocator_s *allocator);
    void * (*fn_alloc)(rr_allocator_s *allocator, size_t num_bytes);
    void (*fn_free)(rr_allocator_s *allocator, void *allocation);
    void * (*fn_realloc)(rr_allocator_s *allocator, void *allocation, size_t num_bytes);
};

struct rr_allocator
{
    rr_allocator_vtable_s vtable;
    void *specifics;
};

void * rr_alloc(
    rr_allocator_s *allocator,
    size_t num_bytes
);

void rr_free(
    rr_allocator_s *allocator,
    void *allocation
);

void * rr_realloc(
    rr_allocator_s *allocator,
    void *allocation,
    size_t new_num_bytes
);

bool rr_cleanup_allocator(
    rr_allocator_s *allocator
);

rr_allocator_s rr_create_cstd_allocator();

#endif // LIBRR_UTILITY_ALLOCATOR_H

