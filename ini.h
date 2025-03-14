#ifndef _INI_H_
#define _INI_H_ 1

#include <stddef.h>

#include "arena.h"

struct ini_entry_t {
    char *key;
    char *value;
};

struct ini_file_t {
    struct ini_entry_t *entries;
    size_t count;
    size_t capacity;
};

void ini_create_with_memory(struct ini_file_t *ini,
                            size_t capacity,
                            struct ini_entry_t* memory);

void ini_parse(arena_t* arena,
               struct ini_file_t* ini,
               const char* const str);


#endif // _INI_H_
