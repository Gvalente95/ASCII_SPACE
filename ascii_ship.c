#include "header.h"

const char *mv_type[] = {"normal", "on_grid", "watch_up", "cutscene", "free_slide", "mv_count"};
const char *atk_type[] = {"press", "autom", "count"};
const char *boss_act[] = {"entrance", "idle", "attack", "sp_atk_1", "sp_atk_2", "reposition", "regen", "hurt", "shielding", "retreat", "calling", "die", "null", "count"};
const char *dir_labels[] = {"up", "dwn", "rgt", "lft", "none", "up_lft", "up_rgt", "dwn_lft", "dwn_rgt", "DIR_COUNT"};

// Function prototypes
void initialize_game(GAME *game, SHIP *plr, LOOT *loots, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars);
int handle_input(GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mobprj, int *has_moved, int time);
int handle_superloot_menu(char *header, char *canv, GAME *game, SHIP *plr);
void update_game_state(GAME *game, SHIP *mobs, BOSS *boss, PROJ *proj, PROJ *mobprj, SHIP *plr, LOOT *loots, int has_moved, int time, int *wind, char *canv);
void render_game(GAME game, BOSS boss, SHIP *plr, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *wind, char *canv);
DIR determine_direction(int w_prj, int max_prj, SHIP plr);
void initialize_projectile(PROJ *proj, int i, DIR dir, SHIP *plr);
void init_boss(GAME game, BOSS *boss);

int main() {
  int time = 0, show_grid = 0, has_moved = 0, god_mode = 0, plr_shwcl_tm = 0, prj_shwcl_tm = 0;
  int wind[WIND_BUFFER];
  char *canv = NULL, *header = NULL;
  SHIP plr;
  SHIP *mobs = malloc(sizeof(SHIP) * MOB_AM);
  BOSS boss;
  PROJ *proj = malloc(sizeof(PROJ) * PROJ_BUFFER), *proj_mob = malloc(sizeof(PROJ) * PRJ_MOB_BUFFER);
  LOOT *loots = malloc(sizeof(LOOT) * LOOT_BUFFER);
  STAR *stars = malloc(sizeof(STAR) * STAR_BUFFER);
  for (int i = 0; i < STAR_BUFFER - 1; i++) {
    stars[i].x_pos = rand_range(1, CANV_W - 1);
    stars[i].y_pos = rand_range(1, CANV_H - 1);
    stars[i].z_pos = rand_range(2, 16);
  }
  BLDING *buildings = malloc(sizeof(BLDING) * BLDING_BUFFER);
  for (int i = 0; i < BLDING_BUFFER - 1; i++) {
    buildings[i].x_pos = rand_range(-200, 200);
    buildings[i].y_pos = rand_range(-200, 200);
    buildings[i].y_size = rand_range(5, 40);
    buildings[i].x_size = buildings[i].y_size * 2;

    buildings[i].type = rand_range(0, BLD_TYPE_COUNT);
    buildings[i].content = init_blank_canv(buildings[i].x_size, buildings[i].y_size, 1, '#');
  }

  // Initialize game entities
  set_nonblocking_mode(1);
  GAME game;
  initialize_game(&game, &plr, loots, mobs, proj, proj_mob, stars);

  FILE *file = fopen(SAVEFILE, "r"); // Open file in read mode
  if (file != NULL) {
    fscanf(file, "%ld", &game.max_score); // Pass the address of max_score
    fclose(file);
  } else
    printf("Error opening file for reading.\n");

  // Main game loop
  while (1) {
    usleep(7000);
    time++;
    if (time % BOSS_INTERVAL == 0 && !game.is_in_boss_fight) {
      game.is_in_boss_fight = 1;
      game.boss_mode = 2;
      init_boss(game, &boss);
    }
    if (plr_shwcl_tm > -1) plr_shwcl_tm--;
    if (prj_shwcl_tm > -1) prj_shwcl_tm--;

    int cur_lv = game.level;
    game.level = game.score / 300;
    if (cur_lv != game.level) {
    }

    int inp = handle_input(&game, boss, &plr, proj, proj_mob, &has_moved, time);
    if (inp == 'm') show_grid = !show_grid;
    if (inp == 'g') {
      god_mode = !god_mode;
      if (god_mode) {
        plr.atk_am = 2;
        plr.atk_pow = 3;
        plr.atk_speed = 3;
        plr.hp = 50;
        plr.weap = 8;
      } else {
        plr.weap = 1;
        plr.hp = 5;
      }
    }

    if (canv != NULL) free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');

    for (int i = 0; i < STAR_BUFFER - 1; i++) {
      if (game.mv_type != on_grid || has_moved) {
        if (time % (plr.dir == left || plr.dir == right ? stars[i].z_pos / 2 : stars[i].z_pos) == 0) {
          stars[i].x_pos += plr.dir == left ? 1 : plr.dir == right ? -1 : 0;
          stars[i].y_pos += plr.dir == up ? 1 : plr.dir == down ? -1 : 0;
        }
      }
      if (game.is_in_boss_fight) {
        if (boss.action == SP_attack2) stars[i].y_pos--;
        if (boss.action == SP_attack || (boss.action == retreat && time % 4 == 0)) stars[i].y_pos++;
      }

      if (stars[i].x_pos > CANV_W - 1 || stars[i].x_pos < 1 || stars[i].y_pos > CANV_H - 1 || stars[i].y_pos < 1) {
        stars[i].x_pos = plr.dir == left ? 1 : plr.dir == right ? CANV_W - 1 : rand_range(1, CANV_W - 1);
        stars[i].y_pos = plr.dir == up ? 1 : plr.dir == down ? CANV_H - 1 : rand_range(1, CANV_H - 1);
      }
      int z = stars[i].z_pos;
      char *c;
      if (z <= 4)
        c = strdup(STAR_FAR);
      else {
        c = malloc(2);
        c[0] = z <= 8 ? STAR_IC : STAR_IC2;
        c[1] = '\0';
      }
      add_to_canv(c, canv, stars[i].x_pos, stars[i].y_pos);
      free(c);
    }

    if (show_grid) {
      for (int y = 0; y < CELL_SIZE; y++) {
        for (int x = 0; x < CELL_SIZE; x++) {
          char *cell = init_blank_canv(CANV_W / CELL_SIZE, CANV_H / CELL_SIZE, show_grid, ' ');
          if (x == CELL_SIZE / 2) game.plr_x_centr = (CANV_W / CELL_SIZE) * x;
          if (x == CELL_SIZE / 2) game.plr_x_centr = (CANV_W / CELL_SIZE) * x;

          add_to_canv(cell, canv, (CANV_W / CELL_SIZE) * x, (CANV_H / CELL_SIZE) * y);
          free(cell);
        }
      }
    }

    update_game_state(&game, mobs, &boss, proj, proj_mob, &plr, loots, has_moved, time, wind, canv);
    if (has_moved > 0) has_moved--;

    render_game(game, boss, &plr, mobs, proj, proj_mob, wind, canv);

    for (int i = 0; i < BLDING_BUFFER - 1; i++) {
      buildings[i].x_pos += plr.dir == left ? 1 : plr.dir == right ? -1 : 0;
      buildings[i].y_pos += plr.dir == up ? 1 : plr.dir == down ? -1 : 0;
      if (buildings[i].x_pos <= CANV_W - 1 && buildings[i].x_pos + buildings[i].x_size > 1 && buildings[i].y_pos <= CANV_H - 1 && buildings[i].y_pos + buildings[i].y_size > 1) {
        char *text = strdup(buildings[i].content);
        if (text != NULL) {
          add_to_canv(text, canv, buildings[i].x_pos, buildings[i].y_pos);
          free(text);
        }
      }
    }

    int i = 0;
    if (header != NULL) free(header);
    header = init_blank_canv(CANV_W, 5, 1, ' ');
    int start_index = CANV_W + 2;
    for (int x = 0; x < PLR_MAX_HP; x++) {
      start_index += 2;
      header[start_index] = plr.hp > x ? HEART_IC : '.';
    }
    start_index += 4;
    write_label_and_value(header, &start_index, "T.", time);
    write_label_and_value(header, &start_index, "Sc.", game.score);
    write_label_and_value(header, &start_index, "Lv.", game.level);
    if (game.plr_col_index != -1) {
      write_label_and_value(header, &start_index, game.plr_col_index == -2 ? "BS_col." : game.plr_col_index == -1 ? "noCl" : "Mb_col.", game.plr_col_index);
      plr_shwcl_tm = 30;
    }
    if (game.prj_col_index != -1) {
      write_label_and_value(header, &start_index, game.prj_col_index == -2 ? "BS_prj." : game.prj_col_index == -1 ? "noCl" : "Mb_prj.", game.plr_col_index);
      prj_shwcl_tm = 30;
    }
    // write_label_and_value(header, &start_index, "Gy.", game.gl_y_pos);
    write_label_and_value(header, &start_index, "Px.", plr.x_pos);
    write_label_and_value(header, &start_index, "Py.", plr.y_pos);
    write_label_and_value(header, &start_index, "Wp.", plr.weap);
    write_label_and_value(header, &start_index, "Atk.", plr.atk_pow);
    write_label_and_value(header, &start_index, "Sh.", plr.shield);
    write_label_and_value(header, &start_index, mv_type[game.mv_type], '\0');
    write_label_and_value(header, &start_index, atk_type[game.atk_type], '\0');
    write_label_and_value(header, &start_index, dir_labels[plr.dir], '\0');

    while (header[start_index] != '\n')
      start_index++;
    start_index += 5;
    write_label_and_value(header, &start_index, "Bspd.", boss.speed);
    write_label_and_value(header, &start_index, "Bhp.", boss.cur_hp);
    write_label_and_value(header, &start_index, "/", boss.max_HP);
    write_label_and_value(header, &start_index, "Bsh.", boss.shield);
    write_label_and_value(header, &start_index, "By.", boss.y_pos);
    write_label_and_value(header, &start_index, "Bx.", boss.x_pos);
    write_label_and_value(header, &start_index, "Bydir.", boss.y_dir);
    write_label_and_value(header, &start_index, "Bxdir.", boss.y_dir);
    write_label_and_value(header, &start_index, "B_Atk.", boss.dmg);
    write_label_and_value(header, &start_index, boss_act[boss.action], '\0');
    write_label_and_value(header, &start_index, " tm.", boss.time_in_state);
    add_to_canv(header, canv, 0, 0);
    if (plr.hp <= 0) {
      plr.hp = 0;
      int x_size = CANV_W / 2, y_size = CANV_H / 2;
      char *msg = init_blank_canv(x_size, y_size, 1, '#');
      write_on_canv(msg, x_size / 2 - 7, 2, "GAME OVER \n \n SCORE: %d\n BEST: %d\n \n \n- CONTINUE (r)\n \n- QUIT (q)\n \n- SHOW_STATS (s)\n \n- PARAM (p)", game.score, game.max_score);
      add_to_canv(msg, canv, (CANV_W / 2) - x_size / 2, (CANV_H / 2) - y_size / 2);
      system("clear");
      print_canvas(canv, &game);
      int inp = -1;
      while (inp == -1) {
        inp = getchar();
        if (inp == 'r') {
          plr.hp = PLR_MAX_HP;
        } else if (inp == 'q') {
          system("clear");
          break;
        }
      }
    }
    print_canvas(canv, &game);
  }

  free(mobs);
  free(proj);
  free(canv);
  // system("clear");

  fopen(SAVEFILE, "w"); // Open file in write mode
  if (file != NULL) {
    if (game.score > game.max_score) {
      game.max_score = game.score; // Update max_score with the new high score
    }
    fprintf(file, "%ld", game.max_score); // Write max_score to the file
    fclose(file);
  } else {
    printf("Error opening file for writing.\n");
  }
  return 0;
}

