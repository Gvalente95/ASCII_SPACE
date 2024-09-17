#include "../../headers/1_includes.h"

// CHECKS
int is_in_canv(int x_pos, int y_pos, int width, int height, int canv_x, int canv_y, int canv_w, int canv_h, DIR d) {
  int final_x = x_pos + (d == left || d == up_left || d == down_left ? -1 : d == right || d == up_right || d == down_right ? 1 : 0);
  int final_y = y_pos + (d == up || d == up_left || d == up_right ? -1 : d == down || d == down_right || d == down_left ? 1 : 0);
  return (final_x >= canv_x + 2 && final_x + width <= canv_w + canv_x - 2 && final_y >= canv_y + 2 && final_y + height <= canv_y + canv_h - 2);
}

// WRITE ON CANVAS
int write_midle_msg(char *canv, int has_exterior, const char *format, ...) {
  if (!canv) return 0;
  va_list args;
  va_start(args, format);
  int required_size = vsnprintf(NULL, 0, format, args) + 1;
  va_end(args);
  char *txt = malloc(required_size);
  if (!txt) return 0;
  va_start(args, format);
  vsnprintf(txt, required_size, format, args);
  va_end(args);
  int dist = has_exterior ? 4 : 2;
  int blok_width = get_width(txt) + dist, blok_height = get_height(txt) + dist;
  int canv_width = get_width(canv), canv_height = get_height(canv);
  if (blok_width >= canv_width) {
    fit_text_to_width(txt, canv_width - 10);
    blok_width = get_width(txt), blok_height = get_height(txt);
  }
  char *blok = init_blank_canv(blok_width, blok_height, has_exterior ? 2 : 0, '#');
  write_on_canv(txt, blok, dist / 2, dist / 2), write_on_canv(blok, canv, canv_width / 2 - blok_width / 2, canv_height / 2 - blok_height / 2);
  free(blok), free(txt);
  return 1;
}
int write_dialog_msg(char *canv, char *speaker_gfx, const char *format, ...) {
  if (!canv) return 0;
  va_list args;
  va_start(args, format);
  int required_size = vsnprintf(NULL, 0, format, args) + 1;
  va_end(args);
  char *txt = malloc(required_size);
  if (!txt) return 0;
  va_start(args, format);
  vsnprintf(txt, required_size, format, args);
  va_end(args);
  int dist = 4;
  int canv_width = get_width(canv), canv_height = get_height(canv);
  int blok_width = canv_width, blok_height = get_height(txt) + dist;
  if (blok_width >= canv_width) {
    fit_text_to_width(txt, canv_width - 10);
    blok_width = get_width(txt), blok_height = get_height(txt);
  }
  char *blok = init_blank_canv(blok_width, blok_height, 2, '#');
  write_on_canv(txt, blok, dist / 2, dist / 2);
  write_on_canv(blok, canv, 0, canv_height - blok_height);
  free(blok), free(txt);
  return 1;
}
__attribute__((no_sanitize("address"))) char *write_on_canv(char *src, char *dst, int x_pos, int y_pos) {
  int i = 0;
  int u = 0;
  int width = get_width(src);
  int height = get_height(src);

  int dst_width = get_width(dst);
  int dst_height = get_height(dst);

  if (x_pos + width + 1 <= 1 || y_pos + height + 1 <= 1) return NULL;

  if (x_pos >= dst_width - 1 || x_pos + width < 0 || y_pos > dst_height - 1 || y_pos + height < 0) return NULL;

  char *mod_src = strdup(src);

  if (x_pos < 0) crop_x(mod_src, abs(x_pos));
  if (x_pos + width > dst_width - 1) { mod_src = set_new_width(mod_src, dst_width - 1 - x_pos); }
  while (y_pos > 0)
    if (dst[i++] == '\n') y_pos--;
  while (y_pos < 0)
    if (src[u++] == '\n') y_pos++;
  i += x_pos;

  while (mod_src[u]) {
    if (mod_src[u] && mod_src[u] == ' ') {
      i++;
      u++;
      continue;
    }
    if (mod_src[u] && mod_src[u] == '\n') {
      while (dst[i] && dst[i] != '\n')
        i++;
      if (!mod_src[u] || !dst[i]) break;
      u++;
      i += x_pos + 1;
    }
    if (!mod_src[u] || !dst[i])
      break;
    else if (mod_src[u] != ' ')
      dst[i] = mod_src[u];
    i++;
    u++;
  }

  return mod_src;
}

