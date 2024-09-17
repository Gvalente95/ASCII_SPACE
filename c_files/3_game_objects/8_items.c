#include "../../headers/1_includes.h"

int is_same_item(ITEM a, ITEM b) { return (a.content == b.content && a.name == b.name ? 1 : 0); }

char *get_ITEM_information(ITEM *t) {
  if (!t || t->content == '\0') return NULL;
  char *txt = malloc(200);
  snprintf(txt, 199, " %-20s | CNT %c | TYP %-10s | X%-5dY%-5d | RAR %d | PRC %-5d$ | DUR %-2d | AM %-2d", t->name, t->content, itm_types_lb[t->type], t->x_pos, t->y_pos, t->rar,
           t->price, t->dur, t->am);
  fit_text_to_width(txt, CANV_W - 10);
  return (txt);
}

ITEM **filter_items_by_type(ITEM *base, int type_sel, int max_items, int *filtered_count) {
  if (type_sel >= ITEM_Type_COUNT || base == NULL) return (NULL);
  ITEM **itms = (ITEM **)malloc(max_items * sizeof(ITEM *));
  if (!itms) return NULL;
  int bs_index = 0, itm_i = 0;
  while (itm_i < max_items && base[bs_index].content) {
    if (base[bs_index].type == type_sel && base[bs_index].am >= 1) itms[itm_i++] = &base[bs_index];
    bs_index++;
  }
  if (filtered_count) *filtered_count = itm_i;
  return itms;
}

ITEM get_item(RARITY rar, ITEM_Type type, ITEM *items, int buffer_size) {
  if (rar >= RARITY_COUNT) rar = RARITY_COUNT - 1;
  int tryes;
  ITEM itm;
  do {
    (Copy_Item(&itm, items[rand_range(0, buffer_size - 1)]));
    tryes++;
  } while (itm.rar != rar && tryes <= 400);

  if (tryes >= 400) reset_item(&itm);
  return itm;
}

void reset_item(ITEM *itm) {
  itm->x_pos = itm->y_pos = itm->center_x = itm->center_y = -1;
  itm->price = itm->dur = 0;
  itm->am = 1;
  itm->type = ITEM_Type_COUNT;
  for (int x = 0; x < Effect_COUNT; x++)
    itm->effect[x].name[0] = '\0';
  for (int x = 0; x < Effect_COUNT; x++)
    itm->bonus[x].name[0] = '\0';
  itm->rar = RARITY_COUNT;
  itm->content = '\0';
  itm->desc[0] = '\0';
  itm->name[0] = '\0';
}

ITEM Get_Item_Of_type(ITEM_Type Type, ITEM *list, int l_size) {
  ITEM to_return;
  to_return.content = '\0';
  if (!list || l_size <= 0) return (to_return);
  ITEM itm_bf[l_size - 1];
  int i = 0, itm_found = 0;
  for (int x = 0; x < l_size; x++) {
    if (list[x].type == Type) {
      Copy_Item(&itm_bf[i++], list[x]);
      itm_found = 1;
    }
  }
  if (!itm_found) return (to_return);
  return (itm_bf[rand_range(0, i - 1)]);
}

int determineRarity(float luck) {
  float baseProbabilities[RARITY_COUNT] = {0.6, 0.25, 0.08, 0.04, 0.015, 0.01, 0.005};
  float probabilities[RARITY_COUNT];
  float scalingFactor = pow(3, luck * 5);

  for (int i = 0; i < RARITY_COUNT; i++) {
    probabilities[i] = baseProbabilities[i] * pow(scalingFactor, i);
  }
  float sum = 0.0;
  for (int i = 0; i < RARITY_COUNT; i++)
    sum += probabilities[i];
  for (int i = 0; i < RARITY_COUNT; i++)
    probabilities[i] /= sum;
  float randomValue = (float)rand() / RAND_MAX;
  float cumulative = 0.0;
  for (int i = 0; i < RARITY_COUNT; i++) {
    cumulative += probabilities[i];
    if (randomValue <= cumulative) return i;
  }
  return 0;
}

void Copy_Item(ITEM *to, ITEM from) {
  strncpy(to->name, from.name, sizeof(to->name) - 1);
  to->name[sizeof(to->name) - 1] = '\0';
  strncpy(to->desc, from.desc, sizeof(to->desc) - 1);
  to->desc[sizeof(to->desc) - 1] = '\0';
  to->content = from.content;
  to->dur = from.dur;
  to->price = from.price, to->rar = from.rar, to->type = from.type;
  to->am = from.am;
}

int Get_itm_duplicate_index(ITEM a, ITEM *items, int size) {
  if (size <= 0) return (-1);
  for (int i = 0; i < size; i++) {
    if (is_same_item(a, items[i])) return (i);
  }
  return (-1);
}

void Generate_items(ITEM *list, NPC *n, int amount, int list_size) {
  n->items = malloc(sizeof(ITEM) * amount);
  if (!n->items) return;

  for (int i = 0; i < amount; i++) {
    Copy_Item(&n->items[i], list[rand_range(0, list_size - 1)]);
    n->items[i].am = 1;
    if (i == 0) continue;
    ITEM new_item;
    do
      Copy_Item(&new_item, list[rand_range(0, list_size - 1)]);
    while (new_item.type == QUEST_ITEM);
    int is_dupl = Get_itm_duplicate_index(new_item, n->items, i);
    if (is_dupl != -1) {
      n->items[is_dupl].am++;
    } else {
      new_item.am = 1;
      Copy_Item(&n->items[i], new_item);
    }
  }
}
void reorder_list(ITEM *list, int max_size) {
  int next_fill = 0;
  for (int i = 0; i < max_size; i++) {
    if (list[i].content != '\0') {
      if (i != next_fill) {
        Copy_Item(&list[next_fill], list[i]);
        reset_item(&list[i]);
      }
      next_fill++;
    }
  }
  for (int j = next_fill; j < max_size; j++) {
    reset_item(&list[j]);
  }
}
