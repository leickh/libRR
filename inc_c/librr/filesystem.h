
#ifndef LIBRR_UTILITY_FILESYSTEM_H
#define LIBRR_UTILITY_FILESYSTEM_H

#include <stdint.h>

#include <librr/allocators/allocator.h>

void * rr_load_file(
    const char *path,
    size_t *out_length,
    rr_allocator_s *allocator
);

#endif // LIBRR_UTILITY_FILESYSTEM_H

