#include "../headers/includes.h"

void render_game(GAME game, BOSS boss, SHIP *plr, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *wind, char *canv) {
  (void)boss, (void)proj, (void)mobprj, (void)wind;
  // Render player ship
  char *plr_txt = get_triangle('*', game.mv_type == watch_up ? up : plr->lk_dir, SHIP_W, SHIP_H, 2);
  if (plr->shield) {
    char *circ = get_circle(SHIP_W + 6, SHIP_W + 6, plr->shield <= 2 ? PLR_SHLD_OUTL : plr->shield <= 4 ? PLR_SHLD_OUTL1 : PLR_SHLD_OUTL2, (float)plr->shield / 6);
    write_on_canv(circ, canv, plr->x_pos - 3, plr->y_pos - 4);
    free(circ);
  }
  if (plr->hurt_timer > 0) {
    if (plr->hurt_timer % 3 == 0) write_on_canv(plr_txt, canv, plr->x_pos, plr->y_pos);
    plr->hurt_timer--;
  } else
    write_on_canv(plr_txt, canv, plr->x_pos, plr->y_pos);
  free(plr_txt);

  // Render mobs
  if (!game.is_in_boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      SHIP mob = mobs[i];
      char *mob_tri;
      if (i <= MOBS1_BUFFER)
        mob_tri = get_triangle('*', mob.dir, SHIP_W, SHIP_H, 1);
      else
        mob_tri = mob.dir == left ? strdup(MB2_L) : mob.dir == right ? strdup(MB2_R) : mob.dir == up ? strdup(MB2_U) : strdup(MB2_D);
      if (mob.hurt_timer > 0) set_text_red(mob_tri);
      if (mob.hurt_timer > 0) {
        int i = 0;
        while (mob_tri[i]) {
          if (mob_tri[i] != ' ' && mob_tri[i] != '\n') mob_tri[i] = EXPL_START;
          i++;
        }
      }
      if (mob.x_pos - SHIP_W > 0 && mob.x_pos < CANV_W && mob.y_pos - SHIP_H < CANV_H && mob.y_pos < CANV_H) write_on_canv(mob_tri, canv, mob.x_pos, mob.y_pos);
      if (mob_tri != NULL) free(mob_tri);
    }
  }
  // Render projectiles and wind effects are handled in upd_game_state()
}

