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

void test_correctly_detect_eol_only_cr(void) {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "HOST=127.0.0.1\nPORT=8080";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 2);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "HOST", 5) == 0);
  assert(memcmp(ini.entries[0].value, "127.0.0.1", strlen("127.0.0.1")) == 0);
  assert(memcmp(ini.entries[1].key, "PORT", 5) == 0);
  assert(memcmp(ini.entries[1].value, "8080", strlen("8080")) == 0);
}

void test_correctly_detect_eol_lf_cr(void) {
  arena_t arena = {0};
  arena_create(&arena, 4096);

  struct ini_file_t ini = {
    .capacity = 12,
    .count = 0,
    .entries = arena_alloc(&arena, sizeof(struct ini_entry_t) * 12)
  };

  const char* const data = "HOST=127.0.0.1\r\nPORT=8080\r\n";

  ini_parse(&arena, &ini, data);

  assert(ini.count == 2);
  assert(ini.capacity == 12);
  assert(memcmp(ini.entries[0].key, "HOST", 5) == 0);
  assert(memcmp(ini.entries[0].value, "127.0.0.1", strlen("127.0.0.1")) == 0);
  assert(memcmp(ini.entries[1].key, "PORT", 5) == 0);
  assert(memcmp(ini.entries[1].value, "8080", strlen("8080")) == 0);
}

int main(void) {
  printf("test ini\n");
  test_empty_file();
  test_one_key_value();
  test_one_key_quoted_value();
  test_allow_escaped_quotes_on_value();
  test_2_lines();
  test_correctly_detect_eol_only_cr();
  test_correctly_detect_eol_lf_cr();
  printf("done.\n");
  return 0;
}
