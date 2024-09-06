#include "../headers/includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ITEM parse_item(char *line, int cur_line) {
  ITEM item;
  int rarity_val, type_val;

  switch (cur_line) {
    case 0:
      sscanf(line, "%[^\n]", item.name);
      break;
    case 1:
      sscanf(line, "%c", &item.content);
      break;
    case 2:
      sscanf(line, "%[^\n]", item.desc);
      break;
    case 3:
      sscanf(line, "%d", &item.rar);
      break;
    case 4:
      sscanf(line, "%d", &item.price);
      break;
    case 5:
      sscanf(line, "%d", &item.val_inc);
      break;
    case 6:
      sscanf(line, "%d", &item.dur);
      break;
    case 7:
      sscanf(line, "%d", &item.type);
      break;
    default:
      break;
  }

  item.x_pos = 0;
  item.y_pos = 0;

  return item;
}

ITEM *read_items_from_file(const char *filename, int *num_items) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
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
    if (cur_line >= 8) {
      cur_line = 0;
      (*num_items)++;
      continue;
    }
    items[*num_items] = parse_item(line, cur_line++);
  }

  free(line);
  fclose(file);

  return items;
}
