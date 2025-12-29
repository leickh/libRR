#include <librr/allocators/arena.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rr_allocator_s * rr_new_static_arena(
    rr_allocator_s *backing_allocator,
    size_t capacity
) {
    size_t capacity_overhead = sizeof(rr_static_arena_s) + sizeof(rr_allocator_s) + sizeof(rr_arena_block_marker_s);

    rr_static_arena_s *arena = rr_alloc(
        backing_allocator,
        capacity + capacity_overhead
    );

    rr_allocator_s *allocator = rr_init_as_static_arena(
        backing_allocator,
        arena,
        capacity + capacity_overhead
    );

    return allocator;
}

rr_allocator_s * rr_init_as_static_arena(
    rr_allocator_s *backing_allocator,
    void *backing_allocation,
    size_t len_backing_allocation
) {
    size_t arena_address = (size_t) backing_allocation;

    if (len_backing_allocation < (sizeof(rr_static_arena_s) * 2))
    {
        return NULL;
    }

    rr_static_arena_s *arena = backing_allocation;
    arena->backing_allocator = backing_allocator;
    arena->capacity = len_backing_allocation;
    arena->usage = sizeof(rr_static_arena_s);
    arena->repartition_region = (void *) arena_address;

    // Create the generic allocator structure, first on the stack to be bale to allocate
    // space for it inside the arena, and then copy the stack-allocated generic allocator
    // into the heap-allocated structure.

    rr_allocator_s stack_allocated_allocator;
    stack_allocated_allocator.specifics = arena;
    stack_allocated_allocator.vtable.fn_cleanup = &rr_static_arena_cleanup;
    stack_allocated_allocator.vtable.fn_alloc = &rr_static_arena_alloc;
    stack_allocated_allocator.vtable.fn_free = NULL;
    stack_allocated_allocator.vtable.fn_realloc = &rr_static_arena_realloc;

    // Make the heap allocation and copy the stack-allocated allocator to the heap.

    rr_allocator_s *heap_allocated_allocator = rr_static_arena_alloc(
        &stack_allocated_allocator,
        sizeof(rr_allocator_s)
    );

    *heap_allocated_allocator = stack_allocated_allocator;

    return heap_allocated_allocator;
}



bool rr_static_arena_cleanup(
    rr_allocator_s *allocator
) {
    rr_static_arena_s *static_arena_head = allocator->specifics;
    rr_free(static_arena_head->backing_allocator, static_arena_head);
    return true;
}

void * rr_static_arena_alloc(
    rr_allocator_s *allocator,
    size_t num_bytes
) {
    rr_static_arena_s *static_arena_head = allocator->specifics;
    if ((static_arena_head->usage + num_bytes + sizeof(rr_arena_block_marker_s)) > static_arena_head->capacity)
    {
        return NULL;
    }
    rr_arena_block_marker_s *block_marker = (void *) &((uint8_t *) static_arena_head->repartition_region)[static_arena_head->usage];
    block_marker->length = num_bytes;
    static_arena_head->usage += sizeof(rr_arena_block_marker_s);

    void *allocation = &((uint8_t *) static_arena_head->repartition_region)[static_arena_head->usage];
    static_arena_head->usage += num_bytes;

    ++static_arena_head->num_allocations;

    return allocation;
}

rr_arena_block_marker_s * rr_static_arena_get_block_marker(
    rr_static_arena_s *static_arena,
    size_t target_offset
) {
    if (target_offset >= static_arena->num_allocations)
    {
        return NULL;
    }
    rr_arena_block_marker_s *marker = NULL;

    size_t offset = 0;
    while (offset <= target_offset)
    {
        marker = (void *) &((uint8_t *) static_arena->repartition_region)[offset];
        if ((offset + sizeof(rr_arena_block_marker_s)) > target_offset)
        {
            break;
        }
        offset += marker->length + sizeof(rr_arena_block_marker_s);
    }
    return marker;
}

void * rr_static_arena_realloc(
    rr_allocator_s *allocator,
    void *allocation,
    size_t new_num_bytes
) {
    rr_static_arena_s *static_arena_head = allocator->specifics;

    // If the block to resize is the last one that was allocated,
    // just increase the length of that; don't duplicate it.
    if (
        &((uint8_t *) static_arena_head->repartition_region)[static_arena_head->last_start]
        == allocation
    ) {
        static_arena_head->usage = static_arena_head->last_start + new_num_bytes;
        return allocation;
    }

    if ((static_arena_head->usage + new_num_bytes) > static_arena_head->capacity)
    {
        return NULL;
    }

    void *new_allocation = rr_static_arena_alloc(
        allocator,
        new_num_bytes
    );

    size_t old_allocation_offset = ((size_t) allocation) - ((size_t) static_arena_head->repartition_region);

    rr_arena_block_marker_s *block_marker = rr_static_arena_get_block_marker(static_arena_head, old_allocation_offset);
    memcpy(new_allocation, allocation, block_marker->length);

    return new_allocation;
}