// MOD TXT
void fit_text_to_width(char *txt, int max_width) {
  int i = 0, x = 0;

  while (txt[i]) {
    if (txt[i] == '\n') txt[i] = ' ';
    i++;
  }
  i = 0;
  while (txt[i]) {
    if (x == max_width) {
      int u = i;
      while (u >= 0 && txt[u] != ' ')
        u--;
      // if (u <= 0) break;
      txt[u] = '\n';
      x = 0;
    }
    i++;
    x++;
  }
}
char *swap_line(char *txt, int i, DIR d) {
  if (!txt) return NULL;

  size_t length = strlen(txt);
  if (i >= length) return NULL;

  int start = i;
  int step = (d == left) ? 1 : (d == right) ? -1 : 0;

  while (start >= 0 && start < length) {
    int nxt_index = start + step;
    if (nxt_index >= 0 && nxt_index < length) {
      char tmp = txt[nxt_index];
      txt[nxt_index] = txt[start];
      txt[start] = tmp;
    }
    start = nxt_index;
  }
  if (start >= 0 && start < length) { txt[start] = ' '; }
  txt[0] = txt[1];

  return txt;
}
char *set_new_width(char *src, int new_width) {
  int i = 0, u = 0, cur_x = 0, size = strlen(src);
  char *new_txt = malloc(size + 1);
  if (!new_txt) return NULL;
  while (i < size) {
    if (cur_x > new_width) {
      new_txt[i++] = '\n';
      cur_x = 0;
    } else {
      if (i + u < strlen(src)) {
        new_txt[i] = src[i + u];
        if (src[i + u] == '\n')
          cur_x = 0;
        else
          cur_x++;
      }
      i++;
    }
    if (i + u < strlen(src)) {
      if (cur_x == new_width && src[i + u] != '\n') {
        while (src[i + u] && src[i + u] != '\n') {
          u++;
        }
      }
    }
  }
  new_txt[i] = '\0';
  return new_txt;
}
int crop_x(char *src, int from_x) {
  int i = 0;
  if (src == NULL) return 0;
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
void set_label(char *canv, int *start_index, const char *label, int value) {
  int canv_len = strlen(canv);
  if (!canv || (*start_index < 0 || (*start_index) > canv_len)) return;
  if (label == NULL && value == 0) {
    canv[*start_index] = '0';
    (*start_index)++;
    return;
  }
  while (*label) {
    if (*label == '#') {
      label++;
      (*start_index)++;
    }
    if (*label == '\n') {
      while (canv[(*start_index)] != '\n')
        (*start_index)++;
      (*start_index)++, label++;
    }
    canv[(*start_index)++] = *label++;
  }
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
  *start_index += 4;
}
int set_text_red(char *src) {
  if (!src) return (0);
  int i = 0;
  while (src[i]) {
    if (src[i] != ' ' && src[i] != '#' && src[i] != SHLD_OFF_MOB_IC && src[i] != SHLD_ON_MOB_IC && src[i] != '\n') src[i] = RED_ICON;
    i++;
  }
  return (1);
}
void render_text_in_color(char *c, char __, char *color) {
  if (color != NULL) {
    if (c != NULL) { printf("%s%s\033[0m", color, c); }
    if (__ != '\0') { printf("%s%c\033[0m", color, __); }
  } else {
    if (c != NULL) { printf("%s", c); }
    if (__ != '\0') { printf("%c", __); }
  }
}

// GET TEXT
char *get_text_bubble(char *txt, char *label) {
  int tx_len = strlen(txt);
  int tx_w = get_width(txt), tx_h = get_height(txt);
  if (tx_w > CANV_W - 20) fit_text_to_width(txt, CANV_W - 20);
  char *bub_BG = init_blank_canv(tx_w + 5, tx_h + 4 + (label != NULL ? 3 : 0), 0, ' ');
  char *bub = init_blank_canv(tx_w + 4, tx_h + 2, 1, '#');
  write_on_canv("###", bub, (tx_w + 4) / 2 + 2, tx_h + 2);
  write_on_canv("\\/", bub_BG, (tx_w + 4) / 2 + 3, tx_h + 3);
  write_on_canv(bub, bub_BG, 0, label != NULL ? 2 : 0);
  write_on_canv(txt, bub_BG, 2, 1 + label != NULL ? 3 : 0);
  free(bub);
  if (label != NULL) {
    char *labelBG = init_blank_canv(strlen(label) + 2, 3, 1, '#');
    write_on_canv(label, labelBG, 1, 1);
    write_on_canv(labelBG, bub_BG, 3, 0);
    free(labelBG);
  }
  return (bub_BG);
}
char *get_quest_description(QUEST q) {
  char *lv_req_txt = malloc(30);
  snprintf(lv_req_txt, 29, "%d", q.level_req);
  char *label = malloc(50);
  char *rar_lab = strdup(rarity_types_lb[q.rar]);
  Color_from_index((int)q.rar, &rar_lab);
  snprintf(label, 300, "QUEST PROPOSED BY \'%s\'  | Lvl: %s  | Rarity : %s", q.name, lv_req_txt, rar_lab);

  char *txt = malloc(500);
  snprintf(txt, 300, "[%s] %s\n \n%s\n \n - reward: %d$\n - xp gain: %d", quest_types_lb[q.q_type], q.name, q.desc, q.currency_am_reward, q.xp_reward_am);

  int end_width = get_width(txt) + 4;
  int end_height = get_height(txt) + 6;

  char *blok = init_blank_canv(end_width, end_height, 2, '#');
  char *underline = init_blank_canv(end_width - 2, 1, 1, '_');

  write_on_canv(label, blok, 2, 1);
  write_on_canv(underline, blok, 1, 2);
  write_on_canv(txt, blok, 2, 4);

  free(txt), free(rar_lab), free(label), free(underline);
  return (blok);
}
char *get_triangle(char interior, DIR dir, int h, int w, int color) {
  (void)w;
  if (dir == left || dir == right) {
    char *res;
    res = strdup(dir == left ? color == 1 ? MBSHP_L : SHP_L : color == 1 ? MBSHP_R : SHP_R);
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
  if (dir == up || dir == down) {
    char *res;
    res = strdup(dir == up ? color == 1 ? MBSHP_U : SHP_U : color == 1 ? MBSHP_D : SHP_D);
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

  char l_bord = color == 1 ? MOB_LBORD : color == 2 ? SHP_LBORD : '/';
  char r_bord = color == 1 ? MOB_RBORD : color == 2 ? SHP_RBORD : '\\';
  char top = color == 1 ? MOB_HOR : color == 2 ? SHP_HOR : '_';

  if (new_try == NULL) { return NULL; }

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
            new_try[i++] = (row == h - 1) ? WIND_VER : interior;
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
          } else if (col > start && col < end - 1)
            new_try[i++] = (row == h - 1) ? top : interior;
          else if (col == end - 1) { new_try[i++] = l_bord; }
        }
        new_try[i++] = '\n';
      }
      break;
  }

  new_try[i] = '\0';
  return new_try;
}
char *get_circle(int width, int height, char interior, float thickness) {
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
char *get_formatted_text(const char *format, ...) {
  va_list args;
  va_start(args, format);
  int needed_len = vsnprintf(NULL, 0, format, args) + 1;
  va_end(args);
  va_start(args, format);
  char *formatted_text = malloc(needed_len);
  if (formatted_text == NULL) {
    va_end(args);
    return NULL;
  }
  vsnprintf(formatted_text, needed_len, format, args);
  va_end(args);
  return formatted_text;
}