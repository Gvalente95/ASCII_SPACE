#include "../headers/includes.h"

const char *mv_type[] = {"normal", "on_grid", "watch_up", "cutscene", "free_slide", "mv_count"};
const char *atk_type[] = {"press", "autom", "disabled", "count"};
const char *boss_act[] = {"entrance", "idle", "attack0", "attack", "attack2", "sp_atk_1", "sp_atk_2", "reposition", "regen", "hurt", "shielding", "retreat", "calling", "die", "null", "count"};
const char *dir_labels[] = {"up", "dwn", "rgt", "lft", "up_lft", "up_rgt", "dwn_lft", "dwn_rgz", "none", "vert", "vert_left", "vert_right", "hor", "hor_up", "hor_down", "all", "circ", "DIR_COUNT"};
const char *prj_labels[] = {"base", "magnet", "bomb", "big", "ricochet", "prj_count"};
const char *bld_labels[] = {"SHOP", "ARMORY", "HOUSE", "FORTRESS", "NONE", "BLDING_COUNT"};
const char *g_state_labels[] = {"normal", "paused", "shopping", "boss_fight", "dialog", "BLDING_COUNT"};

const char *bonus_types_lbb[] = {"SCORE", "HP", "PRJ", "SHLD", "MAGNET", "LUCK", "SPEED", "POWER", "WISE", "GOLD", "COUNT"};
const char *effect_types_lbb[] = {"TP", "SHOW", "AOE", "HURT_MOB", "HURT_PLR", "HEAL_PLR", "FREEZE", "COUNT"};
const char *target_types_lbb[] = {"SELF", "CLOSEMOB", "SEL_MOB", "ALLMOBS", "COUNT"};
const char *rarity_types_lbb[] = {"CMON", "UNCMON", "RARE", "MYSTIC", "ARFCT", "DIVINE", "DIABOLICAL", "COUNT"};
const char *itm_types_lbb[] = {"MAP", "MATERIAL", "WEAPON", "ARMOR", "EXPANDABLE", "COUNT"};

int show_arrays_content(GAME game, PROJ *mobprj, PROJ *plrPRJ, SHIP *mobs, BLDING *bldings, ITEM *in_buff_itms, ITEM *in_bag_itms) {
  int inp = -1;
  int pg = 0;
  int buffer;
  char *window = NULL;

  while (inp != 'q') {
    buffer = pg == 0 ? items_OWNED_BUFFER : pg == 1 ? ITEM_BUFFER : pg == 2 ? PRJ_MOB_BUFFER : pg == 3 ? PROJ_BUFFER : pg == 4 ? MOBS_BUFFER : BLDING_BUFFER;
    if (buffer > CANV_H - 3) buffer = CANV_H - 3;
    inp = getchar();
    pg += inp == 'd' ? 1 : inp == 'a' ? -1 : 0;
    if (pg > 5) pg = 5;
    if (pg < 0) pg = 0;
    window = init_blank_canv(CANV_W, CANV_H, 3, ' ');
    char *page_label = pg == 0 ? "INVENTORY" : pg == 1 ? "IN_BUFFER_ITEMS" : pg == 2 ? "MOB_PROJECTILES" : pg == 3 ? "PLAYER_PROJECTILES" : pg == 4 ? "MOBS" : "BUILDINGS";
    write_on_canv(page_label, window, 1, 1);
    if (pg == 0) {
      int ind = 50;
      set_label(window, &ind, "owned:", game.owned_amnt);
      set_label(window, &ind, "/", items_OWNED_BUFFER);
    }

    for (int i = 0; i < buffer; i++) {

      char *fullText = init_blank_canv(100, 2, 0, ' ');

      char i_c = pg == 0 ? in_bag_itms[i].content : in_buff_itms[i].content;
      ITEM itm = pg == 0 ? in_bag_itms[i] : in_buff_itms[i];
      PROJ prj = pg == 2 ? mobprj[i] : plrPRJ[i];

      int x_p = pg <= 1 ? itm.x_pos : pg <= 3 ? prj.x_pos : pg == 4 ? mobs[i].x_pos : bldings[i].x_pos;
      int y_p = pg <= 1 ? itm.y_pos : pg <= 3 ? prj.y_pos : pg == 4 ? mobs[i].y_pos : bldings[i].y_pos;
      int n_v = pg <= 1 ? i_c : pg <= 3 ? prj.col_timer : pg == 4 ? mobs[i].hurt_timer : bldings[i].flr_am;
      char *txt = strdup(pg <= 1 ? itm.name : pg <= 3 ? "proj" : pg == 4 ? "mob" : "building");
      write_on_canv(txt, fullText, 0, 0);
      int str_index = 15;
      set_label(fullText, &str_index, "i: ", i), set_label(fullText, &str_index, "x_p:  ", x_p), set_label(fullText, &str_index, "y_p:  ", y_p);
      set_label(fullText, &str_index, pg <= 1 ? "icon:  " : pg <= 3 ? "col_timr:  " : pg == 4 ? "hrt_timer:  " : "floors_amount: ", pg <= 1 ? i_c : n_v);
      if (pg <= 1) {
        int type_val = itm.type > ITEM_Type_COUNT ? ITEM_Type_COUNT : itm.type;
        int rar_val = itm.rar > RARITY_COUNT ? RARITY_COUNT : itm.rar;

        char *typ = strdup(itm_types_lbb[type_val]);
        char *rar = strdup(rarity_types_lbb[rar_val]);
        set_label(fullText, &str_index, typ, '\0');
        set_label(fullText, &str_index, rar, '\0');
        set_label(fullText, &str_index, "price: ", itm.price);
        set_label(fullText, &str_index, "Ammount: ", itm.am);

        free(typ), free(rar);
      }

      write_on_canv(fullText, window, 0, i + 2);

      free(txt);
      free(fullText);
    }

    int i = 0;
    system("clear");
    while (window[i])
      printf("%c", window[i++]);
    usleep(70000);
  }
  if (window != NULL) free(window);
  return 1;
}

