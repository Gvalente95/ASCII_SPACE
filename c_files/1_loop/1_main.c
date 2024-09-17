#include "../../headers/1_includes.h"

int main() {
  main_loop();
  return (1);
}

int main_loop() {
  set_nonblocking_mode(1);
  // signal(SIGSEGV, handle_signal), signal(SIGINT, handle_signal);
  // signal(SIGTERM, handle_signal), signal(SIGBUS, handle_signal), signal(SIGABRT, handle_signal);

  pid_t PRC_bg_audio;
  SHIP plr;
  BOSS boss;
  boss.cur_hp = 0;

  int wind[WIND_BUFF_SIZE];
  char *canv = NULL, *header = NULL, *prev_canv = NULL;
  int is_pausing = 0;
  int end_status = 0, new_item_timer = 0;
  int show_grid = 0, show_headr = 0, GLB_show_minimap, has_moved = 0, god_mode = 0;
  int running = 1;

  initialize_game(&plr);
  PRC_bg_audio = play_sound(AU_bg_world, 1);
  while (running) {
    GAME_STATE old_state = GLB_game_state;
    int prev_inv_size = GLB_owned_amnt;
    int cur_plr_hp = plr.hp;

    if (canv != NULL) free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');
    GLB_time++;
    GLB_level = GLB_score / 300;
    if (has_moved > 0) has_moved--;
    int new_inp = upd_input(canv, boss, &plr, &has_moved, &show_grid, &god_mode);
    if (new_inp != -1) {
      GLB_last_input = GLB_input;
      GLB_input = new_inp;
    }
    if (new_inp == 'm') GLB_show_minimap = !GLB_show_minimap;
    if (new_inp == 'h') show_headr = !show_headr;
    if (new_inp == 'i') handle_inventory_menu(canv);

    BLD_TYPE bld = GLB_cur_blding;
    upd_game_state(canv, &plr, &boss, has_moved, wind);

    if (bld != GLB_cur_blding) {
      stop_sound(PRC_bg_audio);
      PRC_bg_audio = play_sound(GLB_cur_blding != NONE ? AU_bg_house : AU_bg_world, 1);
    }

    render_game(boss, &plr, wind, canv, prev_canv, has_moved);
    if (show_headr) show_header(canv, header, plr, boss, has_moved);
    if (GLB_show_minimap || GLB_game_state == map_menu) handle_mini_map(canv, plr, boss);
    if (show_grid) show_grid_view(canv, show_grid);
    if (new_inp == 'v') arrays_menu(game);
    if (prev_inv_size < GLB_owned_amnt) { new_item_timer += 50; }
    if (GLB_owned_amnt >= INVENTORY_BUFF_SIZE - 1) write_midle_msg(canv, 1, "INVENTORY FULL");

    int y_offs = 0;
    for (int x = 0; x < GLB_owned_amnt; x++) {
      if (GLB_itm_ownd[x].center_x > -1) {
        ITEM *itm = &GLB_itm_ownd[x];
        itm->center_x--;
        char *itm_name = strdup(itm->name);
        if (itm_name != NULL) {
          y_offs += 1;
          int col = itm->rar > RARITY_COUNT ? RARITY_COUNT : itm->rar;
          Color_from_index(col, &itm_name);
          write_on_canv("ITEM COLLECTED: ", canv, PLR_CENTER_X - 10, PLR_CENTER_Y + y_offs);
          write_on_canv(itm_name, canv, PLR_CENTER_X + 10, PLR_CENTER_Y + y_offs);
          free(itm_name);
        }
      }
    }
    if (GLB_game_state != map_menu) render_canvas(canv, prev_canv);
    if (prev_canv != NULL) free(prev_canv);
    prev_canv = strdup(canv);

    if (old_state != GLB_game_state) GLB_game_previous_state = old_state;

    if (plr.hp < cur_plr_hp) usleep(HURT_SLEEPTIME);
    if (plr.hp <= 0) {
      end_status = set_plr_death(canv, plr);
      if (end_status == 1)
        plr.hp = PLR_MAX_HP;
      else
        break;
    }
    usleep(10000);
  }
  if (header != NULL) free(header);
  if (end_status == 2) main();

  system("clear");

  SDL_Quit();
  cleanup_game_objects();

  return 0;
}
