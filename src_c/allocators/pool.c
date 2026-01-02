#include <librr/allocators/pool.h>

void rr_reset_static_pool(
    rr_allocator_s *allocator
) {
    rr_static_pool_s *pool = allocator->specifics;
    pool->first_free = 0;

    size_t space_between_marker_starts = pool->unit_size + sizeof(rr_pool_marker_s);
    size_t unit_index = 0;
    while (unit_index < pool->num_units)
    {
        rr_pool_marker_s *marker = (void *) (((uint8_t *) pool->allocation) + unit_index * space_between_marker_starts);
        marker->index = unit_index;
        ++unit_index;
        marker->next_free = unit_index;
    }
    rr_pool_marker_s *last_marker = (void *) (((uint8_t *) pool->allocation) + (pool->num_units - 1) * space_between_marker_starts);
    last_marker->next_free = ~ 0;
}

rr_allocator_s * rr_new_static_pool(
    rr_allocator_s *backing_allocator,
    uint32_t num_units,
    uint32_t unit_size
) {
    rr_allocator_s *generic_allocator = rr_alloc(
        backing_allocator,
        sizeof(rr_static_pool_s)
      + sizeof(rr_allocator_s)
      + (num_units * (unit_size + sizeof(rr_pool_marker_s)))
    );

    generic_allocator->specifics = (void *) (((uint8_t *) generic_allocator) + sizeof(rr_allocator_s));
    generic_allocator->vtable.fn_cleanup = &rr_static_pool_cleanup;
    generic_allocator->vtable.fn_alloc = &rr_static_pool_alloc;
    generic_allocator->vtable.fn_free = &rr_static_pool_free;
    generic_allocator->vtable.fn_realloc = NULL;

    rr_static_pool_s *pool = generic_allocator->specifics;
    pool->allocation = (void *) (((uint8_t *) pool) + sizeof(rr_static_pool_s));
    pool->num_units = num_units;
    pool->unit_size = unit_size;
    pool->backing_allocator = backing_allocator;

    rr_reset_static_pool(generic_allocator);

    return generic_allocator;
}

bool rr_static_pool_cleanup(
    rr_allocator_s *allocator
) {
    rr_static_pool_s *pool = allocator->specifics;
    rr_free(pool->backing_allocator, allocator);
}

void * rr_static_pool_alloc(
    rr_allocator_s *allocator,
    size_t num_bytes
) {
    rr_static_pool_s *pool = allocator->specifics;

    // A pool can only allocate a fixed-size element. Less is fine, even though
    // rather wasteful. More isn't possible, so an error value must be returned.
    if (num_bytes > pool->unit_size)
    {
        return NULL;
    }

    // If 'first_free' contains the marker the very last free slot left behind
    // when it was allocated, then there are no more free slots at the moment.
    if (pool->first_free == (~ 0))
    {
        return NULL;
    }

    size_t space_between_marker_starts = pool->unit_size + sizeof(rr_pool_marker_s);
    rr_pool_marker_s *marker = (void *) (((uint8_t *) pool->allocation) + (pool->first_free * space_between_marker_starts));
    pool->first_free = marker->next_free;

    // Go to the byte right after the marker (where a next marker would start
    // if this was a tightly-packed array).
    return &marker[1];
}

void rr_static_pool_free(
    rr_allocator_s *allocator,
    void *allocation
) {
    rr_static_pool_s *pool = allocator->specifics;

    // Go back the size of one marker before this allocation to find
    // the start of the marker belonging to this element.

    rr_pool_marker_s *marker = &((rr_pool_marker_s *) allocation)[-1];

    marker->next_free = pool->first_free;
    pool->first_free = marker->index;
}

