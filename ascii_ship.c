#include "header.h"

// Function prototypes
void initialize_game(GAME *game, SHIP *plr, LOOT *loots, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *stars);
int handle_input(GAME *game, SHIP *plr, int *has_moved, PROJ *proj, PROJ *mobprj);
void update_game_state(GAME *game, SHIP *plr, LOOT *loots, int has_moved, SHIP *mobs, PROJ *proj, PROJ *mobprj, int time, int *wind, char *canv);
void render_game(SHIP *plr, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *wind, char *canv);
int print_canvas(char *canv, GAME *game);

int main() {
  set_nonblocking_mode(1);
  int time = 0;
  int show_grid = 0;
  int has_moved = 0;

  // Allocate memory for mobs and projectiles
  SHIP *mobs = malloc(sizeof(SHIP) * MOB_AM);
  PROJ *proj = malloc(sizeof(PROJ) * PROJ_BUFFER);
  PROJ *proj_mob = malloc(sizeof(PROJ) * PRJ_MOB_BUFFER);

  LOOT *loots = malloc(sizeof(LOOT) * LOOT_BUFFER);

  int wind[WIND_AM];
  char *canv = NULL;
  char *header = NULL;
  int stars[STAR_BUFFER];

  // Initialize game entities
  SHIP plr;
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

    int lev = game.level;
    game.level = game.score / 10000;
    if (lev != game.level) {
      print_on_text(header, canv, 0, 0);

      char *upg_canv = init_blank_canv(UPG_CANV_W, UPG_CANV_H, 1, '#');
      char *msg = strdup(LV_MSG);
      print_on_text(msg, upg_canv, (UPG_CANV_W - get_width(msg)) / 2, 3);
      game.lv_choices = rand_range(1, 7);
      Loot_Type l_choice[game.lv_choices];
      int range = -1;
      int y_range = 0;
      for (int i = 0; i < game.lv_choices; i++) {
        range++;
        if (range > 2) {
          range = 0;
          y_range++;
        }

        int new_loot = rand() % LOOT_COUNT;
        do
          new_loot = rand() % LOOT_COUNT;
        while (contains(l_choice, i, new_loot));
        l_choice[i] = new_loot;

        int cell_x = 20; // Width of each cell
        int cell_y = 10; // Height of each cell

        char *choice_txt = init_blank_canv(cell_x, cell_y, 1, '#');
        char *elem = l_choice[i] == MOR_ATTR   ? magnet
                     : l_choice[i] == MOR_LOOT ? luck
                     : l_choice[i] == MOR_PRJ  ? gun_ic
                     : l_choice[i] == MOR_SHLD ? shield_ic
                     : l_choice[i] == MOR_HP   ? hart
                     : l_choice[i] == BET_PRJ  ? hart
                                               : hart;
        char *el = strdup(elem);
        print_on_text(el, choice_txt, (cell_x - get_width(el)) / 2, (cell_y - get_height(el)) / 2);
        free(el);

        // Calculate total width occupied by all cells in the row
        int total_row_width = (cell_x + cell_x / 3) * game.lv_choices - cell_x / 3;
        if (game.lv_choices > 3)
          total_row_width = (cell_x + cell_x / 3) * 3;
        // Center the starting position for the first cell
        int start_x = (UPG_CANV_W - total_row_width) / 2;
        int x_pos = start_x + (cell_x + cell_x / 3) * range;

        // Adjust y_pos calculation to center vertically while considering vertical padding
        int total_height = (cell_y + cell_y / 3) * (y_range + 1);
        int start_y = (UPG_CANV_H - total_height) / 2;
        int y_pos = start_y + (cell_y * 2) * y_range;

        print_on_text(choice_txt, upg_canv, x_pos, y_pos);
      }
      print_on_text(upg_canv, canv, (CANV_W - UPG_CANV_W) / 2, (CANV_H - UPG_CANV_H) / 2);

      int inp = -1;
      int is_paused = 1;
      while (is_paused) {
        usleep(7000);
        inp = getchar();
        print_canvas(canv, &game);
        if (inp == 10)
          is_paused = 0;
      }
    }

    if (plr.hp <= 0)
      break;

    int inp = handle_input(&game, &plr, &has_moved, proj, proj_mob);
    if (inp == 'm')
      show_grid = !show_grid;

    if (canv != NULL)
      free(canv);
    canv = init_blank_canv(CANV_W, CANV_H, 1, ' ');
    for (int i = 0; i < STAR_BUFFER - 1; i++) {
      if (canv[stars[i]] == ' ')
        canv[stars[i]] = '.';
      else
        stars[i] = rand_range(1, CANV_SIZE - CANV_W);
    }

    for (int y = 0; y < CELL_SIZE; y++) {
      for (int x = 0; x < CELL_SIZE; x++) {
        char *cell = init_blank_canv(CANV_W / CELL_SIZE, CANV_H / CELL_SIZE, show_grid, ' ');
        print_on_text(cell, canv, (CANV_W / CELL_SIZE) * x, (CANV_H / CELL_SIZE) * y);
        free(cell);
      }
    }

    update_game_state(&game, &plr, loots, has_moved, mobs, proj, proj_mob, time, wind, canv);
    if (has_moved > 0)
      has_moved--;

    render_game(&plr, mobs, proj, proj_mob, wind, canv);

    int i = 0;
    if (header != NULL)
      free(header);
    header = init_blank_canv(CANV_W, 3, 1, ' ');
    int start_index = CANV_W + 2;
    for (int x = 0; x < PLR_MAX_HP; x++) {
      start_index += 2;
      header[start_index] = plr.hp > x ? HEART_IC : '.';
    }
    start_index += 4;
    char scr[] = "SCR.";
    for (int i = 0; i < 4; i++) {
      header[start_index++] = scr[i];
    }
    start_index += 6;
    int sc = game.score;
    int pos = start_index;
    do {
      header[pos--] = (sc % 10) + '0';
      sc /= 10;
    } while (sc > 0);
    start_index += 6;
    char bst[] = "BEST.";
    for (int i = 0; i < 5; i++)
      canv[start_index + i] = bst[i];
    start_index += 11;
    sc = game.max_score;
    pos = start_index;
    do {
      header[start_index--] = (sc % 10) + '0';
      sc /= 10;
    } while (sc > 0);
    start_index += 11;
    char lv[] = "Lv. ";
    for (int i = 0; i < 4; i++)
      canv[start_index + i] = lv[i];
    start_index += 5;
    sc = game.level;
    pos = start_index;
    do {
      header[start_index--] = (sc % 10) + '0';
      sc /= 10;
    } while (sc > 0);
    start_index += 11;
    char Wp[] = "Wp. ";
    for (int i = 0; i < 4; i++)
      canv[start_index++] = Wp[i];
    sc = plr.weap;
    do {
      header[start_index--] = (sc % 10) + '0';
      sc /= 10;
    } while (sc > 0);

    start_index += 11;
    char Sh[] = "SH. ";
    for (int i = 0; i < 4; i++)
      canv[start_index++] = Sh[i];
    sc = game.shots_fired;
    do {
      header[start_index--] = (sc % 10) + '0';
      sc /= 10;
    } while (sc > 0);
    print_on_text(header, canv, 0, 0);
    print_canvas(canv, &game);
  }

  free(mobs);
  free(proj);
  free(canv);
  system("clear");

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

