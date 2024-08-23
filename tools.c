#include "header.h"

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

char *draw_triangle(char interior, DIR dir, int h, int w, int color) {

  if (dir == left || dir == right) {
    char *res;
    res = strdup(dir == left ? SHP_R : SHP_L);
    if (color) {
      int i = 0;
      while (res[i]) {
        if (res[i] == '|')
          res[i] = color == 1 ? MOB_VER : SHP_VER;
        else if (res[i] == '_')
          res[i] = color == 1 ? MOB_HOR : SHP_HOR;
        else if (res[i] == '/')
          res[i] = color == 1 ? MOB_LBORD : SHP_LBORD;
        else if (res[i] == '\\')
          res[i] = color == 1 ? MOB_RBORD : SHP_RBORD;
        i++;
      }
    }
    return (res);
  }

  int i = 0;
  int max_width = h * 2;
  int size = h * (max_width + 1) + 1;
  char *new_try = malloc(size);
  int incr = 0;

  char l_bord = color == 1 ? MOB_LBORD : color == 2 ? SHP_LBORD : '/';
  char r_bord = color == 1 ? MOB_RBORD : color == 2 ? SHP_RBORD : '\\';
  char top = color == 1 ? MOB_HOR : color == 2 ? SHP_HOR : '_';

  if (new_try == NULL) {
    return NULL;
  }

  switch (dir) {
    case up:
    case up_left:
    case up_right:
      for (int row = 0; row < h; row++) {
        int start = h - row - 1;
        int end = h + row + 1;

        for (int col = 0; col < max_width; col++) {
          if (col < start) {
            new_try[i++] = ' '; // Leading spaces
          } else if (col == start) {
            new_try[i++] = l_bord;
          } else if (col > start && col < end - 1) {
            new_try[i++] = (row == h - 1) ? top : interior;
          } else if (col == end - 1) {
            new_try[i++] = r_bord;
          }
        }
        new_try[i++] = '\n';
      }
      break;

    default:
      for (int row = h - 1; row >= 0; row--) {
        int start = h - row - 1;
        int end = h + row + 1;

        for (int col = 0; col < max_width; col++) {
          if (col < start) {
            new_try[i++] = ' '; // Leading spaces
          } else if (col == start) {
            new_try[i++] = r_bord;
          } else if (col > start && col < end - 1) {
            new_try[i++] = (row == h - 1) ? top : interior;
          } else if (col == end - 1) {
            new_try[i++] = l_bord;
          }
        }
        new_try[i++] = '\n';
      }
      break;
  }

  new_try[i] = '\0';
  return new_try;
}

char *init_blank_canv(int w, int h, int has_ext, char interior) {
  char *canv = malloc(w + (w * h) + 1);
  int i = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if ((x == 0 && y == 0) || (x == 0 && y == h - 1) || (x == w - 1 && y == 0) || (x == w - 1 && y == h - 1))
        canv[i++] = ' ';
      else if (x == 0 || x == w - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 1 ? CANV_VER : COL_VER;
      else if (y == 0 || y == h - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 1 ? CANV_HOR : COL_HOR;
      else
        canv[i++] = interior;
    }
    canv[i++] = '\n';
  }
  canv[i] = '\0';
  return (canv);
}

int get_width(char *src) {
  int i = 0;
  int cur_x = 0;
  int max_x = cur_x;

  while (src[i]) {
    if (src[i] == '\n') {
      if (max_x < cur_x) max_x = cur_x;
      cur_x = 0;
    }
    cur_x++;
    i++;
  }
  return (max_x);
}

int get_height(char *src) {
  int i = 0;
  int y_size = 0;
  while (src[i]) {
    if (src[i] == '\n') y_size++;
    i++;
  }
  return (y_size);
}

int crop_x(char *src, int from_x) {
  int i = 0;
  while (src[i]) {
    int next_end = 0;
    while (src[i] && src[i] != '\n' && next_end < from_x) {
      src[i++] = ' ';
      next_end++;
    }
    while (src[i] && src[i] != '\n')
      i++;
    if (src[i] == '\n') i++;
  }
  return 0;
}

char *erase_on_text(char *src, int new_width) {
  int i = 0, u = 0, cur_x = 0, size = 0;
  while (src[size])
    size++;
  char *new_txt = malloc(size + 1);
  if (!new_txt) return NULL;
  while (i < size) {
    if (cur_x > new_width) {
      new_txt[i++] = '\n';
      cur_x = 0;
    } else {
      new_txt[i] = src[i + u];
      if (src[i + u] == '\n')
        cur_x = 0;
      else
        cur_x++;
      i++;
    }
    if (cur_x == new_width && src[i + u] != '\n') {
      while (src[i + u] && src[i + u] != '\n') {
        u++;
      }
    }
  }
  new_txt[i] = '\0';
  return new_txt;
}