// Function definitions

void initialize_game(GAME *game, SHIP *plr, LOOT *loots, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars) {
  game->loot_chance = STRT_LOOT_CHNC;
  game->level = game->score = game->max_score = game->prj_qual = 0;
  game->lv_choices = 3, game->attr = 2;
  game->gl_x_pos = 0, game->gl_y_pos = 0, game->prj_col_index = -1, game->plr_col_index = -1;
  game->boss_mode = 0, game->is_in_boss_fight = 0;
  game->mv_type = normal, game->atk_type = press, game->allowed_dir = all;

  plr->x_pos = PLR_CENTER_X + 5;
  plr->y_pos = CANV_H + SHIP_H + 2;
  plr->hp = PLR_MAX_HP;
  plr->hurt_timer = plr->shield = plr->weap = 0, plr->atk_pow = 1, plr->atk_speed = 10, plr->atk_am = 1;
  plr->speed = 1;
  plr->atk_reload = 1;
  plr->dir = up;
  plr->lk_dir = plr->dir;

  for (int i = 0; i < MOB_AM; i++) {
    int x, y;
    do {
      x = C_WIDTH * (rand() % CELL_SIZE) + 2;
      y = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
    } while (is_position_o__upied(mobs, i, x, y));
    mobs[i].x_pos = x, mobs[i].y_pos = y;
    mobs[i].shield = mobs[i].weap = mobs[i].hurt_timer = 0, mobs[i].hp = MOB_MAX_HP, mobs[i].speed = 1;
    mobs[i].dir = down;
  }
  for (int i = 0; i < PROJ_BUFFER; i++)
    proj[i].x_pos = proj[i].y_pos = -1;
  for (int i = 0; i < PRJ_MOB_BUFFER; i++)
    mobprj[i].x_pos = mobprj[i].y_pos = -1;
  for (int i = 0; i < LOOT_BUFFER; i++)
    loots[i].x_pos = loots[i].y_pos = -1;
}

