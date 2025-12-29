#include <librr/filesystem.h>

#include <stdio.h>

void * rr_load_file(
    const char *path,
    size_t *out_file_length,
    rr_allocator_s *allocator
) {
    FILE *file = fopen(
        path,
        "rb"
    );
    if ( ! file)
    {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t len_file = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *content = rr_alloc(
        allocator,
        len_file
    );
    fread(
        content,
        1,
        len_file,
        file
    );
    fclose(file);

    if (out_file_length)
    {
        *out_file_length = len_file;
    }
    return content;
}

