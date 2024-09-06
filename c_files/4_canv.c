#include "../headers/includes.h"

void render_npcs(GAME *game, BOSS boss, SHIP *plr, SHIP *mobs, BLDING *bldings, PROJ *proj, PROJ *mobprj, int *wind, char *canv, int time, int has_moved, int aftr_plr) {

  // Render NPCs
  if (game->cur_bld_index != 0) {
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
              n->text = strdup(tp == SHOP ? slr_dlgs[rand_range(0, 14)] : forg_dlgs[rand_range(0, 13)]);
              game->cur_seller = n;
            } else
              n->text = strdup(tp == FORTRESS ? adv_dlgs[rand_range(0, 20)] : tp == SHOP || tp == ARMORY ? shpr_dlgs[rand_range(0, 20)] : expl_dlgs[rand_range(0, 20)]);
          }

          char *txt = strdup(n->text);
          int tx_len = strlen(txt);
          int tx_w = get_width(txt), tx_h = get_height(txt);
          bub_BG = init_blank_canv(tx_w + 5, tx_h + 4, 0, ' ');
          char *bub = init_blank_canv(tx_w + 4, tx_h + 3, 1, '#');
          write_on_canv("###", bub, (tx_w + 4) / 2 + 2, tx_h + 2);
          write_on_canv("\\/", bub_BG, (tx_w + 4) / 2 + 3, tx_h + 3);

          write_on_canv(bub, bub_BG, 0, 0);

          x_dial = x - ((tx_w + 4) / 2);
          y_dial = y - (tx_h + 4);

          write_on_canv(txt, bub_BG, 2, 1), free(txt), free(bub);
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
        if (rand() % 50 == 0 && n->type != SELLER) n->dir = n->lk_dir;
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
}

void render_game(GAME *game, BOSS boss, SHIP *plr, SHIP *mobs, BLDING *bldings, PROJ *proj, PROJ *mobprj, int *wind, char *canv, int time, int has_moved) {
  (void)boss, (void)proj, (void)mobprj, (void)wind;

  // Render mobs
  if (game->g_st != boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      SHIP mob = mobs[i];
      char *mob_tri;
      if (i <= MOBS1_BUFFER)
        mob_tri = get_triangle('*', mob.dir, SHIP_W, SHIP_H, 1);
      else
        mob_tri = mob.dir == up ? strdup(MB2_U) : mob.dir == down ? strdup(MB2_D) : mob.dir == left || mob.dir == up_left || mob.dir == down_left ? strdup(MB2_L) : strdup(MB2_R);
      if (mob.hurt_timer > 0) set_text_red(mob_tri);
      if (mob.hurt_timer > 0) {
        int i = 0;
        while (mob_tri[i]) {
          if (mob_tri[i] != ' ' && mob_tri[i] != '\n') mob_tri[i] = EXPL_START;
          i++;
        }
      }
      if (mob.x_pos - SHIP_W > 0 && mob.x_pos < CANV_W && mob.y_pos - SHIP_H < CANV_H && mob.y_pos < CANV_H && canv[i] != 'N' && canv[i] != '#') write_on_canv(mob_tri, canv, mob.x_pos, mob.y_pos);
      if (mob_tri != NULL) free(mob_tri);
    }
  }

  // RENDER INSIDE OF BUILDING
  if (game->cur_blding != NONE) {
    BLDING *bld = &bldings[game->cur_bld_index];
    char *tmp = strdup(bld->floors[game->cur_floor]);
    write_on_canv(tmp, canv, bld->x_pos, bld->y_pos);
    if (tmp != NULL) free(tmp);
    int i = 0;
    while (canv[i]) {
      if (canv[i] != '\n' && canv[i] == ' ') canv[i] = i % 5 == 0 ? WIND_DOT : ' ';
      i++;
    }
  } // RENDER OUTSIDE
  else {
    for (int i = 0; i < BLDING_BUFFER; i++) {
      BLDING *bld = &bldings[i];
      int is_inside = game->cur_bld_index == i;
      if ((bld->x_pos < CANV_W - 1 && bld->x_pos + bld->x_size > 1) && (bld->y_pos < CANV_H - 1 && bld->y_pos + bld->y_size > 1)) {
        char *tmp = strdup(is_inside ? bld->floors[game->cur_floor] : bld->ext_cont);
        write_on_canv(tmp, canv, bld->x_pos, bld->y_pos);
        if (tmp != NULL) free(tmp);
      }
    }
  }

  upd_collisions(canv, game, proj, &boss, mobs, mobprj, plr);

  render_npcs(game, boss, plr, mobs, bldings, proj, mobprj, wind, canv, time, has_moved, 0);
  // Render player ship
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
    write_on_canv(curB, canv, plr->x_pos, plr->y_pos - 4);
    free(curB);
  }
  render_npcs(game, boss, plr, mobs, bldings, proj, mobprj, wind, canv, time, has_moved, 1);
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
    if (c == '#') printf(" ");
    // PROJECTILES
    else if (c == PRJ_BS_HOR || c == PRJ_BS_VER || c == PRJ_BS_DIAG_L || c == PRJ_BS_DIAG_R)
      render_text_in_color(NULL, c == PRJ_BS_HOR ? '_' : c == PRJ_BS_VER ? '|' : c == PRJ_BS_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? RED : ORANGE);
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

    // ITEMS
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
    else if (c == RAND_IC)
      printf("%c", rand() % 2 == 0 ? '.' : rand() % 2 == 0 ? '\'' : ' ');

    // EXPLOSIONS
    else if (canv[i] == EXPL_START || canv[i + 1] == EXPL_START || canv[i - 1] == EXPL_START || canv[i + CANV_W + 1] == EXPL_START || canv[i - CANV_W - 1] == EXPL_START) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? '.' : r == 1 ? '/' : r == 2 ? '\\' : '*', rand() % 2 == 0 ? mob_color1 : mob_color2);
    } else if (c == PRJ_COL) {
      int r = rand_range(0, 4);
      render_text_in_color(NULL, r == 0 ? '.' : r == 1 ? '\'' : r == 2 ? ',' : '*', rand() % 2 == 0 ? RED : ORANGE);
    } else if (c == RED_ICON)
      render_text_in_color(NULL, '#', RED);

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

