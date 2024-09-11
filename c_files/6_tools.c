#include "../headers/includes.h"

void set_nonblocking_mode(int enable) {
  static struct termios oldt, newt;
  if (enable) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, 0);
  }
}

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }

DIR get_prj_dir(int w_prj, int max_weap, SHIP plr, DIR d) {
  if (w_prj == max_weap) return (plr.lk_dir);
  if (w_prj == max_weap - 1) return (d == up ? down : d == down ? up : d == left ? right : left);
  if (w_prj == max_weap - 2) return (d == up ? left : d == down ? right : d == left ? up : down);
  if (w_prj == max_weap - 3) return (d == up ? right : d == down ? left : d == left ? down : up);
  if (w_prj == max_weap - 4) return (d == up ? up_right : d == down ? up_left : d == left ? down_left : down_right);
  if (w_prj == max_weap - 5) return (d == up ? up_left : d == down ? up_right : d == left ? down_right : down_left);
  if (w_prj == max_weap - 6) return (d == up ? down_left : d == down ? down_right : d == left ? up_left : up_right);
  if (w_prj == max_weap - 7) return (d == up ? down_right : d == down ? down_left : d == left ? up_right : up_left);
  return up;
}

DIR get_dir_to_target(int target_x, int target_y, int x_pos, int y_pos, int has_diag) {
  DIR dir;
  int diff_x = target_x - x_pos + 3;
  int diff_y = target_y - y_pos + 3;
  if (abs(diff_x) > abs(diff_y)) {
    if (diff_y == 0)
      dir = diff_x > 0 ? right : left;
    else
      dir = diff_x > 0 ? has_diag ? (diff_y > 0 ? down_right : up_right) : right : has_diag ? (diff_y > 0 ? down_left : up_left) : left;
  } else {
    if (diff_x == 0)
      dir = diff_y > 0 ? down : up;
    else
      dir = diff_y > 0 ? has_diag ? (diff_x > 0 ? down_right : down_left) : down : has_diag ? (diff_x > 0 ? up_right : up_left) : up;
  }
  return (dir);
}

int contains(ITEM_Type *array, int length, ITEM_Type value) {
  if (array == NULL) return 0;
  for (int i = 0; i < length; i++)
    if (array[i] == value) return 1;
  return 0;
}

int check_overlap(BLDING *a, BLDING *b) {
  return !(a->x_pos + a->x_size <= b->x_pos || // a is to the left of b
           a->x_pos >= b->x_pos + b->x_size || // a is to the right of b
           a->y_pos + a->y_size <= b->y_pos || // a is above b
           a->y_pos >= b->y_pos + b->y_size);  // a is below b
}

int is_dir_allowed(DIR dir, DIR allowed_directions) {
  if (allowed_directions == none) return 0;
  if (allowed_directions == all || dir == allowed_directions) return 1;
  if (allowed_directions == down_left) return (dir == down || dir == left);
  if (allowed_directions == down_right) return (dir == down || dir == right);
  if (allowed_directions == up_left) return (dir == up || dir == left);
  if (allowed_directions == up_right) return (dir == up || dir == right);
  if (allowed_directions == vert) return (dir == up || dir == down);
  if (allowed_directions == hor) return (dir == left || dir == right);
  if (allowed_directions == hor_up) return (dir == left || dir == right || dir == up);
  if (allowed_directions == hor_down) return (dir == left || dir == right || dir == down);
  if (allowed_directions == ver_left) return (dir == up || dir == down || dir == left);
  if (allowed_directions == ver_right) return (dir == up || dir == down || dir == right);
  return 0;
}

int get_width(const char *src) {
  if (src == NULL) return 0;
  int max_x = 0, cur_x = 0;

  while (*src) {
    if (*src == '\n') {
      if (cur_x > max_x) max_x = cur_x;
      cur_x = 0;
    } else
      cur_x++;
    src++;
  }
  if (cur_x > max_x) max_x = cur_x;
  return max_x;
}

int get_height(const char *src) {
  if (src == NULL) return 0;
  int y_size = 0;
  while (*src) {
    if (*src == '\n') y_size++;
    src++;
  }
  return y_size + 1;
}

char get_index_value(char *canv, int c_width, int c_height, int x, int y) {
  if (canv == NULL) return '\0';
  if (c_width == -1) c_width = get_width(canv);
  if (c_height == -1) c_height = get_height(canv);
  if ((x < 0 || x > c_width) || (y < 0 || y > c_height)) return ('\0');
  return (canv[x + ((c_width + 1) * y)]);
}

