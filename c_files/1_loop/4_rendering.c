#include "../../headers/1_includes.h"

int is_col(char c) { return (c == blu_st || c == grn_st || c == red_st || c == orn_st || c == yel_st || c == prpl_st); }
int render_canvas(char *canv, char *prev_canv) {

  if (canv == NULL) return (0);
  if (prev_canv != NULL && strcmp(prev_canv, canv) == 0) { return (0); }
  if (GLB_time % 50 == 0) system("clear");

  char *mob_color1 = GLB_level < 5 ? BLUE : GLB_level < 10 ? GREEN : GLB_level < 15 ? YELLOW : GLB_level < 20 ? ORANGE : GLB_level < 25 ? RED : MAGENTA;
  char *mob_color2 = GLB_level < 5 ? LIGHT_BLUE : GLB_level < 10 ? DARK_GREEN : GLB_level < 15 ? PALE_YELLOW : GLB_level < 20 ? RED : GLB_level < 25 ? BROWN : CYAN;
  char *plr_color1 = BRIGHT_RED, *plr_color2 = CYAN;
  int i = 0;
  int size = strlen(canv);

  while (canv[i]) {
    char c = canv[i];
    if (c == '\n') {
      printf("\033[K");
      i++;
      printf("\n");
      continue;
    }
    if (c == '#') {
      printf(" ");
    } else if (canv[i] == ERASE_IC) {
      printf("  ");
    } else if (c == HEART_IC) {
      render_text_in_color(HEART_SMB, '\0', RED);
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
      char *col = strdup(c == blu_st ? BLUE : c == red_st ? RED : c == grn_st ? GREEN : c == orn_st ? ORANGE : c == yel_st ? YELLOW : MAGENTA);
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
    } else if (c == BLD_BACKGROUND1 || c == BLD_BACKGROUND2) {
      render_text_in_color(NULL, '.', MAGENTA);
    }

    // BASE MOB PROJ
    else if (c == MPRJ_BS_HOR || c == MPRJ_BS_VER || c == MPRJ_BS_DIAG_L || c == MPRJ_BS_DIAG_R)
      render_text_in_color(NULL,
                           c == MPRJ_BS_HOR      ? '_'
                           : c == MPRJ_BS_VER    ? GLB_game_state == boss_fight ? 'O' : '|'
                           : c == MPRJ_BS_DIAG_L ? '\\'
                                                 : '/',
                           rand() % 2 == 0 ? mob_color1 : mob_color2);

    // WIND, STARS
    else if (c == WIND_DOT || c == WIND_VER || c == WIND_HOR)
      render_text_in_color(NULL, c == WIND_DOT ? '.' : c == WIND_VER ? '|' : '-', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == STAR_IC || c == STAR_IC2)
      render_text_in_color(NULL, c == STAR_IC ? '+' : '.', rand() % 2 == 0 ? YELLOW : PALE_YELLOW);
    else if (c == WIND_ALL_VER)
      render_text_in_color(NULL, GLB_time % 36 >= 15 ? '.' : GLB_time % 25 >= 10 ? 'l' : ' ', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == WIND_ALL_HOR)
      render_text_in_color(NULL, GLB_time % 24 >= 18 ? '.' : GLB_time % 15 >= 12 ? '-' : ' ', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == WIND_ALL_DOT)
      render_text_in_color(NULL, GLB_time % 28 >= 80 ? '.' : GLB_time % 37 >= 20 ? ',' : ' ', rand() % 2 == 0 ? plr_color1 : plr_color2);

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

    // MOBS AND GLB_ship
    else if (c == SHLD_OFF_MOB_IC || c == SHLD_ON_MOB_IC)
      render_text_in_color(NULL, c == SHLD_OFF_MOB_IC ? ' ' : '.', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == MOB_LBORD || c == MOB_RBORD || c == MOB_HOR || c == MOB_VER)
      render_text_in_color(NULL, c == MOB_LBORD ? '/' : c == MOB_RBORD ? '\\' : c == MOB_HOR ? '_' : '|', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (c == SHP_LBORD || c == SHP_RBORD || c == SHP_HOR || c == SHP_VER)
      render_text_in_color(NULL, c == SHP_LBORD ? '/' : c == SHP_RBORD ? '\\' : c == SHP_HOR ? '_' : '|', rand() % 2 == 0 ? plr_color1 : plr_color2);

    else if (c == RAND_IC)
      printf("%c", rand() % 2 == 0 ? '.' : rand() % 2 == 0 ? '\'' : ' ');

    // EXPLOSIONS
    else if (canv[i] == EXPL_START || (i + 1 < size && canv[i + 1] == EXPL_START) || (i - 1 >= 0 && canv[i - 1] == EXPL_START) ||
             (i + CANV_W + 1 < size && canv[i + CANV_W + 1] == EXPL_START) || (i - CANV_W - 1 >= 0 && canv[i - CANV_W - 1] == EXPL_START)) {
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

  printf("\033[H");

  return 1;
}

void render_mobs(char *canv) {
  int canv_len = strlen(canv);
  if (GLB_game_state != boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      SHIP *mob = &gmo.mobs[i];
      if (mob->x_pos - PLR_SHIP_W > 0 && mob->x_pos < CANV_W && mob->y_pos - PLR_SHIP_H < CANV_H && mob->y_pos < CANV_H && canv[i] != 'N' && canv[i] != '#') {
        char *mob_tri;
        if (i <= MOBS1_BUFFER)
          mob_tri = get_triangle('*', mob->dir, PLR_SHIP_W, PLR_SHIP_H, 1);
        else if (i <= MOBS_BUFFER - 50)
          mob_tri = mob->dir == up                                                     ? strdup(MB2_U)
                    : mob->dir == down                                                 ? strdup(MB2_D)
                    : mob->dir == left || mob->dir == up_left || mob->dir == down_left ? strdup(MB2_L)
                                                                                       : strdup(MB2_R);
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
        if (mob->weap) {
          int st_index = get_index(canv, mob->x_pos + mob->width / 2, mob->y_pos - 2);
          if (st_index > -1 && st_index < canv_len && canv[st_index] != '\n') {
            char *txt = malloc(15);
            snprintf(txt, 15, "%d/%dHP", mob->hp, mob->maxHP);
            if (mob->hurt_timer > 0) color_text(red, txt, 1);
            set_label(canv, &st_index, txt, '\0');
          }
          if (mob->hurt_timer > 0) {
            mob->hurt_timer--;
            color_text(red, mob_tri, 0);
          }
        }

        mob->height = get_height(mob_tri), mob->width = get_width(mob_tri);
        write_on_canv(mob_tri, canv, mob->x_pos, mob->y_pos);
        if (mob_tri != NULL) free(mob_tri);
      }
    }
  }
}
void render_player(char *canv, SHIP *plr) {
  if (GLB_cur_bld_index == -1) {
    char *plr_txt = get_triangle('*', GLB_mv_typ == watch_up ? up : plr->lk_dir, PLR_SHIP_W, PLR_SHIP_H, 2);
    if (plr->shield) {
      char *circ = get_circle(PLR_SHIP_W + 6, PLR_SHIP_W + 6, plr->shield <= 2 ? PLR_SHLD_OUTL : plr->shield <= 4 ? PLR_SHLD_OUTL1 : PLR_SHLD_OUTL2, (float)plr->shield / 6);
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
      curB = strdup(plr->dir == left      ? GLB_time % 20 >= 10 ? plr_mv_left1 : plr_mv_left2
                    : plr->dir == right   ? GLB_time % 20 >= 10 ? plr_mv_right1 : plr_mv_right2
                    : plr->dir == up      ? GLB_time % 20 >= 10 ? plr_mv_up1 : plr_mv_up2
                    : GLB_time % 20 >= 10 ? plr_mv_down1
                                          : plr_mv_down2);
    } else
      curB = strdup(d == up ? plr_idle_up : d == left ? plr_idle_left : d == right ? plr_idle_right : plr_idle_down1);
    color_text(red, curB, 0);
    write_on_canv(curB, canv, plr->x_pos - 2, plr->y_pos - 4);
    free(curB);
  }
}
void render_npcs(SHIP *plr, int *wind, char *canv, char *prv_canv, int has_moved, int aftr_plr) {
  char *bub_BG = NULL;
  int x_dial = 0, y_dial = 0, bld_ind = GLB_cur_bld_index;
  BLDING *bldings = gmo.buildings;
  for (int i = 0; i < bldings[bld_ind].npc_am; i++) {
    BLDING *bld = &bldings[bld_ind];
    NPC *n = &bldings[bld_ind].npcs[i];
    int x = n->x_p + bldings[bld_ind].x_pos;
    int y = n->y_p + bldings[bld_ind].y_pos;
    if (n->floor != GLB_cur_floor) continue;
    if (aftr_plr ? y < plr->y_pos - 3 : y > plr->y_pos - 3) continue;

    if (abs(y - plr->y_pos) < 8 && abs(x - plr->x_pos) < 8) {
      if (GLB_input == 't' && (GLB_is_in_dialog != -1 && GLB_is_in_dialog != i)) {
        GLB_is_in_dialog = i;
        GLB_input = '\0';
      }
      if (GLB_input == ' ' && (GLB_is_in_dialog == -1) || GLB_is_in_dialog == i) {
        if (GLB_is_in_dialog == -1 && n->type == GLB_seller) {
          handle_seller_menu(canv, n, plr);
          GLB_is_in_dialog = -1;
          GLB_input = '.';
        } else if (GLB_is_in_dialog == i && n->quest.name[0] != '\0' && n->type != GLB_seller) {
          char *txt = get_quest_description(n->quest);
          char *old = strdup(prv_canv);
          write_midle_msg(prv_canv, 0, txt);
          free(txt);
          render_canvas(prv_canv, NULL);
          GLB_input = get_char_and_freeze(" q");
          GLB_is_in_dialog = -1;
          if (GLB_input == ' ') {
            QUEST *new = set_in_list(GLB_quests, n->quest, GLB_num_GLB_quests);
            if (new != NULL) new->q_status = NEW;
            n->quest.name[0] = '\0';
            prv_canv = strdup(old);
            free(old);
            write_midle_msg(prv_canv, 1, "\"%s\" added to your inventory!\n", &new->name);
            render_canvas(prv_canv, NULL);
            usleep(500000);
            GLB_input = '\0';
          }
        } else {
          bub_BG = get_text_bubble(n->text, n->name);
          int tx_w = get_width(n->text), tx_h = get_height(n->text);
          x_dial = x - ((tx_w + 4) / 2);
          y_dial = y - (tx_h + 4);
          GLB_is_in_dialog = i;
          GLB_input = '.';
        }

      } else {
        if (n->quest.name[0] != '\0') write_on_canv(" ___\n|#\xA1!|\n#---", canv, x + 2, y - 3);
        if (GLB_is_in_dialog == i) GLB_is_in_dialog = -1;
      }

      if (GLB_is_in_dialog == i || has_moved) {
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
      if (GLB_is_in_dialog == i) GLB_is_in_dialog = -1;
    }

    if (GLB_is_in_dialog != i) {
      if (rand() % 200 == 0) n->lk_dir = rand() % 4;
      if (rand() % 500 == 0 && n->type != GLB_seller) n->dir = n->lk_dir;
    } else
      n->dir = none;

    if (n->dir != none && GLB_time % 5 == 0) {
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
      curB = strdup(n->dir == left        ? GLB_time % 20 >= 10 ? plr_mv_left1 : plr_mv_left2
                    : n->dir == right     ? GLB_time % 20 >= 10 ? plr_mv_right1 : plr_mv_right2
                    : n->dir == up        ? GLB_time % 20 >= 10 ? plr_mv_up1 : plr_mv_up2
                    : GLB_time % 20 >= 10 ? plr_mv_down1
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
void render_building_interior(char *canv) {
  BLDING *bld = &gmo.buildings[GLB_cur_bld_index];
  char *tmp = strdup(bld->floors[GLB_cur_floor]);
  write_on_canv(tmp, canv, bld->x_pos, bld->y_pos);
  if (tmp != NULL) free(tmp);
  int i = 0;
  while (canv[i]) {
    if (canv[i] != '\n' && canv[i] == ' ') canv[i] = i % 5 == 0 ? BLD_BACKGROUND1 : ' ';
    i++;
  }
}
void render_buildings_exterior(char *canv) {
  for (int i = 0; i < BLDING_BUFF_SIZE; i++) {
    BLDING *bld = &gmo.buildings[i];
    if ((bld->x_pos < CANV_W - 1 && bld->x_pos + bld->x_size > 1) && (bld->y_pos < CANV_H - 1 && bld->y_pos + bld->y_size > 1))
      write_on_canv(bld->ext_cont, canv, bld->x_pos, bld->y_pos);
  }
}

void render_game(BOSS boss, SHIP *plr, int *wind, char *canv, char *prv_canv, int has_moved) {
  int in_blding = GLB_cur_blding != NONE;

  render_mobs(canv);
  if (in_blding) render_building_interior(canv);
  if (in_blding) render_npcs(plr, wind, canv, prv_canv, has_moved, 0);
  upd_collisions(canv, plr);
  render_player(canv, plr);
  if (!in_blding) render_buildings_exterior(canv);

  if (in_blding) render_npcs(plr, wind, canv, prv_canv, has_moved, 1);
}