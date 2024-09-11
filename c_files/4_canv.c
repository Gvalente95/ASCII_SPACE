#include "../headers/includes.h"

void render_mobs(char *canv, GAME *game, SHIP *mobs, int time) {
  if (game->g_st != boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      SHIP *mob = &mobs[i];
      if (mob->x_pos - SHIP_W > 0 && mob->x_pos < CANV_W && mob->y_pos - SHIP_H < CANV_H && mob->y_pos < CANV_H && canv[i] != 'N' && canv[i] != '#') {
        char *mob_tri;
        if (i <= MOBS1_BUFFER)
          mob_tri = get_triangle('*', mob->dir, SHIP_W, SHIP_H, 1);
        else if (i <= MOBS_BUFFER - 50)
          mob_tri = mob->dir == up ? strdup(MB2_U) : mob->dir == down ? strdup(MB2_D) : mob->dir == left || mob->dir == up_left || mob->dir == down_left ? strdup(MB2_L) : strdup(MB2_R);
        else if (i <= MOBS_BUFFER - 20)
          mob_tri = strdup(SHIELD_GFX);
        else
          mob_tri = strdup(BOSS1);
        if (mob->death_timer > 0) set_text_red(mob_tri);
        if (mob->death_timer > 0) {
          int i = 0;
          while (mob_tri[i]) {
            if (mob_tri[i] != ' ' && mob_tri[i] != '\n') mob_tri[i] = EXPL_START;
            i++;
          }
        }
        int st_index = get_index(canv, mob->x_pos, mob->y_pos - 5);
        if (st_index > -1 && canv[st_index] != '\n') {
          char *txt = malloc(15);
          sprintf(txt, "%d", mob->hp);
          if (mob->hurt_timer > 0) color_text(red, txt, 1);
          set_label(canv, &st_index, txt, '\0');
        }
        if (mob->hurt_timer > 0) {
          mob->hurt_timer--;
          color_text(red, mob_tri, 0);
        }
        write_on_canv(mob_tri, canv, mob->x_pos, mob->y_pos);
        if (mob_tri != NULL) free(mob_tri);
      }
    }
  }
}

void render_player(char *canv, GAME *game, SHIP *plr, int time) {
  if (game->cur_bld_index == -1) {
    char *plr_txt = get_triangle('*', game->mv_type == watch_up ? up : plr->lk_dir, SHIP_W, SHIP_H, 2);
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
  } else {
    DIR d = plr->lk_dir;
    char *curB;
    if (plr->dir != none) {
      curB = strdup(plr->dir == left    ? time % 20 >= 10 ? plr_mv_left1 : plr_mv_left2
                    : plr->dir == right ? time % 20 >= 10 ? plr_mv_right1 : plr_mv_right2
                    : plr->dir == up    ? time % 20 >= 10 ? plr_mv_up1 : plr_mv_up2
                    : time % 20 >= 10   ? plr_mv_down1
                                        : plr_mv_down2);
    } else
      curB = strdup(d == up ? plr_idle_up : d == left ? plr_idle_left : d == right ? plr_idle_right : plr_idle_down1);
    color_text(red, curB, 0);
    write_on_canv(curB, canv, plr->x_pos - 2, plr->y_pos - 4);
    free(curB);
  }
}

