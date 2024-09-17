#include "../../headers/1_includes.h"

void handle_signal(int signal) {
  if (signal == SIGSEGV)
    printf("Caught segmentation fault! Cleaning up...\n");
  else if (signal == SIGABRT)
    printf("Caught Sigabort! Cleaning up...\n");
  else if (signal == SIGBUS)
    printf("Caught Sigbus! Cleaning up...\n");
  else
    printf("Caught some unknown issue, cleaning up...\n");
  SDL_Quit();
  system("pkill afplay");
  exit(signal);
}

int arrays_menu(GAME game) {
  int inp = -1;
  int pg = 0;
  int buffer;
  char *window = NULL;

  ITEM *in_bag_itms = GLB_itm_ownd;
  ITEM *in_buff_itms = gmo.items;
  BLDING *bldings = gmo.buildings;
  PROJ *mobprj = gmo.proj_mob;
  PROJ *plrPRJ = gmo.proj;
  SHIP *mobs = gmo.mobs;

  while (inp != 'q') {
    buffer = pg == 0   ? INVENTORY_BUFF_SIZE
             : pg == 1 ? ITEM_DROPS_BUFF_SIZE
             : pg == 2 ? GLB_num_items
             : pg == 3 ? GLB_num_GLB_quests
             : pg == 4 ? QUESTS_BUFF_SIZE
             : pg == 5 ? MOBS_BUFFER
             : pg == 6 ? PROJ_BUFF_SIZE
             : pg == 7 ? PROJ_MOB_BUFF_SIZE
             : pg == 8 ? BLDING_BUFF_SIZE
                       : SHOP_BUFF_SIZE;
    pg += inp == 'd' ? 1 : inp == 'a' ? -1 : 0;
    pg = MINMAX(pg, 0, 9);
    window = init_blank_canv(CANV_W, CANV_H, 3, ' ');
    char *page_label = pg == 0   ? "INVENTORY"
                       : pg == 1 ? "BUFFER_ITEMS"
                       : pg == 2 ? "ITEM_LIST"
                       : pg == 3 ? "GLB_quest_list"
                       : pg == 4 ? "GLB_quests"
                       : pg == 5 ? "MOBS"
                       : pg == 6 ? "PROJ"
                       : pg == 7 ? "MOB_PROJ"
                       : pg == 8 ? "BUILDINGS"
                                 : "GLB_seller";
    write_on_canv(page_label, window, 1, 1);
    if (pg == 0) {
      int ind = 50;
      set_label(window, &ind, "owned:", GLB_owned_amnt);
      set_label(window, &ind, "/", INVENTORY_BUFF_SIZE);
    }

    for (int i = 0; i < MIN(CANV_H - 1, buffer); i++) {
      char *fullText = malloc(200);
      fullText[0] = 'n', fullText[1] = '\0';
      char i_c = pg == 0 ? in_bag_itms[i].content : in_buff_itms[i].content;

      if (pg <= 2) {
        ITEM t = pg == 0 ? in_bag_itms[i] : pg == 1 ? in_buff_itms[i] : GLB_itm_list[i];
        char *text = get_ITEM_information(&t);
        snprintf(fullText, 199, "%-3d %s", i, text);
        free(text);
      } else if (pg <= 4) {
        QUEST q = pg == 3 ? GLB_quest_list[i] : GLB_quests[i];
        if (i > buffer) continue;
        char *text = get_quest_debug_info(&q);
        snprintf(fullText, 199, "%-3d %s", i, text);
        free(text);
      } else if (pg == 9 && GLB_cur_bld_index != -1) {
        ITEM t = gmo.buildings[GLB_cur_bld_index].npcs[0].items[i];
        char *text = get_ITEM_information(&t);
        snprintf(fullText, 199, "%-3d %s", i, text);
        free(text);
      } else {
        int x_p = pg == 5 ? mobs[i].x_pos : pg == 6 ? plrPRJ[i].x_pos : pg == 7 ? mobprj[i].x_pos : bldings[i].x_pos;
        int y_p = pg == 5 ? mobs[i].y_pos : pg == 6 ? plrPRJ[i].y_pos : pg == 7 ? mobprj[i].y_pos : bldings[i].y_pos;
        int n_v = pg == 5 ? mobs[i].hurt_timer : pg == 6 ? plrPRJ[i].col_timer : pg == 7 ? mobprj[i].col_timer : bldings[i].flr_am;
        char *text = malloc(100);
        snprintf(text, 99, "%-3d X%-6d Y%-6d %-10s %-3c", i, x_p, y_p, pg == 5 ? "hrt_timer: " : pg == 6 || pg == 7 ? "col_timer: " : bld_types_lb[bldings[i].type], n_v);
        snprintf(fullText, 199, "%s", text);
        free(text);
      }
      write_on_canv(pg == 9 && GLB_cur_bld_index == -1 ? "MUST BE IN SHOP" : fullText, window, 3, i + 2);
      free(fullText);
    }
    int i = 0;
    system("clear");
    while (window[i])
      printf("%c", window[i++]);

    int new_p = -1;

    while (new_p == -1)
      new_p = getchar();
    inp = new_p;
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
  if (GLB_minimap[index] && GLB_minimap[index] != '\n') GLB_minimap[index] = to_write;
}

void show_full_map(char *canv, SHIP plr, BOSS boss) {
  char *map;
  int mm_wdth = CANV_W, mm_hght = CANV_H;
  SHIP *mobs = gmo.mobs;
  BLDING *bldng = gmo.buildings;
  ITEM *items = gmo.items;

  map = init_blank_canv(mm_wdth, mm_hght, 1, '#');
  for (int i = 0; i < MOBS_BUFFER; i++) {
    if (mobs[i].x_pos == -1 || mobs[i].y_pos == -1 || mobs[i].hp <= 0) continue;
    show_on_map(map, mobs[i].x_pos, mobs[i].y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, MOB_MM_IC);
  }
  for (int i = 0; i < BLDING_BUFF_SIZE; i++) {
    BLD_TYPE blt = bldng[i].type;
    char c = blt == SHOP ? SHOP_MM_IC : blt == GLB_armorY ? ARM_MM_IC : blt == HOUSE ? HOUSE_MM_IC : blt == FORTRESS ? FORT_MM_IC : HOUSE_MM_IC;
    show_on_map(map, bldng[i].x_pos + (bldng[i].x_size / 2), bldng[i].y_pos + (bldng[i].y_size / 2), plr.x_pos, plr.y_pos, mm_wdth, mm_hght, c);
  }
  for (int i = 0; i < ITEM_DROPS_BUFF_SIZE; i++) {
    if (items[i].x_pos == -1 || items[i].y_pos == -1 || items[i].content == '\0') continue;
    show_on_map(map, items[i].x_pos, items[i].y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, items[i].content);
  }
  char c[2];
  c[0] = plr.dir == up ? '^' : plr.dir == left ? '<' : plr.dir == right ? '>' : 'v';
  c[1] = '\0';
  color_text(green, c, 0);
  // show_on_map(map, GLB_gl_x_pos, GLB_gl_y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, c[0]);
  map[(mm_wdth + 1) / 2 + ((mm_wdth + 1) * (mm_hght / 2))] = c[0];

  system("clear");
  render_canvas(map, NULL);

  int np = -1;
  while (np == -1) {
    np = getchar();
  }
  if (np == 'M' || np == 'q') GLB_game_state = GLB_game_previous_state;
  free(map);
}

void handle_mini_map(char *canv, SHIP plr, BOSS boss) {
  SHIP *mobs = gmo.mobs;
  BLDING *bldng = gmo.buildings;
  ITEM *items = gmo.items;
  if (GLB_game_state == map_menu) {
    show_full_map(canv, plr, boss);
    return;
  }

  int mm_wdth = 20, mm_hght = 10;

  if (GLB_time % 2 == 0 || GLB_time <= 5) {
    char *GLB_minimap = init_blank_canv(mm_wdth, mm_hght, 1, '#');
    for (int i = 0; i < MOBS_BUFFER; i++) {
      if (mobs[i].x_pos == -1 || mobs[i].y_pos == -1 || mobs[i].hp <= 0) continue;
      show_on_map(GLB_minimap, mobs[i].x_pos, mobs[i].y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, MOB_MM_IC);
    }
    for (int i = 0; i < BLDING_BUFF_SIZE; i++) {
      BLD_TYPE blt = bldng[i].type;
      char c = blt == SHOP ? SHOP_MM_IC : blt == GLB_armorY ? ARM_MM_IC : blt == HOUSE ? HOUSE_MM_IC : blt == FORTRESS ? FORT_MM_IC : HOUSE_MM_IC;
      show_on_map(GLB_minimap, bldng[i].x_pos + (bldng[i].x_size / 2), bldng[i].y_pos + (bldng[i].y_size / 2), plr.x_pos, plr.y_pos, mm_wdth, mm_hght, c);
    }
    for (int i = 0; i < ITEM_DROPS_BUFF_SIZE; i++) {
      if (items[i].x_pos == -1 || items[i].y_pos == -1 || items[i].content == '\0') continue;
      show_on_map(GLB_minimap, items[i].x_pos, items[i].y_pos, plr.x_pos, plr.y_pos, mm_wdth, mm_hght, items[i].content);
    }
    char c[2];
    c[0] = plr.dir == up ? '^' : plr.dir == left ? '<' : plr.dir == right ? '>' : 'v';
    c[1] = '\0';
    color_text(green, c, 0);
    GLB_minimap[(mm_wdth + 1) / 2 + ((mm_wdth + 1) * (mm_hght / 2))] = c[0];
    GLB_minimap = strdup(GLB_minimap);
    free(GLB_minimap);
  }
  if (GLB_minimap != NULL) write_on_canv(GLB_minimap, canv, CANV_W - 21, CANV_H - 11);
}

int show_header(char *canv, char *header, SHIP plr, BOSS boss, int has_moved) {
  if (header != NULL) free(header);
  int start_index;
  header = init_blank_canv(CANV_W, 4, 1, '#');
  start_index = CANV_W + 2;
  for (int x = 0; x < 7; x++) {
    start_index += 2;
    header[start_index] = plr.hp > x ? HEART_IC : '.';
  }
  start_index += 4;

  char *txt = malloc(500);
  snprintf(txt, 499, "T.%d  Sc.%ld  Lv.%d   BLD[%s(i%d)]  G-st[%s|  GLB_is_in_dialog[%i]", GLB_time, GLB_score, GLB_level, bld_types_lb[GLB_cur_blding], GLB_cur_bld_index,
           game_state_types_lb[GLB_game_state], GLB_is_in_dialog);
  snprintf(txt + strlen(txt), 499, "\n  Px. %d Py. %d Wp. %d Atk. %d Sh. %d  is_blk %d", GLB_gl_x_pos, GLB_gl_y_pos, plr.weap, plr.atk_pow, plr.shield, plr.is_blocked);
  if (GLB_game_state == boss_fight) {
    snprintf(txt + strlen(txt), 499, "\n[BOSS] spd %d  hp %d sh %d  y %d x %d yDir %d  xDir%d  Dmg%d action:%s(tm left:%d)", boss.speed, PERCENTILE(boss.cur_hp, boss.max_HP),
             boss.shield, boss.y_pos, boss.x_pos, boss.y_dir, boss.x_dir, boss.dmg, boss_act[boss.action], boss.time_in_state);
  }
  set_label(header, &start_index, txt, '\0');
  free(txt);

  char *text = get_formatted_text("inp.   %c      prv_inp   %c", GLB_input == ' ' ? '_' : GLB_input, GLB_last_input == ' ' ? '_' : GLB_last_input);
  int y_pos = 3 + (CANV_W + 1) * (CANV_H - 6);

  start_index = y_pos;
  set_label(canv, &start_index, text, '\0');
  free(text);
  start_index = y_pos + CANV_W + 1;
  set_label(canv, &start_index, "dir.", '\0'), set_label(canv, &start_index, dir_types_lb[plr.dir], '\0');
  set_label(canv, &start_index, "rule.", '\0'), set_label(canv, &start_index, dir_types_lb[GLB_allowed_dir], '\0');
  start_index = y_pos + (CANV_W + 1) * 2;
  set_label(canv, &start_index, "lk.", '\0'), set_label(canv, &start_index, dir_types_lb[plr.lk_dir], '\0');
  set_label(canv, &start_index, "rule.", '\0'), set_label(canv, &start_index, dir_types_lb[GLB_allowed_lk_dir], '\0');
  start_index = y_pos + (CANV_W + 1) * 3;
  set_label(canv, &start_index, mv_type_lb[GLB_mv_typ], '\0'), set_label(canv, &start_index, atk_type_lb[GLB_atk_typ], '\0');
  set_label(canv, &start_index, "mvd. ", has_moved);
  set_label(canv, &start_index, prj_types_lb[GLB_cur_proj], '\0');
  set_label(canv, &start_index, "GLB_game_previous_state. ", '\0');
  set_label(canv, &start_index, game_state_types_lb[GLB_game_state], '\0');
  set_label(canv, &start_index, "cur_st. ", '\0');
  set_label(canv, &start_index, game_state_types_lb[GLB_game_previous_state], '\0');

  write_on_canv(header, canv, 0, 0);
  free(header);
  return (1);
}

int show_grid_view(char *canv, char show_grid) {
  if (show_grid) {
    for (int y = 0; y < CELL_SIZE; y++) {
      for (int x = 0; x < CELL_SIZE; x++) {
        char *cell = init_blank_canv(CANV_W / CELL_SIZE, CANV_H / CELL_SIZE, show_grid, ' ');
        if (x == CELL_SIZE / 2) GLB_plr_x_centr = (CANV_W / CELL_SIZE) * x;
        if (x == CELL_SIZE / 2) GLB_plr_x_centr = (CANV_W / CELL_SIZE) * x;

        write_on_canv(cell, canv, (CANV_W / CELL_SIZE) * x, (CANV_H / CELL_SIZE) * y);
        free(cell);
      }
    }
  }
  return 1;
}