void show_on_map(char *minimap, int x_pos, int y_pos, int plr_x, int plr_y, int map_width, int map_height, char to_write) {
  if (x_pos == -1 || y_pos == -1) return;
  int rel_x = (x_pos - plr_x) / 20;
  int rel_y = (y_pos - plr_y) / 20;
  int x = (map_width / 2) + rel_x;
  int y = (map_height / 2) + rel_y;
  if (x < -5 || x > map_width + 5 || y < -5 || y > map_height + 5) return;
  x = (x <= 0 ? 0 : x >= map_width ? map_width - 1 : x);
  y = (y <= 0 ? 0 : y >= map_height ? map_height - 1 : y);
  int index = (x + ((map_width + 1) * y));
  if (minimap[index] && minimap[index] != '\n') minimap[index] = to_write;
}

void handle_mini_map(char *canv, GAME *game, SHIP plr, SHIP *mobs, BLDING *bldng, BOSS boss, ITEM *items, int time) {
  int mm_wdth = 20, mm_hght = 10;

  if (time % 2 == 0 || time <= 5) {
    char *minimap = init_blank_canv(mm_wdth, mm_hght, 1, '#');
    for (int i = 0; i < MOBS_BUFFER; i++) {
      if (mobs[i].x_pos == -1 || mobs[i].y_pos == -1 || mobs[i].hp <= 0) continue;
      show_on_map(minimap, mobs[i].x_pos, mobs[i].y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, MOB_MM_IC);
    }
    for (int i = 0; i < BLDING_BUFFER; i++) {
      BLD_TYPE blt = bldng[i].type;
      char c = blt == SHOP ? SHOP_MM_IC : blt == ARMORY ? ARM_MM_IC : blt == HOUSE ? HOUSE_MM_IC : blt == FORTRESS ? FORT_MM_IC : HOUSE_MM_IC;
      show_on_map(minimap, bldng[i].x_pos + (bldng[i].x_size / 2), bldng[i].y_pos + (bldng[i].y_size / 2), plr.x_pos, plr.y_pos, mm_wdth, mm_hght, c);
    }
    for (int i = 0; i < ITEM_BUFFER; i++) {
      if (items[i].x_pos == -1 || items[i].y_pos == -1 || items[i].content == '\0') continue;
      show_on_map(minimap, items[i].x_pos, items[i].y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, items[i].content);
    }
    char c[2];
    c[0] = plr.dir == up ? '^' : plr.dir == left ? '<' : plr.dir == right ? '>' : 'v';
    c[1] = '\0';
    color_text(green, c, 0);
    minimap[(mm_wdth + 1) / 2 + ((mm_wdth + 1) * (mm_hght / 2))] = c[0];
    game->minimap = strdup(minimap);
    free(minimap);
  }
  if (game->minimap != NULL) write_on_canv(game->minimap, canv, CANV_W - 21, CANV_H - 11);
}

int show_header(char *canv, char *header, GAME *game, SHIP plr, SHIP *mobs, BLDING *bldng, BOSS boss, int time, int has_moved, int *plr_shwcl_tm, int *prj_shwcl_tm) {
  if (header != NULL) free(header);
  int start_index;
  header = init_blank_canv(CANV_W, 4, 1, '#');
  start_index = CANV_W + 2;
  for (int x = 0; x < 7; x++) {
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
  set_label(canv, &start_index, g_state_labels[game->g_st], '\0');
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