void initialize_game(GAME *game, SHIP *plr, LOOT *loots, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *stars) {
  game->level = game->score = game->max_score = game->prj_qual = 0;
  game->lv_choices = 3;
  game->attr = 2;
  game->loot_chance = STRT_LOOT_CHNC;

  plr->x_pos = C_WIDTH * 4 + 2;
  plr->y_pos = C_HEIGHT * 8 + 1;
  plr->hp = PLR_MAX_HP;
  plr->hurt_timer = plr->shield = plr->weap = 0;
  plr->dir = up;

  for (int i = 0; i < MOB_AM; i++) {
    int x, y;
    do {
      x = C_WIDTH * (rand() % CELL_SIZE) + 2;
      y = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
    } while (is_position_occupied(mobs, i, x, y));
    mobs[i].x_pos = x;
    mobs[i].y_pos = y;
    mobs[i].dir = down;
    mobs[i].shield = mobs[i].weap = mobs[i].hurt_timer = 0;
    mobs[i].hp = MOB_MAX_HP;
  }
  for (int i = 0; i < PROJ_BUFFER; i++)
    proj[i].x_pos = proj[i].y_pos = -1;
  for (int i = 0; i < PRJ_MOB_BUFFER; i++)
    mobprj[i].x_pos = mobprj[i].y_pos = -1;
  for (int i = 0; i < LOOT_BUFFER; i++)
    loots[i].x_pos = loots[i].y_pos = -1;
  for (int i = 0; i < STAR_BUFFER - 1; i++)
    stars[i] = rand_range(1, CANV_SIZE - CANV_W);
}