int add_to_canv(char *src, char *dst, int x_pos, int y_pos) {
  int i = 0;
  int u = 0;
  int width = get_width(src);
  int height = get_height(src);

  int dst_width = get_width(dst);
  int dst_height = get_height(dst);

  if (x_pos >= dst_width || x_pos + width < 0 || y_pos > dst_height || y_pos + height < 0) return (0);

  while (y_pos > 0)
    if (dst[i++] == '\n') y_pos--;

  while (y_pos < 0) {
    if (src[u++] == '\n') y_pos++;
  }

  if (x_pos < 0) crop_x(src, abs(x_pos));

  if (x_pos + width > dst_width - 1) src = erase_on_text(src, dst_width - 1 - x_pos);

  i += x_pos;

  while (src[u]) {
    if (src[u] && src[u] == '\n') {
      while (dst[i] && dst[i] != '\n')
        i++;
      if (!src[u] || !dst[i]) break;
      u++;
      i += x_pos + 1;
    }
    if (!src[u] || !dst[i])
      break;
    else if (src[u] != ' ')
      dst[i] = src[u];
    i++;
    u++;
  }
  return (1);
}

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }

int is_position_o__upied(SHIP *mobs, int current_mob, int x, int y) {
  for (int i = 0; i < current_mob; i++) {
    if (mobs[i].x_pos == x && mobs[i].y_pos == y) {
      return 1;
    }
  }
  return 0;
}

void print_colored_text(char *c, char __, char *color) {
  if (color != NULL) {
    if (c != NULL) {
      printf("%s%s\033[0m", color, c);
    }
    if (__ != '\0') {
      printf("%s%c\033[0m", color, __);
    }
  } else {
    if (c != NULL) {
      printf("%s", c);
    }
    if (__ != '\0') {
      printf("%c", __);
    }
  }
}

char *draw_circle(int width, int height, char interior, float thickness) {
  int size = (width + 1) * (height + 1) + 1;
  char *dst = (char *)malloc(size);
  int index = 0;
  int centerX = width / 2, centerY = height / 2;

  for (int y = 0; y <= height; y++) {
    for (int x = 0; x <= width; x++) {
      float dx = (float)(x - centerX) / (width / 2);
      float dy = (float)(y - centerY) / (height / 4);
      float distance = (dx * dx) + (dy * dy);

      if (distance >= (1.0f - thickness) && distance <= (1.0f + thickness))
        dst[index++] = interior;
      else
        dst[index++] = ' ';
    }
    dst[index++] = '\n';
  }
  dst[index] = '\0';
  return dst;
}

int contains(Loot_Type *array, int length, Loot_Type value) {
  for (int i = 0; i < length; i++) {
    if (array[i] == value) {
      return 1;
    }
  }
  return 0;
}

int write_on_canv(char *canv, int left_padding, int top_padding, const char *format, ...) {
  va_list args;
  va_start(args, format);

  int max_len = 1024;
  char formatted_text[max_len];

  vsnprintf(formatted_text, max_len, format, args);

  va_end(args);
  int c_width = get_width(canv);
  int c_height = get_height(canv);
  int t_width = get_width(formatted_text);
  int t_height = get_height(formatted_text);

  if (t_height > c_height - top_padding || t_width > c_width - left_padding) return 0;

  int canv_index = 0;
  int text_index = 0;
  canv_index += (c_width + 1) * top_padding;
  canv_index += left_padding;

  while (formatted_text[text_index]) {
    if (formatted_text[text_index] == '\n') {
      text_index++;
      while (canv[canv_index] != '\n')
        canv_index++;
      canv_index += 1 + left_padding;
    }
    canv[canv_index++] = formatted_text[text_index++];
  }
  return 1;
}

int check_overlap(BLDING *a, BLDING *b) {
  return !(a->x_pos + a->x_size <= b->x_pos || // a is to the left of b
           a->x_pos >= b->x_pos + b->x_size || // a is to the right of b
           a->y_pos + a->y_size <= b->y_pos || // a is above b
           a->y_pos >= b->y_pos + b->y_size);  // a is below b
}

void write_label_and_value(char *canv, int *start_index, const char *label, int value) {
  // Write the label first
  while (*label)
    canv[(*start_index)++] = *label++;
  int is_negative = (value < 0);
  if (is_negative) {
    canv[(*start_index)++] = '-';
    value = -value;
  }
  if (value != '\0') {
    int temp_value = value, digit_count = 0;
    do {
      digit_count++;
      temp_value /= 10;
    } while (temp_value > 0);
    *start_index += digit_count;
    int pos = *start_index - 1;
    temp_value = value;
    do {
      canv[pos--] = (temp_value % 10) + '0';
      temp_value /= 10;
    } while (temp_value > 0);
  }
  *start_index += 3;
}

