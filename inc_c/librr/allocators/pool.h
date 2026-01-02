
#ifndef LIBRR_UTILITY_ARENA_H
#define LIBRR_UTILITY_ARENA_H

#include <stdint.h>

#include <librr/allocators/allocator.h>

typedef struct rr_static_pool rr_static_pool_s;
typedef struct rr_pool_marker rr_pool_marker_s;

struct rr_pool_marker
{
    uint32_t index;
    uint32_t next_free;
};

struct rr_static_pool
{
    
    uint32_t num_units;
    uint32_t unit_size;

    uint32_t first_free;
    void *allocation;

    rr_allocator_s *backing_allocator;
};

rr_allocator_s * rr_new_static_pool(
    rr_allocator_s *backing_allocator,
    uint32_t num_units,
    uint32_t unit_size
);

bool rr_static_pool_cleanup(
    rr_allocator_s *allocator
);

void * rr_static_pool_alloc(
    rr_allocator_s *allocator,
    size_t num_bytes
);

void rr_static_pool_free(
    rr_allocator_s *allocator,
    void *allocation
);

#endif // LIBRR_UTILITY_ARENA_H

