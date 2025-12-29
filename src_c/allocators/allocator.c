#include <librr/allocators/allocator.h>

#include <stdlib.h>

void *rr_drain_assignment_target = NULL;

bool rr_cleanup_wrapper(
    rr_allocator_s *allocator
) {
    allocator->specifics = NULL;

    allocator->vtable.fn_cleanup = NULL;
    allocator->vtable.fn_alloc = NULL;
    allocator->vtable.fn_free = NULL;
    allocator->vtable.fn_realloc = NULL;

    return true;
}

void * rr_malloc_wrapper(
    rr_allocator_s *allocator,
    size_t size
) {
    rr_drain_assignment_target = allocator;
    return malloc(size);
}

void rr_free_wrapper(
    rr_allocator_s *allocator,
    void *allocation
) {
    rr_drain_assignment_target = allocator;
    free(allocation);
}

void * rr_realloc_wrapper(
    rr_allocator_s *allocator,
    void *allocation,
    size_t size
) {
    rr_drain_assignment_target = allocator;
    return realloc(allocation, size);
}

rr_allocator_s rr_create_cstd_allocator()
{
    rr_allocator_s cstd_allocator;
    cstd_allocator.specifics = NULL;
    cstd_allocator.vtable.fn_cleanup = &rr_cleanup_wrapper;
    cstd_allocator.vtable.fn_alloc = &rr_malloc_wrapper;
    cstd_allocator.vtable.fn_free = &rr_free_wrapper;
    cstd_allocator.vtable.fn_realloc = &rr_realloc_wrapper;
    return cstd_allocator;
}



void * rr_alloc(
    rr_allocator_s *allocator,
    size_t num_bytes
) {
    if ( ! allocator)
    {
        return NULL;
    }
    if ( ! allocator->vtable.fn_alloc)
    {
        return NULL;
    }
    return allocator->vtable.fn_alloc(
        allocator,
        num_bytes
    );
}

void rr_free(
    rr_allocator_s *allocator,
    void *allocation
) {
    if ( ! allocator)
    {
        return;
    }
    if ( ! allocator->vtable.fn_free)
    {
        return;
    }
    allocator->vtable.fn_free(
        allocator,
        allocation
    );
}

void * rr_realloc(
    rr_allocator_s *allocator,
    void *allocation,
    size_t new_num_bytes
) {
    if ( ! allocator)
    {
        return NULL;
    }
    if ( ! allocator->vtable.fn_realloc)
    {
        return NULL;
    }
    return allocator->vtable.fn_realloc(
        allocator,
        allocation,
        new_num_bytes
    );
}

bool rr_cleanup_allocator(
    rr_allocator_s *allocator
) {
    if ( ! allocator)
    {
        return false;
    }
    if ( ! allocator->vtable.fn_cleanup)
    {
        return false;
    }
    return allocator->vtable.fn_cleanup(
        allocator
    );
}

