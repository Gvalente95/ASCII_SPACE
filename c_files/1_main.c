#include "../headers/includes.h"

int main() {
  main_loop();
  return (1);
}

int main_loop() {
  int end_status;
  int time = 0, show_grid = 0, has_moved = 0, god_mode = 0, plr_shwcl_tm = 0, prj_shwcl_tm = 0;
  int wind[WIND_BUFFER];
  char *canv = NULL, *header = NULL;
  SHIP plr;
  BOSS boss;
  boss.cur_hp = 0;
  GAME game;
  SHIP *mobs = malloc(sizeof(SHIP) * MOBS_BUFFER);
  PROJ *proj = malloc(sizeof(PROJ) * PROJ_BUFFER);
  PROJ *proj_mob = malloc(sizeof(PROJ) * PRJ_MOB_BUFFER);
  BLDING *buildings = malloc(sizeof(BLDING) * BLDING_BUFFER);
  LOOT *loots = malloc(sizeof(LOOT) * LOOT_BUFFER);
  STAR *stars = malloc(sizeof(STAR) * STAR_BUFFER);

  initialize_game(&game, &plr, loots, mobs, proj, proj_mob, stars, buildings);

  FILE *file = fopen(SAVEFILE, "r");
  if (file != NULL) fscanf(file, "%ld", &game.max_score), fclose(file);

  while (1) {
    usleep(game.is_in_boss_fight ? 7000 : 10000);
    time++;
    int cur_plr_hp = plr.hp;
    if (plr_shwcl_tm > -1) plr_shwcl_tm--;
    if (prj_shwcl_tm > -1) prj_shwcl_tm--;
    if (has_moved > 0) has_moved--;

    int new_inp = set_input(&game, boss, &plr, proj, proj_mob, &has_moved, time, &show_grid, &god_mode);
    if (new_inp != -1) {
      game.last_input = game.input;
      game.input = new_inp;
    }
    if (new_inp == 'p') {
      char *pause_canv = init_blank_canv(30, 8, 1, '#');
      int ch = -1;
      write_on_canv("Press 'p' to un-pause", pause_canv, 4, 4);
      write_on_canv(pause_canv, canv, CANV_X_CENTER - 15, CANV_Y_CENTER - 4);
      render_canvas(canv, &game, time);
      while (ch != 'p') {
        ch = getchar();
      }
    }
    game.level = game.score / 300;
    if (canv != NULL) free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');

    upd_game_state(canv, &game, &plr, &boss, mobs, proj, proj_mob, loots, stars, has_moved, time, wind);
    render_game(game, boss, &plr, mobs, proj, proj_mob, wind, canv);
    show_header(canv, header, game, plr, mobs, boss, time, has_moved, &plr_shwcl_tm, &prj_shwcl_tm);
    if (show_grid) show_grid_view(canv, game, show_grid);
    render_canvas(canv, &game, time);
    if (plr.hp < cur_plr_hp) usleep(HURT_SLEEPTIME);
    if (plr.hp <= 0) {
      end_status = set_plr_death(canv, game, plr);
      if (end_status == 1)
        plr.hp = PLR_MAX_HP;
      else
        break;
    }
  }
  if (header != NULL) free(header);
  if (mobs != NULL) free(mobs);
  if (proj != NULL) free(proj);
  if (canv != NULL) free(canv);
  if (stars != NULL) free(stars);
  if (proj_mob != NULL) free(proj_mob);
  if (buildings != NULL) free(buildings);
  if (loots != NULL) free(loots);

  if (end_status == 2) main();

  fopen(SAVEFILE, "w");
  if (file != NULL) {
    if (game.score > game.max_score) game.max_score = game.score;
    fprintf(file, "%ld", game.max_score);
    fclose(file);
  }
  system("clear");
  return 0;
}