char *get_text_bubble(char *txt) {
  int tx_len = strlen(txt);
  int tx_w = get_width(txt), tx_h = get_height(txt);
  char *bub_BG = init_blank_canv(tx_w + 5, tx_h + 4, 0, ' ');
  char *bub = init_blank_canv(tx_w + 4, tx_h + 3, 1, '#');
  write_on_canv("###", bub, (tx_w + 4) / 2 + 2, tx_h + 2);
  write_on_canv("\\/", bub_BG, (tx_w + 4) / 2 + 3, tx_h + 3);
  write_on_canv(bub, bub_BG, 0, 0);
  write_on_canv(txt, bub_BG, 2, 1);
  free(bub);
  return (bub_BG);
}
void render_npcs(GAME *game, BOSS boss, SHIP *plr, SHIP *mobs, BLDING *bldings, PROJ *proj, PROJ *mobprj, int *wind, char *canv, int time, int has_moved, int aftr_plr) {
  char *bub_BG = NULL;
  int x_dial = 0, y_dial = 0, bld_ind = game->cur_bld_index;
  for (int i = 0; i < bldings[bld_ind].npc_am; i++) {
    BLDING *bld = &bldings[bld_ind];
    NPC *n = &bldings[bld_ind].npcs[i];
    int x = n->x_p + bldings[bld_ind].x_pos;
    int y = n->y_p + bldings[bld_ind].y_pos;
    if (n->floor != game->cur_floor) continue;
    if (aftr_plr ? y < plr->y_pos - 3 : y > plr->y_pos - 3) continue;

    if (abs(y - plr->y_pos) < 8 && abs(x - plr->x_pos) < 8) {
      if (game->input == 't' && (game->is_in_dialog != -1 && game->is_in_dialog != i)) {
        game->is_in_dialog = i;
        game->input = '\0';
      }
      if (game->input == ' ' && (game->is_in_dialog == -1) || game->is_in_dialog == i) {
        if (game->is_in_dialog == -1) {
          free(n->text);
          BLD_TYPE tp = bldings[bld_ind].type;
          if (n->type == SELLER) {
            handle_seller_menu(canv, game, n, bld, plr, mobs, &boss, mobprj);
            n->text = strdup(tp == SHOP ? slr_dlgs[rand_range(0, 14)] : forg_dlgs[rand_range(0, 13)]);
            game->cur_seller = n;
          } else
            n->text = strdup(tp == FORTRESS ? adv_dlgs[rand_range(0, 20)] : tp == SHOP || tp == ARMORY ? shpr_dlgs[rand_range(0, 20)] : expl_dlgs[rand_range(0, 20)]);
        }

        bub_BG = get_text_bubble(n->text);
        int tx_w = get_width(n->text), tx_h = get_height(n->text);

        x_dial = x - ((tx_w + 4) / 2);
        y_dial = y - (tx_h + 4);

        game->is_in_dialog = i;
      } else {
        write_on_canv(" ___\n|#!#|\n#---", canv, x + 2, y - 3);
        if (game->is_in_dialog == i) game->is_in_dialog = -1;
      }

      if (game->is_in_dialog == i || has_moved) {
        int x_diff = (x - plr->x_pos) / 2, y_diff = (y + 4) - plr->y_pos;
        if (abs(x_diff) > abs(y_diff)) {
          n->lk_dir = (x_diff < 0) ? right : left;
        } else {
          n->lk_dir = (y_diff < 0) ? down : up;
        }
        if (has_moved)
          n->dir = none;
        else {
          DIR d = n->lk_dir;
          plr->lk_dir = d == down ? up : d == up ? down : d == left ? right : left;
        }
      }
    } else {
      if (game->is_in_dialog == i) game->is_in_dialog = -1;
    }

    if (game->is_in_dialog != i) {
      if (rand() % 200 == 0) n->lk_dir = rand() % 4;
      if (rand() % 500 == 0 && n->type != SELLER) n->dir = n->lk_dir;
    } else
      n->dir = none;

    if (n->dir != none && time % 5 == 0) {
      if (!is_in_canv(x, y, 6, 6, bld->x_pos, bld->y_pos, get_width(bld->floors[0]), get_height(bld->floors[0]), n->dir)) {
        n->y_p += n->dir == up ? 1 : n->dir == down ? -1 : 0;
        n->x_p += n->dir == left ? 1 : n->dir == right ? -1 : 0;
        n->lk_dir = n->dir == up ? down : n->dir == left ? right : n->dir == down ? up : left, n->dir = none;
      } else {
        n->x_p += n->dir == left ? -1 : n->dir == right ? 1 : 0;
        n->y_p += n->dir == up ? -1 : n->dir == down ? 1 : 0;
      }
      if (rand() % 20 == 0) n->dir = none;
    }

    DIR d = n->lk_dir;
    char *curB;
    if (n->dir != none) {
      curB = strdup(n->dir == left    ? time % 20 >= 10 ? plr_mv_left1 : plr_mv_left2
                    : n->dir == right ? time % 20 >= 10 ? plr_mv_right1 : plr_mv_right2
                    : n->dir == up    ? time % 20 >= 10 ? plr_mv_up1 : plr_mv_up2
                    : time % 20 >= 10 ? plr_mv_down1
                                      : plr_mv_down2);
    } else
      curB = strdup(d == up ? n->up_bdy : d == left ? n->lft_bdy : d == right ? n->rgt_bdy : n->dwn_bdy);
    write_on_canv(curB, canv, x, y);
    free(curB);
  }
  if (bub_BG != NULL) {
    write_on_canv(bub_BG, canv, x_dial, y_dial);
    free(bub_BG);
  }
}