int handle_input(GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mob_prj, int *has_moved, int time) {
  if (game->mv_type == cutscene) return (1);
  int c = -1;
  c = getchar();
  if (c != EOF || game->atk_type == autom) {
    if (game->mv_type != on_grid || !(*has_moved)) {
      if (c == 'a' && plr->x_pos > 2 && is_dir_allowed(left, game->allowed_dir))
        plr->dir = left, *has_moved = (CANV_W / CELL_SIZE) / 2;
      else if (c == 'd' && plr->x_pos < CANV_W - (CELL_SIZE - 5 + SHIP_W * 2) && is_dir_allowed(right, game->allowed_dir))
        plr->dir = right, *has_moved = (CANV_W / CELL_SIZE) / 2;
      else if (c == 'w' && plr->y_pos > 2 && is_dir_allowed(up, game->allowed_dir))
        plr->dir = up, *has_moved = CANV_H / CELL_SIZE;
      else if (c == 's' && plr->y_pos < CANV_H - SHIP_H && is_dir_allowed(down, game->allowed_dir))
        plr->dir = down, *has_moved = CANV_H / CELL_SIZE;
    }

    if (((c == ' ' && game->atk_type != disabled) || game->atk_type == autom) && !plr->atk_reload) {
      plr->atk_reload = plr->atk_speed;
      if (plr->weap > 0 && game->shots_fired > 0) game->shots_fired--;
      if (game->shots_fired == 0) {
        if (plr->weap > 0) plr->weap--;
        game->shots_fired = shot_lock;
      }
      DIR old_dir = plr->dir;
      int w_prj = plr->atk_am;
      if (game->mv_type == watch_up) plr->dir = up;
      for (int i = 0; i < PROJ_BUFFER; i++) {
        if (w_prj <= 0) break;
        if (proj[i].col_timer || (proj[i].x_pos != -1 && proj[i].y_pos != -1)) continue;
        DIR dir = determine_direction(w_prj, plr->atk_am, *plr);
        if (proj[i].x_pos == -1) {
          proj[i].x_pos = dir == up || dir == down ? plr->x_pos + SHIP_W / 2 : dir == left || dir == up_left ? plr->x_pos - 3 : plr->x_pos + SHIP_W + 3;
          proj[i].y_pos = dir == up || dir == up_left || dir == up_right         ? plr->y_pos - 1
                          : dir == down || dir == down_left || dir == down_right ? plr->y_pos + SHIP_H / 2
                          : dir == left || dir == right                          ? plr->y_pos + 1
                                                                                 : plr->y_pos - 1;
          proj[i].dir = dir;
          w_prj--;
          if (w_prj < 1) break;
          proj[i].x_pos -= plr->atk_am;
          for (int x = 0; x < plr->atk_am; x++) {
            if (w_prj <= 0) break;
            if (i + x > PROJ_BUFFER - 1) break;
            if (proj[i + x].x_pos == -1 && proj[i + x].y_pos == -1) continue;
            proj[i + x].dir = dir;
            proj[i + x].x_pos = proj[i].x_pos + (dir == up || dir == down ? 1 : 0);
            proj[i + x].y_pos = proj[i].y_pos + (dir == left || dir == right ? 1 : 0);
            w_prj--;
          }
        }
      }

      plr->dir = old_dir;

    } else if (c == 'q') {
      exit(0);
    }
  }
  return (c);
}

