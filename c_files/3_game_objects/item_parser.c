#include "../../headers/1_includes.h"

ITEM parse_item(char *line, int cur_line) {
  ITEM item;
  int rarity_val, type_val;

  item.x_pos = item.y_pos = item.am = 0;

  if (cur_line <= 1) {
    if (line[0] == '\n')
      strcpy(cur_line == 0 ? item.name : item.desc, "undefined");
    else {
      char *element = cur_line == 0 ? item.name : item.desc;
      sscanf(line, "%[^\n]", element);
      if (get_width(element) > CANV_W - 50) fit_text_to_width(element, CANV_W - 50);
    }
    int i = 0;

    return item;
  }
  if (cur_line == 2) {
    sscanf(line, "%c%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d",
           &item.content, // Content
           &item.rar,     // Rarity
           &item.price,   // Price
           &item.dur,     // Durability
           &item.type);   // Type
    return item;
  }
  if (cur_line == 3) {
    BONUS *b = &item.bonus[cur_line - 3];
    sscanf(line, "%*[^0-9]%s%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d",
           b->name,    // name
           &b->dur,    // duration
           &b->target, // target
           &b->value); // value
    return item;
  }
  return item;
}

ITEM *read_items_from_file(const char *filename, int *num_items) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open item file");
    *num_items = 0;
    return NULL;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int capacity = 10;
  *num_items = 0;

  int cur_line = 0;

  ITEM *items = malloc(capacity * sizeof(ITEM));
  if (!items) {
    perror("Failed to allocate memory for items");
    fclose(file);
    return NULL;
  }

  while ((read = getline(&line, &len, file)) != -1) {
    line[strcspn(line, "\n")] = 0;

    int is_empty = 1;
    for (char *p = line; *p; p++) {
      if (!isspace((unsigned char)*p)) {
        is_empty = 0;
        break;
      }
    }

    if (*num_items >= capacity) {
      capacity *= 2;
      items = realloc(items, capacity * sizeof(ITEM));
      if (!items) {
        perror("Failed to reallocate memory for items");
        free(line);
        fclose(file);
        return NULL;
      }
    }
    if (is_empty) {
      cur_line = 0;
      (*num_items)++;
      continue;
    }
    items[*num_items] = parse_item(line, cur_line);
    cur_line++;
  }

  free(line);
  fclose(file);

  return items;
}
