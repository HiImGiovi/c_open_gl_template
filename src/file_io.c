#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    assert(f && "Failed to open file");

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    assert(buffer && "Failed to allocate buffer");

    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}