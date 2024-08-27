#include "../headers/includes.h"

void initialize_game(GAME *game, SHIP *plr, LOOT *loots, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars, BLDING *buildings) {
  set_nonblocking_mode(1);

  game->loot_chance = STRT_LOOT_CHNC;
  game->level = game->score = game->max_score = game->prj_qual = 0;
  game->lv_choices = 3, game->attr = 2;
  game->gl_x_pos = 0, game->gl_y_pos = 0, game->prj_col_index = -1, game->plr_col_index = -1, game->is_framing = 0;
  game->boss_mode = 0, game->is_in_boss_fight = 0;
  game->mv_type = normal, game->atk_type = autom, game->allowed_dir = all, game->allowed_lk_dir = all;
  game->cur_proj = base;

  plr->x_pos = PLR_CENTER_X + 5;
  plr->y_pos = CANV_H + SHIP_H + 2;
  plr->hp = PLR_MAX_HP;
  plr->hurt_timer = plr->shield = plr->weap = 0, plr->atk_pow = 1, plr->atk_speed = 10, plr->atk_am = 1;
  plr->speed = 1;
  plr->atk_reload = 1;
  plr->dir = up, plr->lk_dir = up;

  for (int i = 0; i < MOBS_BUFFER; i++) {
    int is_2 = i > MOBS1_BUFFER;
    int x, y;
    do {
      x = C_WIDTH * (rand() % CELL_SIZE) + 2;
      y = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
    } while (is_pos_free(mobs, i, x, y));
    mobs[i].x_pos = x, mobs[i].y_pos = y;
    mobs[i].shield = mobs[i].weap = mobs[i].hurt_timer = 0, mobs[i].hp = is_2 ? MOBS_2_MAXHP : MOBS_1_MAXHP, mobs[i].speed = is_2 ? MOBS_2_SPD : MOBS_1_SPD;
    mobs[i].dir = rand_range(0, 3);
  }

  for (int i = 0; i < PROJ_BUFFER; i++) {
    proj[i].x_pos = proj[i].y_pos = -1;
  }

  for (int i = 0; i < PRJ_MOB_BUFFER; i++)
    mobprj[i].x_pos = mobprj[i].y_pos = -1;
  for (int i = 0; i < LOOT_BUFFER; i++)
    loots[i].x_pos = loots[i].y_pos = -1;

  for (int i = 0; i < STAR_BUFFER - 1; i++) {
    stars[i].x_pos = rand_range(1, CANV_W - 1);
    stars[i].y_pos = rand_range(1, CANV_H - 1);
    stars[i].z_pos = rand_range(2, 16);
  }
  for (int i = 0; i < BLDING_BUFFER - 1; i++) {
    buildings[i].x_pos = rand_range(-200, 200);
    buildings[i].y_pos = rand_range(-200, 200);
    buildings[i].y_size = rand_range(5, 40);
    buildings[i].x_size = buildings[i].y_size * 2;

    buildings[i].type = rand_range(0, BLD_TYPE_COUNT);
    buildings[i].content = init_blank_canv(buildings[i].x_size, buildings[i].y_size, 1, '#');
  }
}

char *init_blank_canv(int w, int h, int has_ext, char interior) {
  char *canv = malloc(w + (w * h) + 1);
  int i = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if ((x == 0 && y == 0) || (x == 0 && y == h - 1) || (x == w - 1 && y == 0) || (x == w - 1 && y == h - 1))
        canv[i++] = ' ';
      else if (x == 0 || x == w - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 1 ? CANV_VER : COL_VER;
      else if (y == 0 || y == h - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 1 ? CANV_HOR : COL_HOR;
      else
        canv[i++] = interior;
    }
    canv[i++] = '\n';
  }
  canv[i] = '\0';
  return (canv);
}

PROJ Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (prj[i].col_timer || (prj[i].x_pos != -1 && prj[i].y_pos != -1)) continue;
    prj[i].x_pos = x_pos, prj[i].y_pos = y_pos, prj[i].dir = dir, prj[i].type = type;
    return prj[i];
  }
  return prj[PROJ_BUFFER - 1];
}

void init_boss(GAME *game, BOSS *boss) {
  game->is_in_boss_fight = 1, game->boss_mode = 2;

  boss->content = strdup(SHIELD_GFX);
  boss->height = get_height(boss->content), boss->width = get_width(boss->content);
  boss->cur_hp = 200, boss->max_HP = 200, boss->prev_HP = 200, boss->max_shield = 50, boss->prv_shield = 50;
  boss->speed = 8, boss->shield = 0, boss->dmg = 5, boss->atk_spd = 20, boss->speATK_hole = 0, boss->hole_xdir = 1, boss->hole_size = SHIP_H + 4;
  boss->x_dir = 0, boss->y_dir = 0;
  boss->x_pos = CANV_X_CENTER - boss->width / 2, boss->y_pos = -boss->height + 1;
  boss->action = entrance, boss->next_action = shield, boss->time_in_state = 120;
  boss->shld_ic_count = 0;
  int i = 0;
  while (boss->content[i])
    if (boss->content[i++] == SHLD_OFF_MOB_IC) boss->shld_ic_count++;
}