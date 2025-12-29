
#ifndef LIBRR_UTILITY_ARENA_H
#define LIBRR_UTILITY_ARENA_H

#include <stdint.h>

#include <librr/allocators/allocator.h>

typedef struct rr_static_arena rr_static_arena_s;
typedef struct rr_arena_block_marker rr_arena_block_marker_s;

struct rr_static_arena
{
    rr_allocator_s *backing_allocator;

    size_t last_start;
    size_t capacity;
    size_t usage;
    void *repartition_region;

    size_t num_allocations;
};

struct rr_arena_block_marker
{
    size_t length;
};

rr_allocator_s * rr_new_static_arena(
    rr_allocator_s *backing_allocator,
    size_t capacity
);

rr_allocator_s * rr_init_as_static_arena(
    rr_allocator_s *backing_allocator,
    void *backing_allocation,
    size_t len_backing_allocation
);



bool rr_static_arena_cleanup(
    rr_allocator_s *allocator
);

void * rr_static_arena_alloc(
    rr_allocator_s *allocator,
    size_t num_bytes
);

void rr_static_arena_free(
    rr_allocator_s *allocator,
    void *allocation
);

void * rr_static_arena_realloc(
    rr_allocator_s *allocator,
    void *allocation,
    size_t num_bytes
);

#endif // LIBRR_UTILITY_ARENA_H