void update_game_state(GAME *game, SHIP *mobs, BOSS *boss, PROJ *proj, PROJ *mobprj, SHIP *plr, LOOT *loots, int has_moved, int time, int *wind, char *canv) {
  // Update player position
  if (plr->atk_reload > 0) plr->atk_reload--;
  int min_lv = game->level / 5 > 8 ? 8 : game->level / 5;

  if (has_moved && game->mv_type == on_grid) {
    plr->x_pos += plr->dir == right ? 2 : plr->dir == left ? -2 : 0;
    plr->y_pos += plr->dir == down ? 1 : plr->dir == up ? -1 : 0;
  } else if (game->mv_type == watch_up || game->mv_type == watch_up || game->mv_type == cutscene || game->mv_type == normal) {
    if (plr->x_pos != PLR_CENTER_X) plr->x_pos += (plr->x_pos < PLR_CENTER_X + 1 ? 1 : -1);
    if (plr->y_pos != PLR_CENTER_Y) plr->y_pos += (plr->y_pos < PLR_CENTER_Y ? 1 : -1);
  }
  if (game->mv_type == cutscene) plr->dir = up;
  game->gl_x_pos += plr->dir == right ? plr->speed : plr->dir == left ? -plr->speed : 0;
  game->gl_y_pos += plr->dir == down ? plr->speed : plr->dir == up ? -plr->speed : 0;

  if (game->is_in_boss_fight) {
    DIR pdir = plr->dir;
    char *boss_content = NULL;
    char *boss_shield = NULL;

    // DIE
    if (boss->action == die) {
      boss->x_pos += boss->x_pos < CENTER_X(boss->width) ? 1 : 0;
      if (boss->x_pos > CANV_W / 2 - boss->width / 2) boss->cur_hp = 0;
      game->boss_mode = 2;
      boss->time_in_state--;
      for (int i = 0; i < PROJ_BUFFER; i++) {
        if (proj[i].x_pos == -1 && proj[i].y_pos == -1) {
          mobprj[i].x_pos = rand_range(boss->x_pos, boss->x_pos + boss->width + 1);
          mobprj[i].y_pos = rand_range(boss->y_pos, boss->y_pos + boss->height + 1);
          mobprj[i].dir = rand_range(2, 9);
          if (mobprj[i].dir == none) mobprj[i].dir = left;
          break;
        }
      }
      if (boss->time_in_state <= 100) {
        game->boss_mode = 0, game->mv_type = normal, game->atk_type = press, game->allowed_dir = all;
        boss->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        if (time % 2 == 0) boss->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
        if (boss->time_in_state <= 0) {
          game->is_in_boss_fight = 0;
          free(boss->content);
        }
      }

      if (time % 3 == 0) {
        int ind = rand_range(1, strlen(boss->content) - 1);
        if (boss->content[ind] != '\n') boss->content[ind] = EXPL_START;
        boss_content = strdup(boss->content);
      }

    }
    // ENTRANCE
    else if (boss->action == entrance) {
      boss->time_in_state--;
      game->boss_mode = 2;
      game->mv_type = cutscene;

      if (boss->time_in_state <= 0) {
        boss->action = shield;
        game->boss_mode = 1;
        game->mv_type = watch_up;
        boss->time_in_state = 50;
        game->boss_mode = 1;
      }
      if (time % 5 == 0 && boss->y_pos < 12) boss->y_pos++;
      if (boss_content != NULL) free(boss_content);
      boss_content = strdup(boss->content);
    }
    //      REPOSITION
    else if (boss->action == reposition) {
      if (time % 5 == 0) {
        boss->x_dir = boss->x_pos < 3 ? 1 : boss->x_pos + boss->width > CANV_W - 3 ? -1 : 0;
        boss->y_dir = boss->y_pos < 6 ? 1 : boss->y_pos > 12 ? -1 : 0;
        boss->y_pos += boss->y_pos > BOSS_CENTER_Y ? -1 : boss->y_pos < BOSS_CENTER_Y ? 1 : 0;
        plr->x_pos += (plr->x_pos > game->plr_x_centr ? -1 : plr->x_pos < game->plr_y_centr ? 1 : 0);
        plr->y_pos += (plr->y_pos > game->plr_y_centr ? -1 : plr->y_pos < game->plr_y_centr ? 1 : 0);
      }
      if (boss->y_pos == BOSS_CENTER_Y) {
        boss->action = idle;
        plr->dir = up;
      }
      if (boss_content != NULL) free(boss_content);
      boss_content = strdup(boss->content);
    } else {
      int hp_perc = PERCENTILE(boss->cur_hp, boss->max_HP);
      BOSS_ACTIONS prev_action = boss->action;
      int limit = 0;

      if (boss->time_in_state <= 0) {
        game->allowed_dir = all, game->mv_type = normal, game->atk_type = autom;
        boss->time_in_state = 50;
        boss->x_dir = boss->y_dir = 0;

        if (boss->action == hurt)
          boss->action = call;
        else if (boss->cur_hp == boss->max_HP / 10)
          boss->action = regen, boss->time_in_state = 200;
        else if (boss->action == SP_attack2 || boss->action == retreat) {
          boss->action = reposition;

        } else if (boss->action == idle) {
          int r = rand() % 8;
          if (r == 0) {
            do
              boss->speATK_hole = (plr->x_pos + SHIP_W / 2) + (SHIP_W)*rand_range(-10, 10);
            while (boss->speATK_hole > CANV_W - 50 || boss->speATK_hole < 50);
            boss->action = SP_attack, boss->time_in_state = 600;
          } else if (r == 1) {
            boss->action = SP_attack2, boss->y_dir = 1, boss->cur_hp -= 20, boss->time_in_state = 150;
          } else if (r <= 4)
            boss->action = retreat, boss->time_in_state = 200;
          else
            boss->action = attack;
        } else
          boss->action = idle;
      }

      // handle plr_mv
      if (game->mv_type == normal || game->mv_type == watch_up) {
        if (plr->dir == left || plr->dir == right) {
          if (plr->dir == left && boss->x_pos <= CANV_W - boss->width / 2) boss->x_pos += 1;
          if (plr->dir == right && boss->x_pos >= -boss->width / 2) boss->x_pos -= 1;
        }
      }

      //      SP ATTACK
      if (boss->action == SP_attack) {
        if (plr->y_pos <= boss->y_pos + boss->height + 1 && boss->time_in_state > 60) {
          boss->time_in_state = 60;
        }

        int bul_amnt = CANV_W / 2;
        game->mv_type = on_grid;
        game->atk_type = press;
        if (boss->time_in_state >= 60 && time % (boss->atk_spd) == 0) {
          int x_dist = (CANV_W) / bul_amnt;
          if (boss->time_in_state <= 100) {
            boss->x_dir = 0;
            boss->hole_size = SHIP_W + 2;
          } else {
            if (time % 5 == 0) {
              int r_dir = rand() % 4;
              boss->x_dir = r_dir == 0 ? 1 : r_dir == 1 ? -1 : 0;
            }
            boss->hole_size = boss->x_dir == 0 ? (SHIP_W + 3) : 3 * (SHIP_W + 1);
            if (boss->speATK_hole < 10 || boss->speATK_hole > CANV_W - 10)
              boss->x_dir = boss->speATK_hole < 30 ? 1 : boss->speATK_hole > CANV_W - 30 ? -1 : boss->x_dir;
            else if (boss->speATK_hole > CANV_W - 30)
              boss->x_dir = -1;
            boss->speATK_hole += boss->x_dir * 8;
          }
          if (boss->y_pos < CANV_H / 2 - 3) boss->y_pos += boss->x_dir;

          if (boss->time_in_state > 130) {
            for (int i = 0; i < PRJ_MOB_BUFFER; i++) {
              if (mobprj[i].x_pos != -1 && mobprj[i].y_pos != -1) continue;
              if (boss->time_in_state == 50) {
                mobprj[i].x_pos = mobprj[i].y_pos = -1;
                continue;
              }
              mobprj[i].x_pos = 3 + (x_dist * bul_amnt);
              if (mobprj[i].x_pos >= boss->speATK_hole - boss->hole_size && mobprj[i].x_pos <= boss->speATK_hole + boss->hole_size) {
                mobprj[i].x_pos = mobprj[i].y_pos = -1;
                bul_amnt--;
                continue;
              }
              mobprj[i].y_pos = boss->y_pos + boss->height + 1;
              mobprj[i].dir = down;
              bul_amnt--;
              if (bul_amnt == 0) break;
            }
          }
        }
      }
      // SP_ATK 2
      else if (boss->action == SP_attack2) {
        game->mv_type = on_grid, game->atk_type = disabled, game->allowed_dir = down;
        int y_dist = (abs)(boss->y_pos - plr->y_pos);
        if (y_dist > 7) y_dist = 7;
        if (y_dist < 2) y_dist = 2;
        if (time % y_dist == 0) {
          plr->y_pos--;
          boss->y_dir = boss->y_pos > CANV_H - boss->height - 10 ? -1 : boss->y_pos < CANV_H - boss->height - 25 ? 1 : boss->y_dir;
          boss->y_pos += boss->y_dir;
          boss->x_pos = plr->x_pos + SHIP_W / 2 - boss->width / 2;
          if (abs(boss->y_pos - plr->y_pos) < 10) {
            if (!plr->hurt_timer) {
              if (plr->shield)
                plr->shield -= 10;
              else
                plr->hp -= 1;
              game->plr_col_index = -2;
              plr->hurt_timer = HURT_DUR;
            }
          }
        }
        if (boss->y_pos > CANV_H - boss->height - SHIP_H - 6) boss->time_in_state = 0;
        if (boss->time_in_state == 0) plr->dir = up;
      }

      //      ATTACK
      else if (boss->action == attack) {
        game->mv_type = watch_up, game->atk_type = autom;
        if (time % boss->atk_spd == 0) {
          int bul_amnt = rand_range(3, 16 - (hp_perc / 10));
          int avoid = rand_range(1, 5);
          int center = CENTER_X(boss->width) - (6 * bul_amnt);
          for (int i = 0; i < PRJ_MOB_BUFFER; i++) {
            if (mobprj[i].x_pos != -1 && mobprj[i].y_pos != -1) continue;
            if (i == avoid) {
              bul_amnt--;
              continue;
            }
            mobprj[i].x_pos = center + (12 * bul_amnt);
            mobprj[i].y_pos = boss->y_pos + boss->height + 1;
            mobprj[i].dir = down;
            bul_amnt--;
            if (bul_amnt == 0) break;
          }
        }
      }
      // CALL
      else if (boss->action == call) {
        for (int i = 0; i < MOB_AM; i++) {
          if (mobs[i].x_pos != -1 && mobs[i].y_pos != -1) continue;
          mobs[i].x_pos = CANV_W / 2 - SHIP_W / 2, mobs[i].y_pos = boss->y_pos + boss->height + 1;
          mobs[i].speed = 1, mobs[i].dir = down;
          break;
        }
      }
      //      IDLE
      else if (boss->action == idle) {
        game->mv_type = watch_up, game->atk_type = autom;
        if (time % boss->speed == 0) {
          boss->x_dir = boss->x_pos < 3 ? 1 : boss->x_pos + boss->width > CANV_W - 3 ? -1 : boss->x_dir;
          boss->y_dir = boss->y_pos < 6 ? 1 : boss->y_pos > 12 ? -1 : boss->y_dir;
          boss->x_pos += boss->x_dir, boss->y_pos += boss->y_dir;
        }
      }
      //      HURT
      else if (boss->action == hurt) {
        game->mv_type = watch_up, game->atk_type = autom;
        boss->action = shield;
        if (boss->x_pos > CANV_W / 2 - boss->width / 2)
          boss->x_pos--;
        else if (boss->x_pos < CANV_W / 2 - boss->width / 2)
          boss->x_pos++;
      }
      //  SHIELD
      else if (boss->action == shield) {
        boss->shield++;
        boss->time_in_state = 2;
        if (boss->shield == boss->max_shield) boss->time_in_state = 0;
      }
      //  RETREAT
      else if (boss->action == retreat) {
        game->mv_type = on_grid, game->atk_type = press, game->allowed_dir = hor;
        // HANDLE_COLISION
        if (abs(boss->x_pos - plr->x_pos) < 10 && abs(boss->y_pos - plr->y_pos) < 10) {
          if (!plr->hurt_timer) {
            if (time % 10 == 0) {
              if (plr->shield)
                plr->shield -= 10;
              else
                plr->hp -= 1;
            }
            plr->hurt_timer = HURT_DUR;
          }
        }
        // MOVE UP BEFORE 150
        if (boss->time_in_state > 150 && boss->y_pos > -(boss->height * 4)) {
          boss->y_pos--;
          int center_pos = boss->x_pos + boss->width / 2;
          int plr_center = plr->x_pos;
          boss->x_pos += (center_pos < plr_center ? 2 : center_pos > plr_center ? -2 : 0);
        }
        // QUICKLY MOVE DOWN DURING ATTACK
        if (boss->time_in_state > 100 && boss->time_in_state <= 150 && boss->y_pos < plr->y_pos) {

          char *mark = strdup(QUESTIONMARK);
          add_to_canv(mark, canv, boss->x_pos + boss->width / 2, CANV_H - 10);
          free(mark);
          boss->y_pos += 3;
        }
        // MOVE BACK UP AFTER ATTACK
        else if (boss->time_in_state < 100) {
          boss->y_dir = boss->y_pos > BOSS_CENTER_Y ? -2 : boss->y_pos < BOSS_CENTER_Y ? 1 : 0;
          boss->x_dir = boss->x_pos > (CANV_W / 2 - boss->width / 2) ? -1 : boss->x_pos < (CANV_W / 2 - boss->width / 2) ? 1 : 0;
          boss->y_pos += boss->y_dir, boss->x_pos += boss->x_dir;
          if (boss->y_pos <= BOSS_CENTER_Y) boss->time_in_state = 0;
        }
      }
      boss->time_in_state--;
      int has_losthp = 0;
      // HANDLE HEALTH
      if (boss->prev_HP != boss->cur_hp) {
        if (boss->prev_HP > boss->cur_hp) {

          has_losthp = 1;
          if (rand() % boss->cur_hp / 10 <= 0) {
            int index = rand_range((boss->width + 1) * 2, (boss->width + 1) * (boss->height - 1));
            if (boss->content[index] != '\n') boss->content[index] = PRJ_COL;
          }
          if (boss->cur_hp == boss->max_HP / 4 || boss->cur_hp == boss->max_HP - boss->max_HP / 3) {
            boss->action = hurt;
            boss->speed -= boss->speed - 1 < 0 ? 0 : 1;
            boss->atk_spd -= boss->atk_spd - 1 < 0 ? 0 : 1;
            boss->dmg++;
            boss->hole_size -= boss->hole_size < SHIP_W + 2 ? 0 : 1;
            boss->time_in_state = 10;
          }
          if (boss->cur_hp <= 0) {
            boss->action = die;
            boss->time_in_state = 200;
          }
        }
        boss->prev_HP = boss->cur_hp;
      }
      boss_content = strdup(boss->content);
      if (has_losthp) print_in_red(boss_content);
    }
    int show_boss = 1;
    if (boss->prv_shield != boss->shield) {
      if (boss->prv_shield > boss->shield)
        show_boss = 0;
      else {
      }
      boss->prv_shield = boss->shield;
    }
    if (boss_content != NULL && show_boss) {
      if (boss->shield) {
        int active_shields = (boss->shield * boss->shld_ic_count) / 100;
        int incr = 0;
        for (int i = 0; i < strlen(boss->content); i++) {
          if (boss->content[i] == SHLD_OFF_MOB_IC) {
            if (incr < active_shields) {
              boss_content[i] = SHLD_ON_MOB_IC;
              incr++;
            } else {
              break;
            }
          }
        }
      }
      add_to_canv(boss_content, canv, boss->x_pos, boss->y_pos);
      free(boss_content);
    }
  }

  // UPDATE LOOTS
  for (int i = 0; i < LOOT_BUFFER; i++) {
    if (loots[i].x_pos == -1 && loots[i].y_pos == -1) continue;

    if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
      loots[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
      loots[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    }

    add_to_canv(&loots[i].content, canv, loots[i].x_pos, loots[i].y_pos);
    int x_diff = loots[i].x_pos - plr->x_pos;
    int y_diff = loots[i].y_pos - plr->y_pos;
    if (abs(x_diff) < game->attr && abs(y_diff) < game->attr) {
      loots[i].x_pos = -1;
      loots[i].y_pos = -1;
      if (loots[i].type == MOR_HP)
        plr->hp++;
      else if (loots[i].type == MOR_SHLD)
        plr->shield += 2;
      else if (loots[i].type == MOR_SC)
        game->score += 5000;
      else if (loots[i].type == MOR_ATTR)
        game->attr += 2;
      else if (loots[i].type == MOR_LOOT)
        game->loot_chance += 2;
      else if (loots[i].type == MOR_PRJ) {
        plr->weap++;
        game->shots_fired += shot_lock;
      }
    }
  }

  // Update mob positions
  if (!game->is_in_boss_fight) {
    for (int i = 0; i < MOB_AM - 1; i++) {
      if (time % (BOSS_INTERVAL - 10) == 0) mobs[i].hp = 0;

      if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        mobs[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        mobs[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }

      if (mobs[i].hp <= 0 && mobs[i].hurt_timer == 0) mobs[i].hurt_timer += 10;

      if (mobs[i].hurt_timer >= 1) {
        if (mobs[i].hurt_timer == 1) {
          if (rand() % 100 <= BASE_LOOT_CHNC + game->loot_chance) {
            for (int x = 0; x < LOOT_BUFFER; x++) {
              if (loots[x].x_pos == -1 && loots[x].y_pos == -1) {
                loots[x].x_pos = mobs[i].x_pos;
                loots[x].y_pos = mobs[i].y_pos;
                loots[x].type = rand() % 7;
                loots[x].content = loots[x].type == MOR_SHLD   ? SHIELD_IC
                                   : loots[x].type == MOR_SC   ? COIN_IC
                                   : loots[x].type == MOR_PRJ  ? WEAP_IC
                                   : loots[x].type == MOR_HP   ? HEART_IC
                                   : loots[x].type == MOR_ATTR ? ATTR_IC
                                   : loots[x].type == MOR_LOOT ? LUCK_IC
                                                               : LUCK_IC;
                break;
              }
            }
          }
          mobs[i].y_pos = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
          mobs[i].x_pos = C_WIDTH * (rand() % CELL_SIZE) + 2;
          mobs[i].hp = MOB_MAX_HP + game->level / 5;
          mobs[i].hurt_timer = 0;
        } else {
          mobs[i].hurt_timer--;
          game->score += 10;
        }
        continue;
      }

      if (plr->hurt_timer == 0) {
        if (abs(mobs[i].y_pos - plr->y_pos) < 2 && abs(mobs[i].x_pos - plr->x_pos) < 2) {
          if (plr->shield) {
            plr->shield--;
            mobs[i].hp -= 2;
          } else
            plr->hp--;
          game->plr_col_index = i;
          plr->hurt_timer = HURT_DUR;
        }
      }
      int t = 10 - (game->level / 5) < 3 ? 3 : 10 - (game->level / 5);
      if (time % t == 0) {
        if (game->level > 1) {
          int distance_threshold = game->level * 2 > 40 ? 40 : game->level * 2;
          int x_diff = mobs[i].x_pos - plr->x_pos;
          int y_diff = mobs[i].y_pos - plr->y_pos;
          if (abs(x_diff) < distance_threshold && abs(y_diff) < distance_threshold) {
            if (x_diff > 0)
              mobs[i].dir = left;
            else if (x_diff < 0)
              mobs[i].dir = right;
            if (y_diff > 0)
              mobs[i].dir = up;
            else if (y_diff < 0)
              mobs[i].dir = down;
          } else {
            int rand_chance = 50 - game->level < 5 ? 5 : 50 - game->level;
            if (rand() % rand_chance == 0) mobs[i].dir = rand() % 4;
          }
        }
        int rand_chance = 50 - game->level < 5 ? 5 : 50 - game->level;
        if (rand() % rand_chance == 0) {
          for (int j = 0; j < PRJ_MOB_BUFFER; j++) {
            if (mobprj[j].x_pos == -1 && mobprj[j].y_pos == -1) {
              DIR d = mobs[i].dir;
              mobprj[j].x_pos = d == up || d == down ? mobs[i].x_pos + SHIP_W / 2 : d == left || d == up_left ? mobs[i].x_pos - 3 : mobs[i].x_pos + SHIP_W + 3;
              mobprj[j].y_pos = d == up || d == up_left || d == up_right         ? mobs[i].y_pos - 1
                                : d == down || d == down_left || d == down_right ? mobs[i].y_pos + SHIP_H / 2
                                : d == left || d == right                        ? mobs[i].y_pos + 1
                                                                                 : mobs[i].y_pos - 1;
              mobprj[j].dir = mobs[i].dir;
              break;
            }
          }
        }
        mobs[i].y_pos += mobs[i].dir == down ? 1 : mobs[i].dir == up ? -1 : 0;
        mobs[i].x_pos += mobs[i].dir == right ? 2 : mobs[i].dir == left ? -2 : 0;
        if (mobs[i].y_pos >= CANV_H + 10 || mobs[i].y_pos + SHIP_H < -CANV_H - 10 || mobs[i].x_pos >= CANV_W + 10 || mobs[i].x_pos + SHIP_W < -CANV_W - 10) {
          DIR dir = rand() % 5;
          mobs[i].dir = dir;
          switch (dir) {
            case down:
              mobs[i].y_pos = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
              mobs[i].x_pos = C_WIDTH * (rand() % CELL_SIZE) + 2;
              break;
            case left:
              mobs[i].y_pos = rand_range(2, CANV_H - 2);
              mobs[i].x_pos = CANV_W + (SHIP_W + 1) + ((CANV_W / CELL_SIZE) * rand_range(1, 5));
              break;
            case right:
              mobs[i].y_pos = rand_range(2, CANV_H - 2);
              mobs[i].x_pos = -(SHIP_W - 1) - ((CANV_W / CELL_SIZE) * rand_range(1, 5));
              break;
            case up:
              mobs[i].y_pos = CANV_H + ((CANV_H / CELL_SIZE) * rand_range(1, 5));
              mobs[i].x_pos = C_WIDTH * (rand() % CELL_SIZE) + 2;
              break;
            default:
              mobs[i].y_pos = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
              mobs[i].x_pos = C_WIDTH * (rand() % CELL_SIZE) + 2;
              break;
          }
        }
      }
    }
  }

  // Update projectile positions
  for (int i = 0; i < PROJ_BUFFER - 1; i++) {
    if (proj[i].y_pos <= -1 || proj[i].x_pos <= -1 || proj[i].y_pos >= CANV_H || proj[i].x_pos >= CANV_W) {
      proj[i].x_pos = proj[i].y_pos = -1;
      continue;
    }
    int index = proj[i].x_pos + ((CANV_W + 1) * proj[i].y_pos), bull_collision = 0;
    if (proj[i].col_timer) {
      proj[i].col_timer--;
      if (proj[i].col_timer == 0) {
        proj[i].x_pos = proj[i].y_pos = -1;
        continue;
      }
      if (time % 5 == 0) {
        proj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        proj[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
      canv[index] = PRJ_COL;
      continue;
    }
    if (canv[index]) {
      if (canv[index != ' ' && canv[index] != '_']) {
        if (abs(boss->y_pos - proj[i].y_pos) < boss->height && abs((boss->x_pos + (boss->width / 2)) - proj[i].x_pos) < boss->width / 2) {
          int dmg = plr->atk_pow + (boss->action == SP_attack2 || boss->action == SP_attack ? 10 : 0);
          if (boss->shield > 0) {
            boss->shield -= dmg;
          } else {
            boss->cur_hp -= dmg;
            boss->shield = 0;
          }
          game->prj_col_index = -2;
          proj[i].col_timer = 15;
        } else {
          for (int j = 0; j < MOB_AM; j++) {
            int mob_index = mobs[j].x_pos + ((CANV_W + 1) * mobs[j].y_pos);
            if (abs(mobs[j].y_pos - proj[i].y_pos) < 5 && abs(mobs[j].x_pos - proj[i].x_pos) < 5) {
              mobs[j].hp -= plr->atk_pow;
              proj[i].col_timer = 15;
              break;
            }
          }
        }
      }
      canv[index] = proj[i].dir == up || proj[i].dir == down              ? PRJ_VER
                    : proj[i].dir == left || proj[i].dir == right         ? PRJ_HOR
                    : proj[i].dir == up_left || proj[i].dir == down_right ? PRJ_DIAG_L
                                                                          : PRJ_DIAG_R;
    }

    if (time % 8 == 0) proj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
    if (time % 4 == 0) proj[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    proj[i].x_pos += proj[i].dir == left || proj[i].dir == up_left || proj[i].dir == down_left ? -2 : proj[i].dir == up || proj[i].dir == down ? 0 : 2;
    proj[i].y_pos += proj[i].dir == up || proj[i].dir == up_left || proj[i].dir == up_right ? -1 : proj[i].dir == left || proj[i].dir == right ? 0 : 1;
  }

  // Update MOB PROJ
  for (int i = 0; i < PRJ_MOB_BUFFER - 1; i++) {
    int x = mobprj[i].x_pos, y = mobprj[i].y_pos;
    if (y <= -1 || x <= -1 || y >= CANV_H || x >= CANV_W) {
      mobprj[i].x_pos = mobprj[i].y_pos = -1;
      continue;
    }
    if (game->mv_type == cutscene) mobprj[i].x_pos = mobprj[i].y_pos = -1;
    DIR d = mobprj[i].dir;
    int index = x + ((CANV_W + 1) * y);
    if (index > 0 && index < CANV_SIZE) {

      if (abs(plr->y_pos - mobprj[i].y_pos) < 2 && abs(plr->x_pos - mobprj[i].x_pos) < 2 && plr->hurt_timer == 0) {
        mobprj[i].x_pos = mobprj[i].y_pos = -1;
        if (plr->shield)
          plr->shield--;
        else
          plr->hp--;
        game->prj_col_index = game->is_in_boss_fight ? -2 : i;
        plr->hurt_timer += HURT_DUR;
        continue;
      }
      if (canv[index] == ' ') canv[index] = d == up || d == down ? MPRJ_VER : d == left || d == right ? MPRJ_HOR : d == up_left || d == down_right ? MPRJ_DIAG_L : MPRJ_DIAG_R;
    }
    if (time % (game->is_in_boss_fight && game->mv_type == on_grid ? 4 : 2) == 0) {
      mobprj[i].x_pos += d == left || d == up_left || d == down_left ? -2 : d == up || d == down ? 0 : 2;
      mobprj[i].y_pos += d == up || d == up_left || d == up_right ? -1 : d == left || d == right ? 0 : 1;
      if (mobprj[i].dir == left || mobprj[i].dir == right) mobprj[i].y_pos += plr->dir == up ? 1 : 0;
    }
    if (game->mv_type != on_grid) mobprj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
  }

  // Update wind effects
  // Update wind effects
  for (int j = 0; j < (WIND_BUFFER - 1); j++) {
    DIR dir;
    int y_pos, x_pos, x_size, y_size;
    if (j >= PLR_WIND_MIN && j <= PLR_WIND_MAX)
      dir = plr->dir, y_pos = plr->y_pos, x_pos = plr->x_pos, x_size = SHIP_W, y_size = SHIP_H;
    else if (j >= BOSS_WIND_MIN && j <= BOSS_WIND_MAX && game->is_in_boss_fight)
      dir = down, y_pos = boss->y_pos, x_pos = boss->x_pos, x_size = boss->width, y_size = boss->height;
    else
      break;

    int new_rand_index;
    if (dir == up)
      new_rand_index = x_pos + 1 + ((y_pos + 1 + x_size) * (CANV_W + 1)) + (rand() % 2);
    else if (dir == right)
      new_rand_index = x_pos - 3 + ((CANV_W + 1) * (y_pos + rand() % x_size / 2));
    else if (dir == left)
      new_rand_index = x_pos + x_size + 3 + ((CANV_W + 1) * (y_pos + rand() % 2));
    else if (dir == down) {
      if (j >= BOSS_WIND_MIN)
        new_rand_index = x_pos + 6 + ((y_pos - 1) * (CANV_W + 1)) + (rand() % x_size / 4);
      else
        new_rand_index = x_pos + 2 + ((y_pos) * (CANV_W + 1)) + (rand() % x_size - 1);
    }
    if (wind[j] < 0 || wind[j] > CANV_SIZE || canv[wind[j]] != ' ' || rand() % 2 == 0) wind[j] = new_rand_index;
    canv[wind[j]] = rand() % 2 == 0 ? (dir == up || dir == down ? WIND_VER : WIND_HOR) : WIND_DOT;
    wind[j] += dir == up ? CANV_W + 1 : dir == down ? -CANV_W - 1 : dir == left ? 2 : dir == right ? -2 : 1;
    if (wind[j] > CANV_SIZE) wind[j] = new_rand_index;
  }
}

void render_game(GAME game, BOSS boss, SHIP *plr, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *wind, char *canv) {

  // Render player ship
  char *plr_txt = draw_triangle('*', game.mv_type == watch_up ? up : plr->dir, SHIP_W, SHIP_H, 2);
  if (plr->shield) {
    char *circ = draw_circle(SHIP_W + 6, SHIP_W + 6, plr->shield <= 2 ? PLR_SHLD_OUTL : plr->shield <= 4 ? PLR_SHLD_OUTL1 : PLR_SHLD_OUTL2, (float)plr->shield / 6);
    add_to_canv(circ, canv, plr->x_pos - 3, plr->y_pos - 4);
    free(circ);
  }
  if (plr->hurt_timer > 0) {
    if (plr->hurt_timer % 3 == 0) add_to_canv(plr_txt, canv, plr->x_pos, plr->y_pos);
    plr->hurt_timer--;
  } else
    add_to_canv(plr_txt, canv, plr->x_pos, plr->y_pos);
  free(plr_txt);

  // Render mobs
  if (!game.is_in_boss_fight) {
    for (int i = 0; i < MOB_AM - 1; i++) {
      SHIP mob = mobs[i];
      char *mob_tri = draw_triangle('*', mob.dir, SHIP_W, SHIP_H, 1);
      if (mob.hurt_timer > 0) {
        int i = 0;
        while (mob_tri[i]) {
          if (mob_tri[i] != ' ' && mob_tri[i] != '\n') mob_tri[i] = EXPL_START;
          i++;
        }
      }
      if (mob.x_pos - SHIP_W > 0 && mob.x_pos < CANV_W && mob.y_pos - SHIP_H < CANV_H && mob.y_pos < CANV_H) add_to_canv(mob_tri, canv, mob.x_pos, mob.y_pos);
      free(mob_tri);
    }
  }

  // Render projectiles and wind effects are handled in update_game_state()
}

int print_canvas(char *canv, GAME *game) {
  system("clear");
  char *mob_color1 = game->level < 5 ? BLUE : game->level < 10 ? GREEN : game->level < 15 ? YELLOW : game->level < 20 ? ORANGE : game->level < 25 ? RED : MAGENTA;
  char *mob_color2 = game->level < 5 ? LIGHT_BLUE : game->level < 10 ? DARK_GREEN : game->level < 15 ? PALE_YELLOW : game->level < 20 ? RED : game->level < 25 ? BROWN : CYAN;
  char *plr_color1 = BRIGHT_RED, *plr_color2 = CYAN;
  int i = 0;
  while (canv[i]) {
    char c = canv[i];
    if (c == '#')
      printf(" ");
    else if (c == COL_HOR || c == COL_VER)
      print_colored_text(NULL, canv[i] == COL_HOR ? '_' : '|', rand() % 4 == 0 ? MAGENTA : rand() % 4 == 0 ? BLUE : GREEN);
    else if (c == PRJ_HOR || c == PRJ_VER || c == PRJ_DIAG_L || c == PRJ_DIAG_R)
      print_colored_text(NULL, c == PRJ_HOR ? '_' : c == PRJ_VER ? '|' : c == PRJ_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? RED : ORANGE);
    else if (c == MPRJ_HOR || c == MPRJ_VER || c == MPRJ_DIAG_L || c == MPRJ_DIAG_R)
      print_colored_text(NULL, c == MPRJ_HOR ? '_' : c == MPRJ_VER ? game->is_in_boss_fight ? 'O' : '|' : c == MPRJ_DIAG_L ? '\\' : '/', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (c == WIND_DOT || c == WIND_VER || c == WIND_HOR)
      print_colored_text(NULL, c == WIND_DOT ? '.' : c == WIND_VER ? '|' : '-', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == CANV_VER || c == CANV_HOR)
      printf(c == CANV_VER ? "|" : "_");
    else if (c == PLR_SHLD_OUTL)
      print_colored_text(NULL, '.', rand() % 2 == 0 ? BLUE : LIGHT_BLUE);
    else if (c == STAR_IC || c == STAR_IC2)
      print_colored_text(NULL, c == STAR_IC ? '+' : '.', rand() % 2 == 0 ? YELLOW : PALE_YELLOW);
    else if (c == PLR_SHLD_OUTL1)
      print_colored_text(NULL, '.', rand() % 2 == 0 ? GREEN : DARK_GREEN);
    else if (c == PLR_SHLD_OUTL2)
      print_colored_text(NULL, '.', rand() % 2 == 0 ? RED : ORANGE);
    else if (c == SHLD_OFF_MOB_IC || c == SHLD_ON_MOB_IC)
      print_colored_text(NULL, c == SHLD_OFF_MOB_IC ? ' ' : '.', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == MOB_LBORD || c == MOB_RBORD || c == MOB_HOR || c == MOB_VER)
      print_colored_text(NULL, c == MOB_LBORD ? '/' : c == MOB_RBORD ? '\\' : c == MOB_HOR ? '_' : '|', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (c == SHP_LBORD || c == SHP_RBORD || c == SHP_HOR || c == SHP_VER)
      print_colored_text(NULL, c == SHP_LBORD ? '/' : c == SHP_RBORD ? '\\' : c == SHP_HOR ? '_' : '|', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (c == HEART_IC)
      print_colored_text(HEART_SMB, '\0', RED);
    else if (c == COIN_IC)
      print_colored_text(SCR_SMB, '\0', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == LUCK_IC)
      print_colored_text(LUCK_SMB, '\0', rand() % 2 == 0 ? BLUE : MAGENTA);
    else if (c == WEAP_IC)
      print_colored_text(PROJ_SMB, '\0', rand() % 2 == 0 ? BRIGHT_RED : YELLOW);
    else if (c == SHIELD_IC)
      print_colored_text(SHLD_SMB, '\0', rand() % 2 == 0 ? GREEN : DARK_GREEN);
    else if (c == ATTR_IC)
      print_colored_text(ATTR_SYMB, '\0', CYAN);
    else if (canv[i] == EXPL_START || canv[i + 1] == EXPL_START || canv[i - 1] == EXPL_START || canv[i + CANV_W + 1] == EXPL_START || canv[i - CANV_W - 1] == EXPL_START) {
      int r = rand_range(0, 4);
      print_colored_text(NULL, r == 0 ? '.' : r == 1 ? '/' : r == 2 ? '\\' : '*', rand() % 2 == 0 ? mob_color1 : mob_color2);
    } else if (c == PRJ_COL) {
      int r = rand_range(0, 4);
      print_colored_text(NULL, r == 0 ? '.' : r == 1 ? '\'' : r == 2 ? ',' : '*', rand() % 2 == 0 ? RED : ORANGE);
    } else if (c == RED_ICON)
      print_colored_text(NULL, '#', RED);

    else
      printf("%c", c);
    i++;
  }
  return 1;
}

void init_boss(GAME game, BOSS *boss) {
  boss->content = strdup(SHIELD_GFX);
  boss->height = get_height(boss->content), boss->width = get_width(boss->content);
  boss->cur_hp = 200, boss->max_HP = 200, boss->prev_HP = 200, boss->max_shield = 50, boss->prv_shield = 50;
  boss->speed = 8, boss->shield = 0, boss->dmg = 5, boss->atk_spd = 20, boss->speATK_hole = 0, boss->hole_xdir = 1, boss->hole_size = SHIP_H + 4;
  boss->x_dir = 1, boss->y_dir = 1;
  boss->x_pos = CANV_W / 2 - boss->width / 2, boss->y_pos = -boss->height + 1;
  boss->action = entrance, boss->time_in_state = 120;
  boss->shld_ic_count = 0;
  int i = 0;
  while (boss->content[i]) {
    if (boss->content[i++] == SHLD_OFF_MOB_IC) {
      boss->shld_ic_count++;
    }
  }
}