#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "arena.h"
#include "ini.h"

void test_empty_file() {
  const char* const data = "";
  struct ini_file_t ini = {0};

  ini_parse(NULL, &ini, data);

  assert(ini.entries == NULL);
  assert(ini.count == 0);
  assert(ini.capacity == 0);
}

void test_one_key_value() {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "k=v";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 1);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "k", 2) == 0);
  assert(memcmp(ini.entries[0].value, "v", 2) == 0);
}

void test_one_key_quoted_value() {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "k=\"v\"";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 1);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "k", 2) == 0);
  assert(memcmp(ini.entries[0].value, "\"v\"", 4) == 0);
}

void test_allow_escaped_quotes_on_value() {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "k=\"\\\"v\"";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 1);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "k", 2) == 0);
  assert(memcmp(ini.entries[0].value, "\"\\\"v\"", 6) == 0);
}

void test_invalid_quotes_on_value() {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "k=\"v\"";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 1);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "k", 2) == 0);
  assert(memcmp(ini.entries[0].value, "\"\\\"v\"", 6) == 0);
}

void test_2_lines() {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "k1=\"v1\"\nk2=v2";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 2);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "k1", 3) == 0);
  assert(memcmp(ini.entries[0].value, "\"v1\"", 5) == 0);
  assert(memcmp(ini.entries[1].key, "k2", 3) == 0);
  assert(memcmp(ini.entries[1].value, "v2", 3) == 0);
}

int main(void) {
  printf("test ini\n");
  test_empty_file();
  test_one_key_value();
  test_one_key_quoted_value();
  test_allow_escaped_quotes_on_value();
  test_2_lines();
  printf("done.\n");
  return 0;
}