int set_lotterie_menu(char *header, char *canv, GAME *game, SHIP *plr) {
  (void)plr;
  char *upg_canv = init_blank_canv(UPG_CANV_W, UPG_CANV_H, 1, '#');
  char *msg = strdup(LV_MSG);
  write_on_canv(header, canv, 0, 0);
  write_on_canv(msg, upg_canv, (UPG_CANV_W - get_width(msg)) / 2, 3);
  free(msg);
  game->lv_choices = rand_range(1, 7);
  ITEM_Type l_choice[game->lv_choices];
  int x_range = -1, y_range = 0;
  for (int i = 0; i < game->lv_choices; i++) {
    x_range++;
    if (x_range > 2) {
      x_range = 0;
      y_range++;
    }
    ITEM_Type new_ITEM;
    do
      new_ITEM = rand() % ITEM_TYPE_COUNT;
    while (contains(l_choice, i, new_ITEM));
    l_choice[i] = new_ITEM;

    int cell_x = 20, cell_y = 10;

    char *choice_txt = init_blank_canv(cell_x, cell_y, 1, '#');
    char *elem = l_choice[i] == MOR_ATTR   ? ATTR_GFX
                 : l_choice[i] == MOR_ITEM ? LUCK_GFX
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

  if (x_pos >= dst_width || x_pos + width < 0 || y_pos > dst_height || y_pos + height < 0) return 0;

  while (y_pos > 0)
    if (dst[i++] == '\n') y_pos--;

  while (y_pos < 0)
    if (src[u++] == '\n') y_pos++;

  char *mod_src = strdup(src);

  if (x_pos < 0) crop_x(mod_src, abs(x_pos));

  if (x_pos + width > dst_width - 1) mod_src = set_new_width(mod_src, dst_width - 1 - x_pos);

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
  free(mod_src);
  return 1;
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