int render_canvas(char *canv, GAME *game, int time) {
  (void)time;
  system("clear");
  char *mob_color1 = game->level < 5 ? BLUE : game->level < 10 ? GREEN : game->level < 15 ? YELLOW : game->level < 20 ? ORANGE : game->level < 25 ? RED : MAGENTA;
  char *mob_color2 = game->level < 5 ? LIGHT_BLUE : game->level < 10 ? DARK_GREEN : game->level < 15 ? PALE_YELLOW : game->level < 20 ? RED : game->level < 25 ? BROWN : CYAN;
  char *plr_color1 = BRIGHT_RED, *plr_color2 = CYAN;
  int i = 0;
  while (canv[i]) {
    char c = canv[i];
    if (c == '#')
      printf(" ");
    else if (c == COL_HOR || c == COL_VER)
      render_text_in_color(NULL, canv[i] == COL_HOR ? '_' : '|', rand() % 4 == 0 ? MAGENTA : rand() % 4 == 0 ? BLUE : GREEN);

    // BASE PROJ
    else if (c == PRJ_BS_HOR || c == PRJ_BS_VER || c == PRJ_BS_DIAG_L || c == PRJ_BS_DIAG_R)
      render_text_in_color(NULL, c == PRJ_BS_HOR ? '_' : c == PRJ_BS_VER ? '|' : c == PRJ_BS_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? RED : ORANGE);
    // MAGN PROJ
    else if (c == PRJ_MAGN_HOR || c == PRJ_MAGN_VER || c == PRJ_MAGN_DIAG_L || c == PRJ_MAGN_DIAG_R)
      render_text_in_color(NULL, 'o', rand() % 2 == 0 ? MAGENTA : YELLOW);
    // BOMB PROJ
    else if (c == PRJ_BMB_HOR || c == PRJ_BMB_VER || c == PRJ_BMB_DIAG_L || c == PRJ_BMB_DIAG_R) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? 'O' : r == 1 ? '#' : r == 2 ? '*' : '$', rand() % 2 == 0 ? YELLOW : ORANGE);
    }
    // BIG PROJ
    else if (c == PRJ_BIG_HOR || c == PRJ_BIG_VER || c == PRJ_BIG_DIAG_L || c == PRJ_BIG_DIAG_R)
      render_text_in_color(NULL, rand_range(33, 76), rand() % 2 == 0 ? GREEN : LIGHT_BLUE);

    // BASE MOB PROJ
    else if (c == MPRJ_BS_HOR || c == MPRJ_BS_VER || c == MPRJ_BS_DIAG_L || c == MPRJ_BS_DIAG_R)
      render_text_in_color(NULL, c == MPRJ_BS_HOR ? '_' : c == MPRJ_BS_VER ? game->is_in_boss_fight ? 'O' : '|' : c == MPRJ_BS_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (c == WIND_DOT || c == WIND_VER || c == WIND_HOR)
      render_text_in_color(NULL, c == WIND_DOT ? '.' : c == WIND_VER ? '|' : '-', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == CANV_VER || c == CANV_HOR)
      printf(c == CANV_VER ? "|" : "_");
    else if (c == PLR_SHLD_OUTL)
      render_text_in_color(NULL, '.', rand() % 2 == 0 ? BLUE : LIGHT_BLUE);
    else if (c == STAR_IC || c == STAR_IC2)
      render_text_in_color(NULL, c == STAR_IC ? '+' : '.', rand() % 2 == 0 ? YELLOW : PALE_YELLOW);
    else if (c == PLR_SHLD_OUTL1)
      render_text_in_color(NULL, '.', rand() % 2 == 0 ? GREEN : DARK_GREEN);
    else if (c == PLR_SHLD_OUTL2)
      render_text_in_color(NULL, '.', rand() % 2 == 0 ? RED : ORANGE);
    else if (c == SHLD_OFF_MOB_IC || c == SHLD_ON_MOB_IC)
      render_text_in_color(NULL, c == SHLD_OFF_MOB_IC ? ' ' : '.', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == MOB_LBORD || c == MOB_RBORD || c == MOB_HOR || c == MOB_VER)
      render_text_in_color(NULL, c == MOB_LBORD ? '/' : c == MOB_RBORD ? '\\' : c == MOB_HOR ? '_' : '|', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (c == SHP_LBORD || c == SHP_RBORD || c == SHP_HOR || c == SHP_VER)
      render_text_in_color(NULL, c == SHP_LBORD ? '/' : c == SHP_RBORD ? '\\' : c == SHP_HOR ? '_' : '|', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == HEART_IC)
      render_text_in_color(HEART_SMB, '\0', RED);
    else if (c == COIN_IC)
      render_text_in_color(SCR_SMB, '\0', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == LUCK_IC)
      render_text_in_color(LUCK_SMB, '\0', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == WEAP_IC)
      render_text_in_color(PROJ_SMB, '\0', rand() % 2 == 0 ? BRIGHT_RED : YELLOW);
    else if (c == SHIELD_IC)
      render_text_in_color(SHLD_SMB, '\0', rand() % 2 == 0 ? GREEN : DARK_GREEN);
    else if (c == ATTR_IC)
      render_text_in_color(ATTR_SYMB, '\0', CYAN);
    else if (canv[i] == EXPL_START || canv[i + 1] == EXPL_START || canv[i - 1] == EXPL_START || canv[i + CANV_W + 1] == EXPL_START || canv[i - CANV_W - 1] == EXPL_START) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? '.' : r == 1 ? '/' : r == 2 ? '\\' : '*', rand() % 2 == 0 ? mob_color1 : mob_color2);
    } else if (c == PRJ_COL) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? '.' : r == 1 ? '\'' : r == 2 ? ',' : '*', rand() % 2 == 0 ? RED : ORANGE);
    } else if (c == RED_ICON)
      render_text_in_color(NULL, '#', RED);

    else
      printf("%c", c);
    i++;
  }
  return 1;
}

int set_lotterie_menu(char *header, char *canv, GAME *game, SHIP *plr) {
  (void)plr;
  char *upg_canv = init_blank_canv(UPG_CANV_W, UPG_CANV_H, 1, '#');
  char *msg = strdup(LV_MSG);
  write_on_canv(header, canv, 0, 0);
  write_on_canv(msg, upg_canv, (UPG_CANV_W - get_width(msg)) / 2, 3);
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
    write_on_canv(el, choice_txt, (cell_x - get_width(el)) / 2, (cell_y - get_height(el)) / 2);
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

    set_over_canv(choice_txt, 3, 1, "hello!\n how is\n it going");

    write_on_canv(choice_txt, upg_canv, x_pos, y_pos);
    free(choice_txt);
  }
  write_on_canv(upg_canv, canv, (CANV_W - UPG_CANV_W) / 2, (CANV_H - UPG_CANV_H) / 2);
  free(upg_canv);
  int inp = -1;
  int is_paused = 1;
  while (is_paused) {
    usleep(7000);
    inp = getchar();
    render_canvas(canv, game, 0);
    if (inp == 10) is_paused = 0;
  }
  return (inp);
}

char *set_new_width(char *src, int new_width) {
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

int write_on_canv(char *src, char *dst, int x_pos, int y_pos) {
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

  if (x_pos + width > dst_width - 1) src = set_new_width(src, dst_width - 1 - x_pos);

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
          else if (col == end - 1) {
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

int set_over_canv(char *canv, int left_padding, int top_padding, const char *format, ...) {
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

void set_label(char *canv, int *start_index, const char *label, int value) {
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

int is_in_canv(int x_pos, int y_pos, int width, int height, DIR d) {
  int final_x = x_pos + (d == left || d == up_left || d == down_left ? -1 : d == right || d == up_right || d == down_right ? 1 : 0);
  int final_y = y_pos + (d == up || d == up_left || d == up_right ? -1 : d == down || d == down_right || d == down_left ? 1 : 0);
  return (final_x >= 2 && final_x + width <= CANV_W - 2 && final_y >= 2 && final_y + height <= CANV_H - 2);
}
