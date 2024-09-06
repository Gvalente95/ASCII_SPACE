#include "../headers/includes.h"

const char *mv_type[] = {"normal", "on_grid", "watch_up", "cutscene", "free_slide", "mv_count"};
const char *atk_type[] = {"press", "autom", "disabled", "count"};
const char *boss_act[] = {"entrance", "idle", "attack0", "attack", "attack2", "sp_atk_1", "sp_atk_2", "reposition", "regen", "hurt", "shielding", "retreat", "calling", "die", "null", "count"};
const char *dir_labels[] = {"up", "dwn", "rgt", "lft", "up_lft", "up_rgt", "dwn_lft", "dwn_rgz", "none", "vert", "vert_left", "vert_right", "hor", "hor_up", "hor_down", "all", "DIR_COUNT"};
const char *prj_labels[] = {"base", "magnet", "bomb", "big", "prj_count"};
const char *bld_labels[] = {"SHOP", "ARMORY", "HOUSE", "FORTRESS", "NONE", "BLDING_COUNT"};

int show_header(char *canv, char *header, GAME *game, SHIP plr, SHIP *mobs, BLDING *bldng, BOSS boss, int time, int has_moved, int *plr_shwcl_tm, int *prj_shwcl_tm) {
  if (header != NULL) free(header);
  int start_index;
  int mm_wdth = 20, mm_hght = 10;

  if (time % 15 == 0 || time <= 5) {
    char *minimap = init_blank_canv(mm_wdth, mm_hght, 1, '#');
    for (int i = 0; i < MOBS_BUFFER; i++) {
      if (mobs[i].x_pos == -1 || mobs[i].y_pos == -1 || mobs[i].hp <= 0) continue;
      int rel_x = (mobs[i].x_pos - plr.x_pos) / 20;
      int rel_y = (mobs[i].y_pos - plr.y_pos) / 20;
      int x = (mm_wdth / 2) + rel_x;
      int y = (mm_hght / 2) + rel_y;
      if (x < -5 || x > mm_wdth + 5 || y < -5 || y > mm_hght + 5) continue;
      x = (x <= 0 ? 0 : x >= mm_wdth ? mm_wdth - 1 : x);
      y = (y <= 0 ? 0 : y >= mm_hght ? mm_hght - 1 : y);
      int index = (x + ((mm_wdth + 1) * y));
      if (minimap[index] && minimap[index] != '\n') minimap[index] = MOB_MM_IC;
    }
    for (int i = 0; i < BLDING_BUFFER; i++) {

      int rel_x = (bldng[i].x_pos + (bldng[i].x_size / 2) - plr.x_pos) / 20;
      int rel_y = (bldng[i].y_pos + (bldng[i].y_size / 2) - plr.y_pos) / 20;
      int x = (mm_wdth / 2) + rel_x;
      int y = (mm_hght / 2) + rel_y;
      if (x < -5 || x > mm_wdth + 5 || y < -5 || y > mm_hght + 5) continue;
      x = (x <= 0 ? 0 : x >= mm_wdth ? mm_wdth - 1 : x);
      y = (y <= 0 ? 0 : y >= mm_hght ? mm_hght - 1 : y);
      int index = (x + ((mm_wdth + 1) * y));
      BLD_TYPE blt = bldng[i].type;
      if (minimap[index] && minimap[index] != '\n') minimap[index] = blt == SHOP ? SHOP_MM_IC : blt == ARMORY ? ARM_MM_IC : blt == HOUSE ? HOUSE_MM_IC : blt == FORTRESS ? FORT_MM_IC : HOUSE_MM_IC;
    }
    minimap[(mm_wdth + 1) / 2 + ((mm_wdth + 1) * (mm_hght / 2))] = PLR_MM_IC;
    game->minimap = strdup(minimap);
    free(minimap);
  }
  if (game->minimap != NULL) write_on_canv(game->minimap, canv, CANV_W - 21, CANV_H - 11);
  header = init_blank_canv(CANV_W, 4, 1, '#');
  start_index = CANV_W + 2;
  for (int x = 0; x < 10; x++) {
    start_index += 2;
    header[start_index] = plr.hp > x ? HEART_IC : '.';
  }

  start_index += 4;
  set_label(header, &start_index, "T.", time);
  set_label(header, &start_index, "Sc.", game->score);
  set_label(header, &start_index, "Lv.", game->level);
  set_label(header, &start_index, bld_labels[game->cur_blding], game->cur_bld_index);
  set_label(header, &start_index, game->plr_col_index == -2 ? "BS_col." : game->plr_col_index == -1 ? "noCl" : "Mb_col.", game->plr_col_index);
  if (game->plr_col_index != -1) *plr_shwcl_tm = 30;
  set_label(header, &start_index, game->prj_col_index == -2 ? "BS_prj." : game->prj_col_index == -1 ? "noCl" : "Mb_prj.", game->plr_col_index);
  if (game->prj_col_index != -1) *prj_shwcl_tm = 30;
  // set_label(header, &start_index, "Gy.", game.gl_y_pos);

  char *first_line = get_formatted_text("Px. %d Py. %d Wp. %d Atk. %d Sh. %d  is_blk %d", plr.x_pos, plr.y_pos, plr.weap, plr.atk_pow, plr.shield, plr.is_blocked);
  set_label(header, &start_index, first_line, '\0');
  while (header[start_index] != '\n')
    start_index++;
  start_index += 5;
  if (game->g_st == boss_fight) {
    set_label(header, &start_index, "Bspd.", boss.speed), set_label(header, &start_index, "Bhp\%", PERCENTILE(boss.cur_hp, boss.max_HP));
    set_label(header, &start_index, "Bsh.", boss.shield);
    set_label(header, &start_index, "By.", boss.y_pos), set_label(header, &start_index, "Bx.", boss.x_pos);
    set_label(header, &start_index, "Bydir.", boss.y_dir), set_label(header, &start_index, "Bxdir.", boss.y_dir);
    set_label(header, &start_index, "B Atk.", boss.dmg), set_label(header, &start_index, boss_act[boss.action], '\0'), set_label(header, &start_index, " tm.", boss.time_in_state);
  }

  char *text = get_formatted_text("inp.   %c      prv_inp   %c", game->input == ' ' ? '_' : game->input, game->last_input == ' ' ? '_' : game->last_input);
  int y_pos = 3 + (CANV_W + 1) * (CANV_H - 6);

  start_index = y_pos;
  set_label(canv, &start_index, text, '\0');
  free(text);
  start_index = y_pos + CANV_W + 1;
  set_label(canv, &start_index, "dir.", '\0'), set_label(canv, &start_index, dir_labels[plr.dir], '\0');
  set_label(canv, &start_index, "rule.", '\0'), set_label(canv, &start_index, dir_labels[game->allowed_dir], '\0');
  start_index = y_pos + (CANV_W + 1) * 2;
  set_label(canv, &start_index, "lk.", '\0'), set_label(canv, &start_index, dir_labels[plr.lk_dir], '\0');
  set_label(canv, &start_index, "rule.", '\0'), set_label(canv, &start_index, dir_labels[game->allowed_lk_dir], '\0');
  start_index = y_pos + (CANV_W + 1) * 3;
  set_label(canv, &start_index, mv_type[game->mv_type], '\0'), set_label(canv, &start_index, atk_type[game->atk_type], '\0');
  set_label(canv, &start_index, "mvd. ", has_moved);
  set_label(canv, &start_index, prj_labels[game->cur_proj], '\0');
  write_on_canv(header, canv, 0, 0);
  free(header);
  return (1);
}

int show_grid_view(char *canv, GAME game, char show_grid) {
  if (show_grid) {
    for (int y = 0; y < CELL_SIZE; y++) {
      for (int x = 0; x < CELL_SIZE; x++) {
        char *cell = init_blank_canv(CANV_W / CELL_SIZE, CANV_H / CELL_SIZE, show_grid, ' ');
        if (x == CELL_SIZE / 2) game.plr_x_centr = (CANV_W / CELL_SIZE) * x;
        if (x == CELL_SIZE / 2) game.plr_x_centr = (CANV_W / CELL_SIZE) * x;

        write_on_canv(cell, canv, (CANV_W / CELL_SIZE) * x, (CANV_H / CELL_SIZE) * y);
        free(cell);
      }
    }
  }
  return 1;
}