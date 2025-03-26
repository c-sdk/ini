#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "ini.h"
#include "arena.h"

void ini_create_with_memory(struct ini_file_t *ini, size_t capacity, struct ini_entry_t* memory) {
  ini->capacity = capacity;
  ini->count = 0;
  ini->entries = memory;
}

enum csv_parser_item_t {
  INI_PARSER_INVALID = -1,
  INI_PARSER_INIT,
  INI_PARSE_KEY,
  INI_PARSER_QUOTED_ITEM,
  INI_PARSER_ITEM,
  INI_PARSER_SEPARATOR,
  INI_PARSER_EOL,
  INI_PARSER_EOF
};

static bool _key_character(int character, int delimiter) {
  return character != 13 && character != delimiter;
}


static size_t _reach_eol(char* position) {
  if (*position == '\n') { return 1; }
  if (*position == '\r' && *(position + 1) == '\n') { return 2; }
  return 0;
}

static char* _parse_quoted_item(const char* position, unsigned char delimiter) {
  (void)delimiter;
  char* next = (char*)position;
  int character = 0;
  while ((character = *++next) != 0) {
    size_t eol = 0;
    if ((eol = _reach_eol(next)) > 0) {
      next -= 1;
      break;
    }
    if (character == '\\' && *(next + 1) == '"') {
      next += 2;
    }
    if (character == 34) {
      ++next;
      break;
    }
  }
  return next;
}

static const char* _parse_item(const char* position, int delimiter) {
  while (*position++ != 0 && _key_character(*position, delimiter)) {
    size_t eol = 0;
    if ((eol = _reach_eol((char*)position)) > 0) {
      break;
    }
  }
  return position;
}

static size_t _parse(const char* const position,
                     enum csv_parser_item_t *item,
                     int delimiter,
                     int expecting_value) {
  if (*position == 0) {
    *item = INI_PARSER_EOF;
    return 0;
  }

  if (*position == delimiter) {
    *item = INI_PARSER_SEPARATOR;
    return 1;
  }

  switch (*position) {
  case '\r': {
    if (*(position+1) != '\n') {
      *item = INI_PARSER_INVALID;
      return 0;
    }
    *item = INI_PARSER_EOL;
    return 2;
  } break;
  case '\n':
    *item = INI_PARSER_EOL;
    return 1;
  case '"': {
    char* end = _parse_quoted_item(position, delimiter);
    *item = INI_PARSER_QUOTED_ITEM;
    return end - position;
  } break;
  default: {
    if (expecting_value) {
      char* end = (char*)position;
      while (*end != 0 && _reach_eol(++end) == 0);
      *item = INI_PARSER_ITEM;
      return end - position;
    } else {
      const char* end = _parse_item(position, '=');
      *item = INI_PARSE_KEY;
      return end - position;
    }
  }
  }

  return 0;
}

void ini_parse(arena_t* arena, struct ini_file_t *ini, const char *const str) {
  char* position = (char*)str;
  struct ini_entry_t* current = current = &ini->entries[ini->count];
  size_t read = 0;
  enum csv_parser_item_t item = INI_PARSER_INIT;
  bool expecting_value = false;

  while ((read = _parse(position, &item, '=', expecting_value)), item != INI_PARSER_EOF) {
    switch (item) {
    case INI_PARSE_KEY: {
      current->key = arena_string_with_null(arena, position, read + 1);
      expecting_value = true;
    } break;
    case INI_PARSER_QUOTED_ITEM:
    case INI_PARSER_ITEM: {
      current->value = arena_string_with_null(arena, position, read + 1);
      expecting_value = false;
      ++ini->count;
    } break;
    case INI_PARSER_EOL: {
      current = &ini->entries[ini->count];
    } break;
    case INI_PARSER_SEPARATOR:
    default:;
    }

    position += read;
  }
}

const char* ini_find_by_key(struct ini_file_t* ini, const char* const key) {
  for (size_t i = 0; i < ini->count; ++i) {
    if (memcmp(ini->entries[i].key, key, strlen(key) + 1) == 0) {
      return ini->entries[i].value;
    }
  }
  return NULL;
}
