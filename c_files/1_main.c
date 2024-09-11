#include "../headers/includes.h"

int main() {
  // int init = SDL_Init(SDL_INIT_EVERYTHING);
  main_loop();
  return (1);
}

const char *bonus_types_lb[] = {"SCORE", "HP", "PRJ", "SHLD", "MAGNET", "LUCK", "SPEED", "POWER", "WISE", "GOLD", "COUNT"};
const char *effect_types_lb[] = {"TP", "SHOW", "AOE", "HURT_MOB", "HURT_PLR", "HEAL_PLR", "FREEZE", "COUNT"};
const char *target_types_lb[] = {"SELF", "CLOSEMOB", "SEL_MOB", "ALLMOBS", "COUNT"};
const char *rarity_types_lb[] = {"CMON", "UNCMON", "RARE", "MYSTIC", "ARFCT", "DIVINE", "DIABOLICAL", "COUNT"};
const char *itm_types[] = {"MAP", "MATERIAL", "WEAPON", "ARMOR", "COUNT"};

int main_loop() {
  set_nonblocking_mode(1);
  pid_t PRC_bg_audio = play_sound(AU_bg_world, 1);

  int np = -1;
  int x_p = 5, y_p = 5;
  int is_pausing = 0;
  int width = 10;

  int end_status;
  int new_item_timer = 0;
  int time = 0, show_grid = 0, show_headr = 0, show_minimap, has_moved = 0, god_mode = 0, plr_shwcl_tm = 0, prj_shwcl_tm = 0;
  int wind[WIND_BUFFER];
  char *canv = NULL, *header = NULL;
  SHIP plr;
  BOSS boss;
  boss.cur_hp = 0;
  GAME game;
  SHIP *mobs = malloc(sizeof(SHIP) * MOBS_BUFFER);
  if (mobs == NULL) { return 1; }
  PROJ *proj = malloc(sizeof(PROJ) * PROJ_BUFFER);
  if (proj == NULL) { return 1; }
  PROJ *proj_mob = malloc(sizeof(PROJ) * PRJ_MOB_BUFFER);
  if (proj_mob == NULL) { return 1; }
  BLDING *buildings = malloc(sizeof(BLDING) * BLDING_BUFFER);
  if (buildings == NULL) { return 1; }
  int n_items;
  game.itm_list = read_items_from_file("stuff/items.txt", &n_items);
  if (game.itm_list == NULL) { return 1; }
  game.num_items = n_items;

  game.itm_ownd = malloc(sizeof(ITEM) * items_OWNED_BUFFER);
  for (int i = 0; i < items_OWNED_BUFFER; i++)
    game.itm_ownd[i].content = '\0';

  ITEM *items = malloc(sizeof(ITEM) * ITEM_BUFFER);
  for (int i = 0; i < ITEM_BUFFER; i++)
    items[i].content = '\0';

  STAR *stars = malloc(sizeof(STAR) * STAR_BUFFER);
  if (stars == NULL) { return 1; }

  initialize_game(&game, &plr, items, mobs, proj, proj_mob, stars, buildings);

  FILE *file = fopen(SAVEFILE, "r");
  if (file != NULL) fscanf(file, "%ld", &game.max_score), fclose(file);

  while (1) {
    useconds_t ref_time = game.g_st == boss_fight ? 7000 : 13000;
    usleep(ref_time);

    int prev_inv_size = game.owned_amnt;
    int cur_plr_hp = plr.hp;

    time++;
    game.level = game.score / 300;

    if (plr_shwcl_tm > -1) plr_shwcl_tm--;
    if (prj_shwcl_tm > -1) prj_shwcl_tm--;
    if (has_moved > 0) has_moved--;

    int new_inp = set_input(canv, &game, boss, &plr, proj, proj_mob, &has_moved, time, &show_grid, &god_mode);
    if (new_inp == 'q') {
      system("pkill afplay");
      exit(0);
    }
    if (is_in_string(new_inp, "wasd") && game.cur_blding != NONE) play_sound(AU_thump, 0);
    if (new_inp != -1) {
      game.last_input = game.input;
      game.input = new_inp;
    }
    if (new_inp == 'm') show_minimap = !show_minimap;
    if (new_inp == 'h') show_headr = !show_headr;
    if (canv != NULL) free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');

    BLD_TYPE bld = game.cur_blding;
    upd_game_state(canv, &game, &plr, &boss, mobs, proj, proj_mob, items, buildings, stars, has_moved, time, wind);

    if (bld != game.cur_blding) {
      stop_sound(PRC_bg_audio);
      PRC_bg_audio = play_sound(game.cur_blding != NONE ? AU_bg_house : AU_bg_world, 1);
    }
    render_game(&game, boss, &plr, mobs, buildings, proj, proj_mob, wind, canv, time, has_moved);
    if (show_headr) show_header(canv, header, &game, plr, mobs, buildings, boss, time, has_moved, &plr_shwcl_tm, &prj_shwcl_tm);
    if (show_minimap) handle_mini_map(canv, &game, plr, mobs, buildings, boss, items, time);
    if (show_grid) show_grid_view(canv, game, show_grid);
    if (new_inp == 'v') show_arrays_content(game, proj_mob, proj, mobs, buildings, items, game.itm_ownd);
    if (prev_inv_size < game.owned_amnt) { new_item_timer += 50; }
    if (game.owned_amnt >= items_OWNED_BUFFER - 1) { write_on_canv("INVENTORY FULL", canv, PLR_CENTER_X - 10, PLR_CENTER_Y + 5); }

    int y_offs = 0;
    for (int x = 0; x < game.owned_amnt; x++) {
      if (game.itm_ownd[x].center_x > -1) {
        ITEM *itm = &game.itm_ownd[x];
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
  if (items != NULL) free(items);
  if (end_status == 2) main();

  fopen(SAVEFILE, "w");
  if (file != NULL) {
    if (game.score > game.max_score) game.max_score = game.score;
    fprintf(file, "%ld", game.max_score);
    fclose(file);
  }
  system("clear");

  // SDL_Quit();
  return 0;
}