int get_index(char *canv, int x_pos, int y_pos) {
  int width = get_width(canv), height = get_height(canv);
  if (x_pos > width || y_pos > height) return -1;
  return (x_pos + (y_pos * (width + 1)));
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

int is_in_string(char c, char *str) {
  int index = 0;
  if (str == NULL) return 0;
  while (str[index])
    if (str[index++] == c) return 1;
  return 0;
}

int is_pos_free(SHIP *mobs, int current_mob, int x, int y) {
  for (int i = 0; i < current_mob; i++)
    if (mobs[i].x_pos == x && mobs[i].y_pos == y) return 1;
  return 0;
}

int is_pos_free_BUILDINGS(BLDING *bldings, int current_mob, int x, int y) {
  for (int i = 0; i < current_mob; i++)
    if ((bldings[i].x_pos >= x - 50 && bldings[i].x_pos <= x + 50) && bldings[i].y_pos >= y - 50 && bldings[i].y_pos <= y + 50) return 1;
  return 0;
}

void tile_canvas(char *canv, int width, int tile_size) {
  int i = 0;
  if (canv == NULL) return;
  while (canv[i]) {
    int row = i / width;
    int col = i % width;
    int col_half = col / 2;
    if (row % tile_size == 0 && col_half % tile_size == 0 && (canv[i] == ' ' || canv[i] == '#')) {
      canv[i] = '+';
    } else if (col_half % tile_size == 0 && (canv[i] == ' ' || canv[i] == '#')) {
      canv[i] = '|';
    } else if (row % tile_size == 0 && (canv[i] == ' ' || canv[i] == '#')) {
      canv[i] = '_';
    }
    i++;
  }
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
int is_same_item(ITEM a, ITEM b) { return (a.content == b.content && a.name == b.name ? 1 : 0); }
int Get_duplicate_index(ITEM a, ITEM *items, int size) {
  if (size == 0) return (0);
  for (int i = 0; i < size; i++) {
    if (is_same_item(a, items[i])) return (i);
  }
  return (-1);
}

void Generate_items(ITEM *list, NPC *n, int amount, int list_size) {
  n->items = malloc(sizeof(ITEM) * amount);
  if (!n->items) return;

  for (int i = 0; i < amount; i++) {
    ITEM new_item;
    Copy_Item(&new_item, list[rand_range(0, list_size - 1)]);
    int is_dupl = (i > 0) ? Get_duplicate_index(new_item, n->items, i) : -1;
    if (is_dupl != -1) {
      n->items[is_dupl].am++;
    } else {
      new_item.am = 1;
      Copy_Item(&n->items[i], new_item);
    }
  }
}

void reorder_list(ITEM *list, int max_size) {
  int next_fill = 0; // Index to place the next non-empty item

  for (int i = 0; i < max_size; i++) {
    if (list[i].content == '\0') {
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

void Color_from_index(COLOR col, char **text) {
  if (!*text) return;
  char color_char = col == red ? red_st : col == blue ? blu_st : col == green ? grn_st : col == yellow ? yel_st : col == orange ? orng_st : col == purple ? prpl_st : red_st;
  int original_size = strlen(*text);
  int new_size = original_size * 2 + 1;
  char *new_text = malloc(new_size);
  if (!new_text) return;

  int new_i = 0, prv_i = 0;

  while (prv_i < original_size) {
    char c = (*text)[prv_i];
    if (c == '\n' || c == ' ') {
      new_text[new_i++] = c;
      prv_i++;
    } else
      new_text[new_i++] = color_char, new_text[new_i++] = (*text)[prv_i++];
  }

  new_text[new_i] = '\0';
  free(*text);
  *text = new_text;
}

int color_text(COLOR col, char *text, int fill_with_dots) {
  int i = 0;
  while (text[i]) {
    if (text[i] == ' ' && fill_with_dots)
      text[i] = col == red ? red_dot : col == blue ? blu_dot : col == green ? grn_dot : yel_dot;
    else {
      switch (text[i]) {
        case '(':
          text[i] = col == red ? red_L_parenth : col == blue ? blu_L_parenth : col == green ? grn_L_parenth : yel_L_parenth;
          break;
        case ')':
          text[i] = col == red ? red_R_parenth : col == blue ? blu_R_parenth : col == green ? grn_R_parenth : yel_R_parenth;
          break;
        case '|':
          text[i] = col == red ? red_ver_bar : col == blue ? blu_ver_bar : col == green ? grn_ver_bar : yel_ver_bar;
          break;
        case '_':
        case '-':
          text[i] = col == red ? red_hor_bar : col == blue ? blu_hor_bar : col == green ? grn_hor_bar : yel_hor_bar;
          break;
        case '.':
        case ',':
        case '\'':
          text[i] = col == red ? red_dot : col == blue ? blu_dot : col == green ? grn_dot : yel_dot;
          break;
        case 'v':
          text[i] = col == red ? red_dwn_arw : col == blue ? blu_dwn_arw : col == green ? grn_dwn_arw : yel_dwn_arw;
          break;
        case '^':
          text[i] = col == red ? red_up_arw : col == blue ? blu_up_arw : col == green ? grn_up_arw : yel_up_arw;
          break;
        case '<':
          text[i] = col == red ? red_lft_arw : col == blue ? blu_lft_arw : col == green ? grn_lft_arw : yel_lft_arw;
          break;
        case '>':
          text[i] = col == red ? red_rgt_arw : col == blue ? blu_rgt_arw : col == green ? grn_rgt_arw : yel_rgt_arw;
          break;
        case '/':
          text[i] = col == red ? red_R_diag_r : grn_R_diag_r;
          break;
        case '\\':
          text[i] = col == red ? red_L_diag_l : grn_L_diag_l;
          break;
        default:
          break;
      }
    }

    i++;
  }
  return 1;
}