int handle_input(GAME *game, SHIP *plr, int *has_moved, PROJ *proj, PROJ *mob_prj) {
  int c = -1;
  if (!(*has_moved)) {
    c = getchar();
    if (c != EOF) {
      if (c == 'a' && plr->x_pos > 2) {
        plr->dir = left;
        *has_moved = (CANV_W / CELL_SIZE) / 2;
      } else if (c == 'd' && plr->x_pos < CANV_W - (CELL_SIZE - 5 + SHIP_W * 2)) {
        plr->dir = right;
        *has_moved = (CANV_W / CELL_SIZE) / 2;
      } else if (c == 'w' && plr->y_pos > 2) {
        plr->dir = up;
        *has_moved = CANV_H / CELL_SIZE;
      } else if (c == 's' && plr->y_pos < CANV_H - SHIP_H) {
        plr->dir = down;
        *has_moved = CANV_H / CELL_SIZE;
      } else if (c == ' ') {
        if (plr->weap > 0 && game->shots_fired > 0)
          game->shots_fired--;
        if (game->shots_fired == 0) {
          if (plr->weap > 0)
            plr->weap--;
          game->shots_fired = shot_lock;
        }
        int w_prj = plr->weap;
        for (int i = 0; i < PROJ_BUFFER; i++) {
          DIR dir = w_prj == plr->weap       ? plr->dir
                    : w_prj == plr->weap - 1 ? plr->dir == up     ? down
                                               : plr->dir == down ? up
                                               : plr->dir == left ? right
                                                                  : left
                    : w_prj == plr->weap - 2 ? plr->dir == up     ? left
                                               : plr->dir == down ? right
                                               : plr->dir == left ? up
                                                                  : down
                    : w_prj == plr->weap - 3 ? plr->dir == up     ? right
                                               : plr->dir == down ? left
                                               : plr->dir == left ? down
                                                                  : up
                    : w_prj == plr->weap - 4 ? plr->dir == up     ? up_right
                                               : plr->dir == down ? up_left
                                               : plr->dir == left ? down_left
                                                                  : down_right
                    : w_prj == plr->weap - 5 ? plr->dir == up     ? up_left
                                               : plr->dir == down ? up_right
                                               : plr->dir == left ? down_right
                                                                  : down_left
                    : w_prj == plr->weap - 6 ? plr->dir == up     ? down_left
                                               : plr->dir == down ? down_right
                                               : plr->dir == left ? up_left
                                                                  : up_right
                    : w_prj == plr->weap - 7 ? plr->dir == up     ? down_right
                                               : plr->dir == down ? down_left
                                               : plr->dir == left ? up_right
                                                                  : up_left
                                             : up;
          if (proj[i].x_pos == -1) {
            proj[i].x_pos = dir == up || dir == down ? plr->x_pos + SHIP_W / 2 : dir == left || dir == up_left ? plr->x_pos - 3 : plr->x_pos + SHIP_W + 3;
            proj[i].y_pos = dir == up || dir == up_left || dir == up_right         ? plr->y_pos - 1
                            : dir == down || dir == down_left || dir == down_right ? plr->y_pos + SHIP_H / 2
                            : dir == left || dir == right                          ? plr->y_pos + 1
                                                                                   : plr->y_pos - 1;
            proj[i].dir = dir;
            w_prj--;
            if (w_prj <= 0)
              break;
          }
        }
      } else if (c == 'q') {
        exit(0);
      }
    }
  }
  return (c);
}