void render_building_interior(char *canv, GAME *game, BLDING *bld) {
  char *tmp = strdup(bld->floors[game->cur_floor]);
  write_on_canv(tmp, canv, bld->x_pos, bld->y_pos);
  if (tmp != NULL) free(tmp);
  int i = 0;
  while (canv[i]) {
    if (canv[i] != '\n' && canv[i] == ' ') canv[i] = i % 5 == 0 ? WIND_DOT : ' ';
    i++;
  }
}

void render_buildings_exterior(char *canv, BLDING *bldings) {
  for (int i = 0; i < BLDING_BUFFER; i++) {
    BLDING *bld = &bldings[i];
    if ((bld->x_pos < CANV_W - 1 && bld->x_pos + bld->x_size > 1) && (bld->y_pos < CANV_H - 1 && bld->y_pos + bld->y_size > 1)) write_on_canv(bld->ext_cont, canv, bld->x_pos, bld->y_pos);
  }
}
void render_game(GAME *game, BOSS boss, SHIP *plr, SHIP *mobs, BLDING *bldings, PROJ *proj, PROJ *mobprj, int *wind, char *canv, int time, int has_moved) {
  (void)boss, (void)proj, (void)mobprj, (void)wind;
  int in_blding = game->cur_blding != NONE;

  render_mobs(canv, game, mobs, time);
  if (in_blding) render_building_interior(canv, game, &bldings[game->cur_bld_index]);
  if (in_blding) render_npcs(game, boss, plr, mobs, bldings, proj, mobprj, wind, canv, time, has_moved, 0);
  if (!in_blding) render_buildings_exterior(canv, bldings);
  upd_collisions(canv, game, proj, &boss, mobs, mobprj, plr);
  render_player(canv, game, plr, time);

  if (in_blding) render_npcs(game, boss, plr, mobs, bldings, proj, mobprj, wind, canv, time, has_moved, 1);
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

char *mirror_txt(char *txt, int x_center, DIR d) {
  if (!txt) return NULL;

  int x_size = get_width(txt);
  int y_size = get_height(txt);
  if (x_center <= 0) return NULL;

  for (int y = 0; y < y_size; y++) {
    int cur_y = y * (x_size + 1); // Position of the current line including newline
    int start = cur_y;            // Start of the line
    int end = cur_y + x_size - 1; // End of the line before newline

    if (d == left) {
      end = cur_y + x_center - 1;
    } else if (d == right) {
      start = cur_y + x_center;
    }

    while (start < end) {
      char tmp = txt[start];
      txt[start] = txt[end];
      txt[end] = tmp;
      start++;
      end--;
    }
  }

  return txt;
}

int is_col(char c) { return (c == blu_st || c == grn_st || c == red_st || c == orng_st || c == yel_st || c == prpl_st); }

int render_canvas(char *canv, GAME *game, int time) {
  (void)time;
  system("clear");
  char *mob_color1 = game->level < 5 ? BLUE : game->level < 10 ? GREEN : game->level < 15 ? YELLOW : game->level < 20 ? ORANGE : game->level < 25 ? RED : MAGENTA;
  char *mob_color2 = game->level < 5 ? LIGHT_BLUE : game->level < 10 ? DARK_GREEN : game->level < 15 ? PALE_YELLOW : game->level < 20 ? RED : game->level < 25 ? BROWN : CYAN;
  char *plr_color1 = BRIGHT_RED, *plr_color2 = CYAN;
  int i = 0;
  int size = strlen(canv);

  while (canv[i]) {
    char c = canv[i];
    if (c == '#')
      printf(" ");
    else if (canv[i] == ERASE_IC) {
      printf("  ");
    }

    else if (is_col(c)) {
      int u = i, last_col = i;
      while (canv[u] && canv[u] != '\n') {
        u++;
        if (is_col(canv[u])) last_col = u;
      }
      last_col += 2;
      while (canv[last_col] == ERASE_IC)
        last_col++;
      canv[last_col] = ERASE_IC;
      char *col = strdup(c == blu_st ? BLUE : c == red_st ? RED : c == grn_st ? GREEN : c == orng_st ? ORANGE : c == yel_st ? YELLOW : MAGENTA);
      i++;
      render_text_in_color(NULL, canv[i], col);
      free(col);
    }
    // PROJECTILES
    else if (c == PRJ_BS_HOR || c == PRJ_BS_VER || c == PRJ_BS_DIAG_L || c == PRJ_BS_DIAG_R)
      render_text_in_color(NULL, c == PRJ_BS_HOR ? '_' : c == PRJ_BS_VER ? '|' : c == PRJ_BS_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? RED : ORANGE);
    else if (c == PRJ_RIC_HOR || c == PRJ_RIC_VER || c == PRJ_RIC_DIAG_L || c == PRJ_RIC_DIAG_R)
      render_text_in_color(NULL, c == PRJ_RIC_HOR ? '>' : c == PRJ_RIC_VER ? '^' : c == PRJ_RIC_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? MAGENTA : ORANGE);
    else if (c == PRJ_MAGN_HOR || c == PRJ_MAGN_VER || c == PRJ_MAGN_DIAG_L || c == PRJ_MAGN_DIAG_R)
      render_text_in_color(NULL, 'o', rand() % 2 == 0 ? MAGENTA : YELLOW);
    else if (c == PRJ_BIG_HOR || c == PRJ_BIG_VER || c == PRJ_BIG_DIAG_L || c == PRJ_BIG_DIAG_R)
      render_text_in_color(NULL, rand_range(33, 76), rand() % 2 == 0 ? GREEN : LIGHT_BLUE);
    else if (c == PRJ_BMB_HOR || c == PRJ_BMB_VER || c == PRJ_BMB_DIAG_L || c == PRJ_BMB_DIAG_R) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? 'O' : r == 1 ? '#' : r == 2 ? '*' : '$', rand() % 2 == 0 ? YELLOW : ORANGE);
    }

    // BASE MOB PROJ
    else if (c == MPRJ_BS_HOR || c == MPRJ_BS_VER || c == MPRJ_BS_DIAG_L || c == MPRJ_BS_DIAG_R)
      render_text_in_color(NULL, c == MPRJ_BS_HOR ? '_' : c == MPRJ_BS_VER ? game->g_st == boss_fight ? 'O' : '|' : c == MPRJ_BS_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? mob_color1 : mob_color2);

    // WIND, STARS
    else if (c == WIND_DOT || c == WIND_VER || c == WIND_HOR)
      render_text_in_color(NULL, c == WIND_DOT ? '.' : c == WIND_VER ? '|' : '-', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == STAR_IC || c == STAR_IC2)
      render_text_in_color(NULL, c == STAR_IC ? '+' : '.', rand() % 2 == 0 ? YELLOW : PALE_YELLOW);
    else if (c == WIND_ALL_VER)
      render_text_in_color(NULL, time % 36 >= 15 ? '.' : time % 25 >= 10 ? 'l' : ' ', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == WIND_ALL_HOR)
      render_text_in_color(NULL, time % 24 >= 18 ? '.' : time % 15 >= 12 ? '-' : ' ', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == WIND_ALL_DOT)
      render_text_in_color(NULL, time % 28 >= 80 ? '.' : time % 37 >= 20 ? ',' : ' ', rand() % 2 == 0 ? plr_color1 : plr_color2);

    // PLR SHIELD
    else if (c == PLR_SHLD_OUTL)
      render_text_in_color(NULL, '.', rand() % 2 == 0 ? BLUE : LIGHT_BLUE);
    else if (c == PLR_SHLD_OUTL1)
      render_text_in_color(NULL, '.', rand() % 2 == 0 ? GREEN : DARK_GREEN);
    else if (c == PLR_SHLD_OUTL2)
      render_text_in_color(NULL, '.', rand() % 2 == 0 ? RED : ORANGE);

    // COLLIDERS; BORDERS
    else if (c == CANV_VER || c == CANV_HOR || c == VRT_WALL || c == HOR_WALL)
      printf(c == CANV_VER || c == VRT_WALL ? "|" : "_");
    else if (c == DWNLFT_WALL || c == DWNRGT_WALL || c == UPLFT_WALL || c == UPRGT_WALL)
      printf(".");
    else if (c == UPSTAIR_IC || c == DWNSTAIR_IC)
      printf("_");
    else if (c == COL_HOR || c == COL_VER)
      render_text_in_color(NULL, canv[i] == COL_HOR ? '_' : '|', rand() % 4 == 0 ? MAGENTA : rand() % 4 == 0 ? BLUE : GREEN);

    // MOBS AND SHIP
    else if (c == SHLD_OFF_MOB_IC || c == SHLD_ON_MOB_IC)
      render_text_in_color(NULL, c == SHLD_OFF_MOB_IC ? ' ' : '.', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == MOB_LBORD || c == MOB_RBORD || c == MOB_HOR || c == MOB_VER)
      render_text_in_color(NULL, c == MOB_LBORD ? '/' : c == MOB_RBORD ? '\\' : c == MOB_HOR ? '_' : '|', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (c == SHP_LBORD || c == SHP_RBORD || c == SHP_HOR || c == SHP_VER)
      render_text_in_color(NULL, c == SHP_LBORD ? '/' : c == SHP_RBORD ? '\\' : c == SHP_HOR ? '_' : '|', rand() % 2 == 0 ? plr_color1 : plr_color2);

    else if (c == RAND_IC)
      printf("%c", rand() % 2 == 0 ? '.' : rand() % 2 == 0 ? '\'' : ' ');

    // EXPLOSIONS
    else if (canv[i] == EXPL_START || (i + 1 < size && canv[i + 1] == EXPL_START) || (i - 1 >= 0 && canv[i - 1] == EXPL_START) || (i + CANV_W + 1 < size && canv[i + CANV_W + 1] == EXPL_START) ||
             (i - CANV_W - 1 >= 0 && canv[i - CANV_W - 1] == EXPL_START)) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? '.' : r == 1 ? '/' : r == 2 ? '\\' : '*', rand() % 2 == 0 ? mob_color1 : mob_color2);
    } else if (c == PRJ_COL) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? '.' : r == 1 ? '\'' : r == 2 ? ',' : '*', rand() % 2 == 0 ? RED : ORANGE);
    }

    // MINI MAP
    else if (c == MOB_MM_IC)
      render_text_in_color(NULL, '.', RED);
    else if (c == PLR_MM_IC)
      render_text_in_color(NULL, '.', GREEN);
    else if (c == SHOP_MM_IC)
      render_text_in_color(NULL, 'S', MAGENTA);
    else if (c == HOUSE_MM_IC)
      render_text_in_color(NULL, 'H', BLUE);
    else if (c == FORT_MM_IC)
      render_text_in_color(NULL, 'F', RED);
    else if (c == ARM_MM_IC)
      render_text_in_color(NULL, 'A', YELLOW);

    // COLORS
    else if (c == blu_dot || c == red_dot || c == grn_dot || c == yel_dot)
      render_text_in_color(NULL, '.', c == blu_dot ? BLUE : c == red_dot ? RED : c == grn_dot ? GREEN : YELLOW);
    else if (c == blu_L_parenth || c == red_L_parenth || c == grn_L_parenth || c == yel_L_parenth)
      render_text_in_color(NULL, '(', c == blu_L_parenth ? BLUE : c == red_L_parenth ? RED : c == grn_L_parenth ? GREEN : YELLOW);
    else if (c == blu_R_parenth || c == red_R_parenth || c == grn_R_parenth || c == yel_R_parenth)
      render_text_in_color(NULL, ')', c == blu_R_parenth ? BLUE : c == red_R_parenth ? RED : c == grn_R_parenth ? GREEN : YELLOW);
    else if (c == blu_ver_bar || c == red_ver_bar || c == grn_ver_bar || c == yel_ver_bar)
      render_text_in_color(NULL, '|', c == blu_ver_bar ? BLUE : c == red_ver_bar ? RED : c == grn_ver_bar ? GREEN : YELLOW);
    else if (c == blu_hor_bar || c == red_hor_bar || c == grn_hor_bar || c == yel_hor_bar)
      render_text_in_color(NULL, '_', c == blu_hor_bar ? BLUE : c == red_hor_bar ? RED : c == grn_hor_bar ? GREEN : YELLOW);
    else if (c == blu_dwn_arw || c == red_dwn_arw || c == grn_dwn_arw || c == yel_dwn_arw)
      render_text_in_color(NULL, 'v', c == blu_dwn_arw ? BLUE : c == red_dwn_arw ? RED : c == grn_dwn_arw ? GREEN : YELLOW);
    else if (c == blu_up_arw || c == red_up_arw || c == grn_up_arw || c == yel_up_arw)
      render_text_in_color(NULL, '^', c == blu_up_arw ? BLUE : c == red_up_arw ? RED : c == grn_up_arw ? GREEN : YELLOW);
    else if (c == blu_lft_arw || c == red_lft_arw || c == grn_lft_arw || c == yel_lft_arw)
      render_text_in_color(NULL, '<', c == blu_lft_arw ? BLUE : c == red_lft_arw ? RED : c == grn_lft_arw ? GREEN : YELLOW);
    else if (c == blu_rgt_arw || c == red_rgt_arw || c == grn_rgt_arw || c == yel_rgt_arw)
      render_text_in_color(NULL, '>', c == blu_rgt_arw ? BLUE : c == red_rgt_arw ? RED : c == grn_rgt_arw ? GREEN : YELLOW);
    else if (c == red_L_diag_l || c == grn_L_diag_l)
      render_text_in_color(NULL, '\\', c == red_L_diag_l ? RED : GREEN);
    else if (c == red_R_diag_r || c == grn_R_diag_r)
      render_text_in_color(NULL, '/', c == red_R_diag_r ? RED : GREEN);
    else
      printf("%c", c);
    i++;
  }
  return 1;
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

char *write_on_canv(char *src, char *dst, int x_pos, int y_pos) {
  int i = 0;
  int u = 0;
  int width = get_width(src);
  int height = get_height(src);

  int dst_width = get_width(dst);
  int dst_height = get_height(dst);

  if (x_pos + width + 1 <= 1 || y_pos + height + 1 <= 1) return 0;

  if (x_pos >= dst_width - 1 || x_pos + width < 0 || y_pos > dst_height - 1 || y_pos + height < 0) return 0;

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
  int canv_len = strlen(canv);
  if (!canv || (*start_index < 0 || (*start_index) > canv_len)) return;
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

int is_in_canv(int x_pos, int y_pos, int width, int height, int canv_x, int canv_y, int canv_w, int canv_h, DIR d) {
  int final_x = x_pos + (d == left || d == up_left || d == down_left ? -1 : d == right || d == up_right || d == down_right ? 1 : 0);
  int final_y = y_pos + (d == up || d == up_left || d == up_right ? -1 : d == down || d == down_right || d == down_left ? 1 : 0);
  return (final_x >= canv_x + 2 && final_x + width <= canv_w + canv_x - 2 && final_y >= canv_y + 2 && final_y + height <= canv_y + canv_h - 2);
}
