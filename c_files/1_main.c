#include "../headers/includes.h"

int main() {
  int init = SDL_Init(SDL_INIT_EVERYTHING);
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
  if (mobs == NULL) {
    puts("mobs failed to alloc");
    return 1;
  }
  PROJ *proj = malloc(sizeof(PROJ) * PROJ_BUFFER);
  if (proj == NULL) {
    puts("proj failed to alloc");
    return 1;
  }
  PROJ *proj_mob = malloc(sizeof(PROJ) * PRJ_MOB_BUFFER);
  if (proj_mob == NULL) {
    puts("proj_mob failed to alloc");
    return 1;
  }
  BLDING *buildings = malloc(sizeof(BLDING) * BLDING_BUFFER);
  if (buildings == NULL) {
    puts("buildings failed to alloc");
    return 1;
  }
  int n_items;
  game.itm_list = read_items_from_file("stuff/items.txt", &n_items);
  if (game.itm_list == NULL) {
    puts("item_list failed to alloc");
    return 1;
  }
  game.num_items = n_items;
  /*
  for (int i = 0; i < game.num_items; i++) {
    ITEM it = game.itm_list[i];
    printf("%s\n%c\nprc:%d\nrsel:%d\ntyp:%d\nrar:%d\n\n", it.name, it.content, it.price, it.val_inc, it.type, it.rar);
  }
  return 1;*/

  game.itm_ownd = malloc(sizeof(ITEM) * ITEMS_OWNED_BUFFER);
  for (int i = 0; i < ITEMS_OWNED_BUFFER; i++)
    game.itm_ownd[i].content = '\0';

  ITEM *ITEMs = malloc(sizeof(ITEM) * ITEM_BUFFER);
  for (int i = 0; i < ITEM_BUFFER; i++)
    ITEMs[i].content = '\0';

  STAR *stars = malloc(sizeof(STAR) * STAR_BUFFER);
  if (stars == NULL) {
    puts("stars failed to alloc");
    return 1;
  }

  initialize_game(&game, &plr, ITEMs, mobs, proj, proj_mob, stars, buildings);

  FILE *file = fopen(SAVEFILE, "r");
  if (file != NULL) fscanf(file, "%ld", &game.max_score), fclose(file);

  while (1) {
    useconds_t ref_time = game.g_st == boss_fight ? 7000 : 10000;
    usleep(ref_time);
    time++;
    int cur_plr_hp = plr.hp;
    if (plr_shwcl_tm > -1) plr_shwcl_tm--;
    if (prj_shwcl_tm > -1) prj_shwcl_tm--;
    if (has_moved > 0) has_moved--;

    int new_inp = set_input(canv, &game, boss, &plr, proj, proj_mob, &has_moved, time, &show_grid, &god_mode);
    if (new_inp != -1) {
      game.last_input = game.input;
      game.input = new_inp;
    }
    game.level = game.score / 300;
    if (game.cur_seller != NULL) {
      if (game.g_st != shopping) {
        if (game.input == ' ')
          game.g_st = shopping;
        else
          game.cur_seller = NULL;
      } else
        handle_seller_menu(canv, &game, game.cur_seller, buildings, &plr, mobs, &boss, proj_mob);
    }
    if (canv != NULL) free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');

    upd_game_state(canv, &game, &plr, &boss, mobs, proj, proj_mob, ITEMs, buildings, stars, has_moved, time, wind);
    render_game(&game, boss, &plr, mobs, buildings, proj, proj_mob, wind, canv, time, has_moved);
    show_header(canv, header, &game, plr, mobs, buildings, boss, time, has_moved, &plr_shwcl_tm, &prj_shwcl_tm);
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
  if (ITEMs != NULL) free(ITEMs);
  if (end_status == 2) main();

  fopen(SAVEFILE, "w");
  if (file != NULL) {
    if (game.score > game.max_score) game.max_score = game.score;
    fprintf(file, "%ld", game.max_score);
    fclose(file);
  }
  system("clear");

  SDL_Quit();
  return 0;
}