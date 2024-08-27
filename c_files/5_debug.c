#include "../headers/includes.h"

const char *mv_type[] = {"normal", "on_grid", "watch_up", "cutscene", "free_slide", "mv_count"};
const char *atk_type[] = {"press", "autom", "count"};
const char *boss_act[] = {"entrance", "idle", "attack0", "attack", "attack2", "sp_atk_1", "sp_atk_2", "reposition", "regen", "hurt", "shielding", "retreat", "calling", "die", "null", "count"};
const char *dir_labels[] = {"up", "dwn", "rgt", "lft", "up_lft", "up_rgt", "dwn_lft", "dwn_rgz", "none", "vert", "vert_left", "vert_right", "hor", "hor_up", "hor_down", "all", "DIR_COUNT"};
const char *prj_labels[] = {"base", "magnet", "bomb", "big", "prj_count"};

int show_header(char *canv, char *header, GAME game, SHIP plr, SHIP *mobs, BOSS boss, int time, int has_moved, int *plr_shwcl_tm, int *prj_shwcl_tm) {
  if (header != NULL) free(header);
  int start_index;
  char *minimap = init_blank_canv(20, 10, 1, ' ');
  for (int i = 0; i < MOBS_BUFFER; i++) {
    if (mobs[i].x_pos == -1 || mobs[i].y_pos == -1 || mobs[i].hp <= 0) continue;

    int rel_x = (mobs[i].x_pos - plr.x_pos) / 50;
    int rel_y = (mobs[i].y_pos - plr.y_pos) / 50;
    int x = 10 + rel_x; // 10 is the center x of the 20-width minimap
    int y = 5 + rel_y;  // 5 is the center y of the 10-height minimap
    if (x <= 0) x = 0;
    if (x >= 20) x = 19;
    if (y <= 0) y = 0;
    if (y >= 10) y = 9;

    DIR d = mobs[i].dir;

    char *to_write = d == left ? "<" : d == right ? ">" : d == up ? "^" : d == down ? "v" : "?";

    write_on_canv(to_write, minimap, x, y);
  }
  write_on_canv("\xA0", minimap, 10, 5);

  write_on_canv(minimap, canv, CANV_W - 21, CANV_H - 11);
  header = init_blank_canv(CANV_W, 4, 1, ' ');
  start_index = CANV_W + 2;
  for (int x = 0; x < 10; x++) {
    start_index += 2;
    header[start_index] = plr.hp > x ? HEART_IC : '.';
  }
  start_index += 4;
  set_label(header, &start_index, "T.", time);
  set_label(header, &start_index, "Sc.", game.score);
  set_label(header, &start_index, "Lv.", game.level);
  set_label(header, &start_index, game.plr_col_index == -2 ? "BS_col." : game.plr_col_index == -1 ? "noCl" : "Mb_col.", game.plr_col_index);
  if (game.plr_col_index != -1) *plr_shwcl_tm = 30;
  set_label(header, &start_index, game.prj_col_index == -2 ? "BS_prj." : game.prj_col_index == -1 ? "noCl" : "Mb_prj.", game.plr_col_index);
  if (game.prj_col_index != -1) *prj_shwcl_tm = 30;
  // set_label(header, &start_index, "Gy.", game.gl_y_pos);

  char *first_line = get_formatted_text("Px. %d Py. %d Wp. %d Atk. %d Sh. %d", plr.x_pos, plr.y_pos, plr.weap, plr.atk_pow, plr.shield);
  set_label(header, &start_index, first_line, '\0');
  while (header[start_index] != '\n')
    start_index++;
  start_index += 5;
  if (game.is_in_boss_fight) {
    set_label(header, &start_index, "Bspd.", boss.speed), set_label(header, &start_index, "Bhp\%", PERCENTILE(boss.cur_hp, boss.max_HP));
    set_label(header, &start_index, "Bsh.", boss.shield);
    set_label(header, &start_index, "By.", boss.y_pos), set_label(header, &start_index, "Bx.", boss.x_pos);
    set_label(header, &start_index, "Bydir.", boss.y_dir), set_label(header, &start_index, "Bxdir.", boss.y_dir);
    set_label(header, &start_index, "B Atk.", boss.dmg), set_label(header, &start_index, boss_act[boss.action], '\0'), set_label(header, &start_index, " tm.", boss.time_in_state);
  }

  char *text = get_formatted_text("inp.   %c      prv_inp   %c", game.input == ' ' ? '_' : game.input, game.last_input == ' ' ? '_' : game.last_input);
  int y_pos = 3 + (CANV_W + 1) * (CANV_H - 6);

  start_index = y_pos;
  set_label(canv, &start_index, text, '\0');
  free(text);
  start_index = y_pos + CANV_W + 1;
  set_label(canv, &start_index, "dir.", '\0'), set_label(canv, &start_index, dir_labels[plr.dir], '\0');
  set_label(canv, &start_index, "rule.", '\0'), set_label(canv, &start_index, dir_labels[game.allowed_dir], '\0');
  start_index = y_pos + (CANV_W + 1) * 2;
  set_label(canv, &start_index, "lk.", '\0'), set_label(canv, &start_index, dir_labels[plr.lk_dir], '\0');
  set_label(canv, &start_index, "rule.", '\0'), set_label(canv, &start_index, dir_labels[game.allowed_lk_dir], '\0');
  start_index = y_pos + (CANV_W + 1) * 3;
  set_label(canv, &start_index, mv_type[game.mv_type], '\0'), set_label(canv, &start_index, atk_type[game.atk_type], '\0');
  set_label(canv, &start_index, "mvd. ", has_moved);
  set_label(canv, &start_index, prj_labels[game.cur_proj], '\0');
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