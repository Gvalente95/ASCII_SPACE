#include "../headers/includes.h"

int main() {
  int init = SDL_Init(SDL_INIT_EVERYTHING);
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
  int np = -1;
  int x_p = 5, y_p = 5;
  int is_pausing = 0;
  int width = 10;
  /*
  while (1) {
    char *test = init_blank_canv(50, 25, 3, ' ');
    char *inside = init_blank_canv(25, 20, 3, '.');
    if (width > 0) { inside = set_new_width(inside, get_width(inside) - width); }
    char *ver = init_blank_canv(3, 7, 3, 'u');
    write_on_canv(ver, inside, get_width(inside) / 2, get_height(inside) / 2);

    int u = 0;
    while (inside[u]) {
      if (inside[u + 1] == '\0') inside[u] = 'o';
      if (inside[u] == '\n') inside[u - 1] = 'n';
      u++;
    }

    int canv_w = get_width(test) - 1, canv_h = get_height(test) - 1;

    np = getchar();
    if (np != -1 || is_pausing) {
      if (is_in_string(np, "wasd")) is_pausing = 1;
      while (np == -1)
        np = getchar();
      x_p += np == 'd' ? 1 : np == 'a' ? -1 : 0;
      y_p += np == 'w' ? -1 : np == 's' ? 1 : 0;
      if (np == 'q') return 0;
      if (np == 'c') is_pausing = 0;
      np = -1;
    } else
      x_p++;
    if (x_p > canv_w) {
      x_p = -get_width(inside);
      y_p++;
    }
    if (x_p < -get_width(inside) - 1) x_p = 50;
    if (y_p > canv_h) y_p = -get_height(inside);
    if (y_p < -get_height(inside)) y_p = canv_h;
    // inside = fit_in_right(test, inside, x_p, y_p);
    char *new_ver = write_on_canv(inside, test, x_p, y_p);
    system("clear");
    int i = 0;
    while (test[i])
      printf("%c", test[i++]);
    printf("\n\n");
    if (new_ver) {
      i = 0;
      while (new_ver[i]) {
        printf("%c", new_ver[i + 1] == '\n' ? 'n' : new_ver[i]);
        i++;
      }
    }

    usleep(900);
    free(test), free(inside);
  }
  return 0;
  */

  int i = 5;
  while (0) {
    i++;
    int v = getchar();
    if (v == 'q') return 0;
    char *base = strdup("|123456789|\n|abcdefghi|\n|^>.-:_=!;|\n");
    char *text = strdup(base);
    if (i > strlen(text)) i = 0;
    system("clear");
    int u = 0;
    while (text[u])
      printf("%c", text[u++]);
    free(text), text = strdup(base);
    printf("\nRIGHT: \n");
    swap_line(text, i, right);
    u = 0;
    while (text[u])
      printf("%c", text[u++]);
    free(text), text = strdup(base);

    printf("\n\nMIRROR: \n");
    mirror_txt(text, i, right);
    u = 0;
    while (text[u])
      printf("%c", text[u++]);

    free(text), text = strdup(base);
    usleep(50000);
    printf("\n\n%d\n", i);
  }

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

  for (int i = 0; i < game.num_items; i++) {
    ITEM it = game.itm_list[i];
    char *type = strdup(itm_types[it.type > ITEM_Type_COUNT ? ITEM_Type_COUNT : it.type]);
    char *rar = strdup(rarity_types_lb[it.rar > RARITY_COUNT ? RARITY_COUNT : it.rar]);
    printf("%s\n%c\nprc:%d\ntyp:%s\nrar:%s\n\n", it.name, it.content, it.price, type, rar);
    free(type), free(rar);
  }
  printf("%d", game.num_items);
  int inp = -1;
  while (inp == -1)
    inp = getchar();
  game.itm_ownd = malloc(sizeof(ITEM) * items_OWNED_BUFFER);
  for (int i = 0; i < items_OWNED_BUFFER; i++)
    game.itm_ownd[i].content = '\0';

  ITEM *items = malloc(sizeof(ITEM) * ITEM_BUFFER);
  for (int i = 0; i < ITEM_BUFFER; i++)
    items[i].content = '\0';

  STAR *stars = malloc(sizeof(STAR) * STAR_BUFFER);
  if (stars == NULL) {
    puts("stars failed to alloc");
    return 1;
  }

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
    if (new_inp != -1) {
      game.last_input = game.input;
      game.input = new_inp;
    }
    if (new_inp == 'm') show_minimap = !show_minimap;
    if (new_inp == 'h') show_headr = !show_headr;
    if (canv != NULL) free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');

    upd_game_state(canv, &game, &plr, &boss, mobs, proj, proj_mob, items, buildings, stars, has_moved, time, wind);
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

  SDL_Quit();
  return 0;
}