void update_game_state(GAME *game, SHIP *plr, LOOT *loots, int has_moved, SHIP *mobs, PROJ *proj, PROJ *mobprj, int time, int *wind, char *canv) {
  // Update player position

  int min_lv = game->level / 5;

  if (min_lv > 8)
    min_lv = 8;

  if (has_moved) {
    plr->x_pos += plr->dir == right ? 2 : plr->dir == left ? -2 : 0;
    plr->y_pos += plr->dir == down ? 1 : plr->dir == up ? -1 : 0;
  }

  for (int i = 0; i < LOOT_BUFFER; i++) {
    if (loots[i].x_pos == -1 && loots[i].y_pos == -1)
      continue;
    print_on_text(&loots[i].content, canv, loots[i].x_pos, loots[i].y_pos);
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
  for (int i = 0; i < MOB_AM - 1; i++) {

    if (mobs[i].hurt_timer >= 1) {
      if (mobs[i].hurt_timer == 1) {
        if (rand() % game->loot_chance == 0) {
          for (int x = 0; x < LOOT_BUFFER; x++) {
            if (loots[x].x_pos == -1 && loots[x].y_pos == -1) {
              loots[x].x_pos = mobs[i].x_pos;
              loots[x].y_pos = mobs[i].y_pos;
              loots[x].type = rand() % 7;
              loots[x].content = loots[x].type == MOR_SHLD   ? SHIELD_IC
                                 : loots[x].type == MOR_SC   ? COIN_IC
                                 : loots[x].type == MOR_SHLD ? SHIELD_IC
                                 : loots[x].type == MOR_HP   ? HEART_IC
                                 : loots[x].type == MOR_ATTR ? ATTR_IC
                                 : loots[x].type == MOR_LOOT ? MORLOOT_IC
                                                             : WEAP_IC;
              break;
            }
          }
        }
        mobs[i].y_pos = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
        mobs[i].x_pos = C_WIDTH * (rand() % CELL_SIZE) + 2;
        mobs[i].hp = MOB_MAX_HP + game->level / 5;
        mobs[i].hurt_timer = 0;
      } else
        mobs[i].hurt_timer--;
      continue;
    }

    if (plr->hurt_timer == 0) {
      if (abs(mobs[i].y_pos - plr->y_pos) < 2 && abs(mobs[i].x_pos - plr->x_pos) < 2) {
        if (plr->shield) {
          plr->shield--;
        } else
          plr->hp--;
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
          if (rand() % rand_chance == 0)
            mobs[i].dir = rand() % 4;
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
        DIR dir = rand() % 2;
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

  // Update projectile positions
  for (int i = 0; i < PROJ_BUFFER - 1; i++) {
    if (proj[i].y_pos <= -1 || proj[i].x_pos <= -1 || proj[i].y_pos >= CANV_H || proj[i].x_pos >= CANV_W) {
      proj[i].x_pos = proj[i].y_pos = -1;
      continue;
    }
    int index = proj[i].x_pos + ((CANV_W + 1) * proj[i].y_pos), bull_collision = 0;
    if (canv[index]) {
      if (canv[index != ' ' && canv[index] != '_']) {
        for (int j = 0; j < MOB_AM; j++) {
          int mob_index = mobs[j].x_pos + ((CANV_W + 1) * mobs[j].y_pos);
          if (abs(mobs[j].y_pos - proj[i].y_pos) < 2 && abs(mobs[j].x_pos - proj[i].x_pos) < 2) {
            mobs[j].hp--;
            bull_collision = 1;
            if (mobs[j].hp == 0)
              mobs[j].hurt_timer += 10;
            break;
          }
        }
      }
      if (bull_collision) {
        proj[i].x_pos = proj[i].y_pos = -1;
        continue;
      }
      canv[index] = proj[i].dir == up || proj[i].dir == down ? PRJ_VER : proj[i].dir == left || proj[i].dir == right ? PRJ_HOR : proj[i].dir == up_left || proj[i].dir == down_right ? PRJ_DIAG_L : PRJ_DIAG_R;
    }
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
    DIR d = mobprj[i].dir;
    int index = x + ((CANV_W + 1) * y);
    if (index > 0 && index < CANV_SIZE) {

      if (abs(plr->y_pos - mobprj[i].y_pos) < 2 && abs(plr->x_pos - mobprj[i].x_pos) < 2 && plr->hurt_timer == 0) {
        mobprj[i].x_pos = mobprj[i].y_pos = -1;
        if (plr->shield)
          plr->shield--;
        else
          plr->hp--;
        plr->hurt_timer += HURT_DUR;
        continue;
      }
      if (canv[index] == ' ') {
        canv[index] = d == up || d == down ? PRJ_VER : d == left || d == right ? PRJ_HOR : d == up_left || d == down_right ? PRJ_DIAG_L : PRJ_DIAG_R;
      }
    }
    if (time % 2 == 0) {
      mobprj[i].x_pos += d == left || d == up_left || d == down_left ? -2 : d == up || d == down ? 0 : 2;
      mobprj[i].y_pos += d == up || d == up_left || d == up_right ? -1 : d == left || d == right ? 0 : 1;
    }
  }

  // Update wind effects
  for (int j = 0; j < WIND_AM; j++) {
    int new_rand_index;
    if (plr->dir == up)
      new_rand_index = plr->x_pos + 1 + ((plr->y_pos - 3 + SHIP_H) * (CANV_W + 1)) + (rand() % 2);
    else if (plr->dir == right)
      new_rand_index = plr->x_pos - 3 + ((CANV_W + 1) * (plr->y_pos + rand() % 2));
    else if (plr->dir == left)
      new_rand_index = plr->x_pos + SHIP_W + 3 + ((CANV_W + 1) * (plr->y_pos + rand() % 2));
    else if (plr->dir == down)
      new_rand_index = plr->x_pos + 1 + ((plr->y_pos - 3) * (CANV_W + 1)) + (rand() % 2);
    if (wind[j] < 0 || wind[j] > CANV_SIZE || canv[wind[j]] != ' ' || rand() % 2 == 0)
      wind[j] = new_rand_index;
    canv[wind[j]] = rand() % 2 == 0 ? (plr->dir == up || plr->dir == down ? WIND_VER : WIND_HOR) : WIND_DOT;
    wind[j] += plr->dir == up ? CANV_W + 1 : plr->dir == down ? -CANV_W - 1 : plr->dir == left ? 2 : plr->dir == right ? -2 : 1;
    if (wind[j] > CANV_SIZE)
      wind[j] = new_rand_index;
  }
}

void render_game(SHIP *plr, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *wind, char *canv) {

  // Render player ship
  char *plr_txt = draw_triangle('*', plr->dir, SHIP_W, SHIP_H, 2);
  if (plr->shield) {
    char *circ = draw_circle(SHIP_W + 6, SHIP_W + 6, plr->shield <= 2 ? PLR_SHLD_OUTL : plr->shield <= 4 ? PLR_SHLD_OUTL1 : PLR_SHLD_OUTL2, (float)plr->shield / 6);
    print_on_text(circ, canv, plr->x_pos - 3, plr->y_pos - 4);
    free(circ);
  }

  if (plr->hurt_timer > 0) {
    if (plr->hurt_timer % 3 == 0)
      print_on_text(plr_txt, canv, plr->x_pos, plr->y_pos);
    plr->hurt_timer--;
  } else
    print_on_text(plr_txt, canv, plr->x_pos, plr->y_pos);
  free(plr_txt);

  // Render mobs
  for (int i = 0; i < MOB_AM - 1; i++) {
    SHIP mob = mobs[i];

    char *mob_tri = draw_triangle('*', mob.dir, SHIP_W, SHIP_H, 1);
    if (mob.hurt_timer > 0) {
      int i = 0;
      while (mob_tri[i]) {
        if (mob_tri[i] != ' ' && mob_tri[i] != '\n')
          mob_tri[i] = EXPL_START;
        i++;
      }
    }

    if (mob.x_pos - SHIP_W > 0 && mob.x_pos < CANV_W && mob.y_pos - SHIP_H < CANV_H && mob.y_pos < CANV_H)
      print_on_text(mob_tri, canv, mob.x_pos, mob.y_pos);
    free(mob_tri);
  }

  // Render projectiles and wind effects are handled in update_game_state()
}

int print_canvas(char *canv, GAME *game) {
  system("clear");
  char *mob_color1 = game->level < 5 ? BLUE : game->level < 10 ? GREEN : game->level < 15 ? YELLOW : game->level < 20 ? ORANGE : game->level < 25 ? RED : MAGENTA;
  char *mob_color2 = game->level < 5 ? LIGHT_BLUE : game->level < 10 ? DARK_GREEN : game->level < 15 ? PALE_YELLOW : game->level < 20 ? RED : game->level < 25 ? BROWN : CYAN;

  char *plr_color1 = BRIGHT_RED;
  char *plr_color2 = CYAN;
  int i = 0;
  while (canv[i]) {
    if (canv[i] == '#')
      printf(" ");
    else if (canv[i] == COL_HOR || canv[i] == COL_VER)
      print_colored_text(NULL, canv[i] == COL_HOR ? '_' : '|', rand() % 4 == 0 ? MAGENTA : rand() % 4 == 0 ? BLUE : GREEN);
    else if (canv[i] == PRJ_HOR || canv[i] == PRJ_VER || canv[i] == PRJ_DIAG_L || canv[i] == PRJ_DIAG_R)
      print_colored_text(NULL, canv[i] == PRJ_HOR ? '_' : canv[i] == PRJ_VER ? '|' : canv[i] == PRJ_DIAG_L ? '/' : '\\', rand() % 2 == 0 ? RED : ORANGE);
    else if (canv[i] == WIND_DOT || canv[i] == WIND_VER || canv[i] == WIND_HOR)
      print_colored_text(NULL, canv[i] == WIND_DOT ? '.' : canv[i] == WIND_VER ? '|' : '-', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (canv[i] == CANV_VER || canv[i] == CANV_HOR)
      printf(canv[i] == CANV_VER ? "|" : "_");
    else if (canv[i] == PLR_SHLD_OUTL)
      print_colored_text(NULL, '.', rand() % 2 == 0 ? BLUE : LIGHT_BLUE);
    else if (canv[i] == PLR_SHLD_OUTL1)
      print_colored_text(NULL, '.', rand() % 2 == 0 ? GREEN : DARK_GREEN);
    else if (canv[i] == PLR_SHLD_OUTL2)
      print_colored_text(NULL, '.', rand() % 2 == 0 ? RED : ORANGE);
    else if (canv[i] == MOB_LBORD || canv[i] == MOB_RBORD || canv[i] == MOB_HOR || canv[i] == MOB_VER)
      print_colored_text(NULL, canv[i] == MOB_LBORD ? '/' : canv[i] == MOB_RBORD ? '\\' : canv[i] == MOB_HOR ? '_' : '|', rand() % 2 == 0 ? mob_color1 : mob_color2);
    else if (canv[i] == SHP_LBORD || canv[i] == SHP_RBORD || canv[i] == SHP_HOR || canv[i] == SHP_VER)
      print_colored_text(NULL, canv[i] == SHP_LBORD ? '/' : canv[i] == SHP_RBORD ? '\\' : canv[i] == SHP_HOR ? '_' : '|', rand() % 2 == 0 ? plr_color1 : plr_color2);
    else if (canv[i] == HEART_IC)
      print_colored_text(HEART_CODE, '\0', RED);
    else if (canv[i] == COIN_IC)
      print_colored_text(COIN_CODE, '\0', BLUE);
    else if (canv[i] == WEAP_IC)
      print_colored_text(WEAP_CODE, '\0', RED);
    else if (canv[i] == SHIELD_IC)
      print_colored_text(WEAP_CODE, '\0', MAGENTA);
    else if (canv[i] == ATTR_IC)
      print_colored_text(ATTR_CODE, '\0', CYAN);
    else if (canv[i] == EXPL_START || canv[i + 1] == EXPL_START || canv[i - 1] == EXPL_START || canv[i + CANV_W + 1] == EXPL_START || canv[i - CANV_W - 1] == EXPL_START) {
      game->score += 10;
      int r = rand_range(0, 4);
      print_colored_text(NULL, r == 0 ? '.' : r == 1 ? '/' : r == 2 ? '\\' : '*', rand() % 2 == 0 ? mob_color1 : mob_color2);
    } else
      printf("%c", canv[i]);
    i++;
  }
  return 1;
}