int is_in_canv(int x_pos, int y_pos, int width, int height, DIR d) {
  int final_x = x_pos + (d == left || d == up_left || d == down_left ? -1 : d == right || d == up_right || d == down_right ? 1 : 0);
  int final_y = y_pos + (d == up || d == up_left || d == up_right ? -1 : d == down || d == down_right || d == down_left ? 1 : 0);
  return (final_x >= 2 && final_x + width <= CANV_W - 2 && final_y >= 2 && final_y + height <= CANV_H - 2);
}

int handle_superloot_menu(char *header, char *canv, GAME *game, SHIP *plr) {
  char *upg_canv = init_blank_canv(UPG_CANV_W, UPG_CANV_H, 1, '#');
  char *msg = strdup(LV_MSG);
  add_to_canv(header, canv, 0, 0);
  add_to_canv(msg, upg_canv, (UPG_CANV_W - get_width(msg)) / 2, 3);
  free(msg);
  game->lv_choices = rand_range(1, 7);
  Loot_Type l_choice[game->lv_choices];
  int x_range = -1, y_range = 0;
  for (int i = 0; i < game->lv_choices; i++) {
    x_range++;
    if (x_range > 2) {
      x_range = 0;
      y_range++;
    }
    Loot_Type new_loot;
    do
      new_loot = rand() % LOOT_TYPE_COUNT;
    while (contains(l_choice, i, new_loot));
    l_choice[i] = new_loot;

    int cell_x = 20, cell_y = 10;

    char *choice_txt = init_blank_canv(cell_x, cell_y, 1, '#');
    char *elem = l_choice[i] == MOR_ATTR   ? ATTR_GFX
                 : l_choice[i] == MOR_LOOT ? LUCK_GFX
                 : l_choice[i] == MOR_PRJ  ? GUN_GFX
                 : l_choice[i] == MOR_SHLD ? SHIELD_GFX
                 : l_choice[i] == MOR_HP   ? HEART_GFX
                 : l_choice[i] == BET_PRJ  ? GUN_GFX
                 : l_choice[i] == MOR_SC   ? SCOR_GFX
                                           : HEART_GFX;
    char *el = strdup(elem);
    add_to_canv(el, choice_txt, (cell_x - get_width(el)) / 2, (cell_y - get_height(el)) / 2);
    free(el);
    // Calculate total width o__upied by all cells in the row
    int total_row_width = (cell_x + cell_x / 3) * game->lv_choices - cell_x / 3;
    if (game->lv_choices > 3) total_row_width = (cell_x + cell_x / 3) * 3;
    // Center the starting position for the first cell
    int start_x = (UPG_CANV_W - total_row_width) / 2;
    int x_pos = start_x + (cell_x + cell_x / 3) * x_range;
    // Adjust y_pos calculation to center vertically while considering vertical padding
    int total_height = (cell_y + cell_y / 3) * (y_range + 1);
    int start_y = (UPG_CANV_H - total_height) / 2;
    int y_pos = start_y + (cell_y * 2) * y_range;

    write_on_canv(choice_txt, 3, 1, "hello!\n how is\n it going");

    add_to_canv(choice_txt, upg_canv, x_pos, y_pos);
    free(choice_txt);
  }
  add_to_canv(upg_canv, canv, (CANV_W - UPG_CANV_W) / 2, (CANV_H - UPG_CANV_H) / 2);
  free(upg_canv);
  int inp = -1;
  int is_paused = 1;
  while (is_paused) {
    usleep(7000);
    inp = getchar();
    print_canvas(canv, game);
    if (inp == 10) is_paused = 0;
  }
  return (inp);
}

int is_dir_allowed(DIR dir, DIR allowed_directions) {
  if (allowed_directions == none) return 0;
  if (allowed_directions == all || dir == allowed_directions) return 1;
  if (allowed_directions == vert) return (dir == up || dir == down);
  if (allowed_directions == hor) return (dir == left || dir == right);
  return 0;
}

int print_in_red(char *src) {
  if (!src) return (0);
  int i = 0;
  while (src[i]) {
    if (src[i] != ' ' && src[i] != '#' && src[i] != SHLD_OFF_MOB_IC && src[i] != SHLD_ON_MOB_IC && src[i] != '\n') src[i] = RED_ICON;
    i++;
  }
  return (1);
}