#include "../headers/includes.h"

int launch_proj(GAME *game, SHIP *plr, PROJ *proj) {
  plr->atk_reload = plr->atk_speed;
  if (plr->weap > 0 && game->shots_fired > 0) game->shots_fired--;
  if (game->shots_fired == 0) {
    if (plr->weap > 0) plr->weap--;
    game->shots_fired = shot_lock;
  }
  int prj_x;
  int prj_y;
  DIR dir = get_prj_dir(plr->atk_am, plr->atk_am, *plr, plr->lk_dir);
  prj_x = (dir == up || dir == down ? plr->x_pos + SHIP_W / 2 : dir == left || dir == up_left ? plr->x_pos - 3 : plr->x_pos + SHIP_W + 3);
  prj_y = dir == up || dir == up_left || dir == up_right         ? plr->y_pos - 1
          : dir == down || dir == down_left || dir == down_right ? plr->y_pos + SHIP_H / 2
          : dir == left || dir == right                          ? plr->y_pos + 1
                                                                 : plr->y_pos - 1;
  Init_bullet(proj, game->cur_proj, game->mv_type == watch_up ? up : dir, PROJ_BUFFER, prj_x, prj_y);

  int w_prj = plr->atk_am - 1;
  int i = 0;
  for (int x = w_prj; x > 0; x++) {
    if (w_prj <= 0 || i + x > PROJ_BUFFER - 1) break;
    if (proj[i + x].x_pos == -1 && proj[i + x].y_pos == -1) continue;
    DIR dir = get_prj_dir(w_prj, plr->atk_am, *plr, plr->lk_dir);
    prj_x = proj[i].x_pos + (dir == up || dir == down ? 1 : 0);
    prj_y = proj[i].y_pos + (dir == left || dir == right ? 1 : 0);
    Init_bullet(proj, game->cur_proj, dir, PROJ_BUFFER, prj_x, prj_y);
    w_prj--;
  }
  return (1);
}
void update_proj_direction(int mob_x, int mob_y, PROJ *proj) {
  int diff_x = mob_x - proj->x_pos + 3;
  int diff_y = mob_y - proj->y_pos + 3;
  if (abs(diff_x) > abs(diff_y)) {
    if (diff_y == 0)
      proj->dir = diff_x > 0 ? right : left;
    else
      proj->dir = diff_x > 0 ? (diff_y > 0 ? down_right : up_right) : (diff_y > 0 ? down_left : up_left);
  } else {
    if (diff_x == 0)
      proj->dir = diff_y > 0 ? down : up;
    else
      proj->dir = diff_y > 0 ? (diff_x > 0 ? down_right : down_left) : (diff_x > 0 ? up_right : up_left);
  }
}

int set_input(GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mob_prj, int *has_moved, int time, int *show_grid, int *god_mode) {
  (void)boss, (void)proj, (void)mob_prj, (void)time;

  int c = -1;
  if (game->is_framing) {
    while (c == EOF) {
      c = getchar();
      if (c == '<') return (1);
    }
    game->is_framing = !game->is_framing;
  }

  c = getchar();
  game->shift_on = (c >= 'A' && c <= 'Z') ? 1 : 0;
  if (game->shift_on) {
    c += 32;
    plr->dir = none;
  }
  if (is_in_string(c, "wasd ") && game->mv_type == cutscene) return (c);

  if (c != EOF) {
    switch (c) {
      case '\t':
        game->cur_proj++;
        if (game->cur_proj == prj_count) game->cur_proj = 0;
        return ('t');
      case '\033':
        getchar();
        char arrow = getchar();
        DIR lk_dir = arrow == 'A' ? up : arrow == 'B' ? down : arrow == 'C' ? right : left;
        if (is_dir_allowed(lk_dir, game->allowed_lk_dir)) plr->lk_dir = lk_dir;
        if ((game->atk_type != disabled || game->atk_type == autom) && !plr->atk_reload) launch_proj(game, plr, proj);
        return (arrow == 'A' ? '^' : arrow == 'B' ? 'v' : arrow == 'C' ? '<' : '>');
      case ' ':
        if (game->atk_type != disabled && !plr->atk_reload) launch_proj(game, plr, proj);
        return (c);
      case 'w':
      case 'a':
      case 's':
      case 'd': {
        DIR d = plr->lk_dir;
        if (c == 'a' && plr->x_pos > 2)
          d = left, *has_moved = (CANV_W / CELL_SIZE) / 2;
        else if (c == 'd' && plr->x_pos < CANV_W - (CELL_SIZE - 5 + SHIP_W * 2))
          d = right, *has_moved = (CANV_W / CELL_SIZE) / 2;
        else if (c == 'w' && plr->y_pos > 2)
          d = up, *has_moved = CANV_H / CELL_SIZE;
        else if (c == 's' && plr->y_pos < CANV_H - SHIP_H)
          d = down, *has_moved = CANV_H / CELL_SIZE;
        if (is_dir_allowed(d, game->allowed_dir)) plr->dir = d;
      }
      case 'm':
        *show_grid = !show_grid;
        return (c);
      case '1':
        game->mv_type = normal;
        break;
      case '2':
        game->mv_type = on_grid;
        break;
      case 'g':
        *god_mode = !god_mode;
        if (god_mode)
          plr->atk_am = 2, plr->atk_pow = 3, plr->atk_speed = 3, plr->hp = 50, plr->weap = 8;
        else
          plr->atk_am = 1, plr->atk_pow = 1, plr->atk_speed = 1, plr->hp = 5, plr->weap = 1;
        return (c);
      case 'q':
        exit(0);
      case '<':
        game->is_framing = 1;
        return (c);
    }
    return (c);
  }
  return (c);
}

void upd_game_state(char *canv, GAME *game, SHIP *plr, BOSS *boss, SHIP *mobs, PROJ *proj, PROJ *mobprj, LOOT *loots, STAR *stars, int has_moved, int time, int *wind) {
  // upd player position
  if (!game->is_in_boss_fight) {
    if (time % BOSS_SPWNTIME >= BOSS_SPWNTIME - 100 && time % BOSS_SPWNTIME < BOSS_SPWNTIME) {
      int dng_x = 20, dng_y = 5;
      game->mv_type = cutscene;
      game->allowed_dir = up, game->allowed_lk_dir = up, plr->dir = up, plr->lk_dir = up;
      char *dng_msg = init_blank_canv(dng_x, dng_y, 1, '#');
      write_on_canv("DANGER !\n BOSS INCOMING", dng_msg, 3, 2);
      write_on_canv(dng_msg, canv, CANV_X_CENTER - dng_x / 2, CANV_Y_CENTER - dng_y / 2);
      free(dng_msg);
    }
    if (time % BOSS_SPWNTIME == 0) init_boss(game, boss);
  }
  upd_player(game, plr, proj, has_moved, time);
  upd_background(canv, stars, *game, *boss, *plr, has_moved, time);

  if (game->is_in_boss_fight) upd_boss_fight(canv, game, boss, mobs, mobprj, loots, plr, time);
  upd_loots(canv, game, plr, loots, time);
  upd_mob_pos(canv, game, *boss, mobs, mobprj, loots, plr, time);
  upd_plr_proj(canv, game, proj, mobs, boss, plr, time);
  upd_mob_prj(canv, game, mobprj, plr, time);
  upd_wind(canv, wind, game, plr, boss);
  // upd_buildings(canv, building, game, plr);
}
int upd_player(GAME *game, SHIP *plr, PROJ *proj, int has_moved, int time) {
  if (plr->atk_reload > 0) plr->atk_reload--;
  if (game->atk_type == autom && !plr->atk_reload) launch_proj(game, plr, proj);

  // SET PLR POS ON CANVAS
  switch (game->mv_type) {
    case normal:
    case cutscene:
      if (time % 4 == 0) {
        int y_center = game->is_in_boss_fight ? PLR_CENTER_Y_BOSS : PLR_CENTER_Y;
        if (plr->x_pos != PLR_CENTER_X) plr->x_pos += (plr->x_pos < PLR_CENTER_X + 1 ? 1 : -1);
        if (plr->y_pos != y_center) plr->y_pos += (plr->y_pos < y_center ? 1 : -1);
      }
      break;
    case on_grid:
      if (has_moved && is_in_canv(plr->x_pos, plr->y_pos, SHIP_W, SHIP_H, plr->dir)) {
        plr->y_pos += plr->dir == down ? 2 : plr->dir == up ? -2 : 0;
        plr->x_pos += plr->dir == right ? 2 : plr->dir == left ? -2 : 0;
        plr->lk_dir = plr->dir;
      }
      break;
    case free_slide:
    case watch_up:
      if (is_in_canv(plr->x_pos, plr->y_pos, SHIP_W, SHIP_H, plr->dir)) plr->x_pos += plr->dir == right ? 1 : plr->dir == left ? -1 : 0;
      break;
    default:
      break;
  }
  // SET GLOB POSITION
  if (game->mv_type != on_grid) {
    game->gl_x_pos += plr->dir == right ? plr->speed : plr->dir == left ? -plr->speed : 0;
    game->gl_y_pos += plr->dir == down ? plr->speed : plr->dir == up ? -plr->speed : 0;
  }
  return (1);
}
int upd_background(char *canv, STAR *stars, GAME game, BOSS boss, SHIP plr, int has_moved, int time) {
  for (int i = 0; i < STAR_BUFFER - 1; i++) {

    if (time % stars[i].z_pos == 0) {
      if ((game.mv_type != on_grid && game.mv_type != free_slide) || has_moved) {
        if (time % (plr.dir == left || plr.dir == right ? stars[i].z_pos / 2 : stars[i].z_pos) == 0) {
          stars[i].x_pos += plr.dir == left ? 1 : plr.dir == right ? -1 : 0;
          stars[i].y_pos += plr.dir == up ? 1 : plr.dir == down ? -1 : 0;
        }
      }
      if (game.is_in_boss_fight) {
        if (boss.action == SP_attack2)
          stars[i].y_pos--;
        else
          stars[i].y_pos++;
      }

      if (stars[i].x_pos > CANV_W - 1 || stars[i].x_pos < 1 || stars[i].y_pos > CANV_H - 1 || stars[i].y_pos < 1) {
        stars[i].x_pos = plr.dir == left ? 1 : plr.dir == right ? CANV_W - 1 : rand_range(1, CANV_W - 1);
        stars[i].y_pos = plr.dir == up ? 1 : plr.dir == down ? CANV_H - 1 : rand_range(1, CANV_H - 1);
      }
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
    write_on_canv(c, canv, stars[i].x_pos, stars[i].y_pos);
    free(c);
  }
  return 1;
}
int upd_loots(char *canv, GAME *game, SHIP *plr, LOOT *loots, int time) {
  // upd LOOTS
  for (int i = 0; i < LOOT_BUFFER; i++) {
    if (loots[i].x_pos == -1 && loots[i].y_pos == -1) continue;
    if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
      loots[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
      loots[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    }
    write_on_canv(&loots[i].content, canv, loots[i].x_pos, loots[i].y_pos);
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
  return 1;
}

int upd_mob_pos(char *canv, GAME *game, BOSS boss, SHIP *mobs, PROJ *mobprj, LOOT *loots, SHIP *plr, int time) {
  (void)canv, (void)boss;

  if (!game->is_in_boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      if (time % BOSS_SPWNTIME >= BOSS_SPWNTIME - 100 && time % BOSS_SPWNTIME <= BOSS_SPWNTIME) mobs[i].hp = 0;

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
          mobs[i].hp = (MOBS_BUFFER <= MOBS1_BUFFER ? MOBS_1_MAXHP : MOBS_2_MAXHP);
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
      if (time % mobs[i].speed != 0) continue;

      int t = 10 - (game->level / 5) < 3 ? 3 : 10 - (game->level / 5);
      if (time % t == 0) {
        int distance_threshold = game->level * 2 > 40 ? 40 : game->level * 2;
        int x_diff = mobs[i].x_pos - plr->x_pos;
        int y_diff = mobs[i].y_pos - plr->y_pos;
        if (abs(x_diff) < distance_threshold || abs(y_diff) < distance_threshold) {
          if (abs(x_diff) > abs(y_diff)) {
            mobs[i].dir = x_diff > 0 ? left : right;
          } else
            mobs[i].dir = y_diff > 0 ? up : down;
        } else {
          int rand_chance = 50;
          if (rand() % rand_chance == 0) mobs[i].dir = rand() % 4;
        }
        int rand_chance = 50 - game->level < 5 ? 5 : 50 - game->level;
        if (rand() % rand_chance == 0) {
          DIR d = mobs[i].dir;
          int x_p = d == up || d == down ? mobs[i].x_pos + SHIP_W / 2 : d == left || d == up_left ? mobs[i].x_pos - 3 : mobs[i].x_pos + SHIP_W + 3;
          int y_p = d == up || d == up_left || d == up_right         ? mobs[i].y_pos - 1
                    : d == down || d == down_left || d == down_right ? mobs[i].y_pos + SHIP_H / 2
                    : d == left || d == right                        ? mobs[i].y_pos + 1
                                                                     : mobs[i].y_pos - 1;
          Init_bullet(mobprj, base, mobs[i].dir, PRJ_MOB_BUFFER, x_p, y_p);
        }
        mobs[i].y_pos += mobs[i].dir == down ? 1 : mobs[i].dir == up ? -1 : 0;
        mobs[i].x_pos += mobs[i].dir == right ? 2 : mobs[i].dir == left ? -2 : 0;

        // REPOSITION MOB IF TOO FAR FROM CANVAS
        if (mobs[i].y_pos >= CANV_H + MOB_OUT_LIM || mobs[i].y_pos + SHIP_H < -CANV_H - MOB_OUT_LIM || mobs[i].x_pos >= CANV_W + MOB_OUT_LIM || mobs[i].x_pos + SHIP_W < -CANV_W - MOB_OUT_LIM) {
          DIR dir = rand_range(0, 3);
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
  return 1;
}
int upd_plr_proj(char *canv, GAME *game, PROJ *proj, SHIP *mobs, BOSS *boss, SHIP *plr, int time) {
  for (int i = 0; i < PROJ_BUFFER - 1; i++) {
    if (proj[i].y_pos <= -1 || proj[i].x_pos <= -1 || proj[i].y_pos >= CANV_H || proj[i].x_pos >= CANV_W) {
      proj[i].x_pos = proj[i].y_pos = -1;
      continue;
    }
    int index = proj[i].x_pos + ((CANV_W + 1) * proj[i].y_pos);
    if (proj[i].col_timer) {
      proj[i].col_timer--;
      if (proj[i].col_timer == 0) {
        if (proj[i].type == bomb) {
          for (int y = -2; y <= 2; y++) {
            for (int x = -2; x <= 2; x++) {
              if (x == 0 && y == 0) continue;
              int x_p = proj[i].x_pos + x, y_p = proj[i].y_pos + y;
              PROJ new_proj = Init_bullet(proj, bomb, none, PROJ_BUFFER, x_p, y_p);
              new_proj.col_timer = 5;
            }
          }
        }
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
          for (int j = 0; j < MOBS_BUFFER; j++) {
            if (abs(mobs[j].y_pos - proj[i].y_pos) < 5 && abs(mobs[j].x_pos - proj[i].x_pos) < 5) {
              mobs[j].hp -= plr->atk_pow;
              proj[i].col_timer = 15;
              break;
            }
          }
        }
      }
      char ic;
      PRJ_TYPES t = proj[i].type;
      switch (proj[i].dir) {
        case up:
        case down:
          ic = t == base ? PRJ_BS_VER : t == magnet ? PRJ_MAGN_VER : t == bomb ? PRJ_BMB_VER : t == big ? PRJ_BIG_VER : PRJ_BS_VER;
          break;
        case left:
        case right:
          ic = t == base ? PRJ_BS_HOR : t == magnet ? PRJ_MAGN_HOR : t == bomb ? PRJ_BMB_HOR : t == big ? PRJ_BIG_HOR : PRJ_BS_HOR;
          break;
        case up_left:
        case down_right:
          ic = t == base ? PRJ_BS_DIAG_L : t == magnet ? PRJ_MAGN_DIAG_L : t == bomb ? PRJ_BMB_DIAG_L : t == big ? PRJ_BIG_DIAG_L : PRJ_BS_DIAG_L;
          break;
        default:
          ic = t == base ? PRJ_BS_DIAG_R : t == magnet ? PRJ_MAGN_DIAG_R : t == bomb ? PRJ_BMB_DIAG_R : t == big ? PRJ_BIG_DIAG_R : PRJ_BS_DIAG_R;
          break;
      }
      canv[index] = ic;
    }
    if (!game->is_in_boss_fight) {
      proj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
      if (time % 2 == 0) proj[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    }
    PRJ_TYPES tp = proj[i].type;
    int speed = tp == base ? 1 : tp == magnet ? 3 : tp == bomb ? 6 : tp == big ? 4 : 1;
    if (game->is_in_boss_fight) speed = 1;

    if (time % speed == 0) {
      if (proj[i].dir != none) {
        proj[i].x_pos += proj[i].dir == left || proj[i].dir == up_left || proj[i].dir == down_left ? -2 : proj[i].dir == up || proj[i].dir == down ? 0 : 2;
        proj[i].y_pos += proj[i].dir == up || proj[i].dir == up_left || proj[i].dir == up_right ? -1 : proj[i].dir == left || proj[i].dir == right ? 0 : 1;
      }

      if (proj[i].type == big && rand() % 10 == 0) {
        int x_offset = 0;
        while (x_offset == 0)
          x_offset = rand_range(-2, 2);
        int y_offset = 0;
        while (y_offset == 0)
          y_offset = rand_range(-2, 2);
        Init_bullet(proj, proj[i].type, proj[i].dir, PROJ_BUFFER, proj[i].x_pos + x_offset, proj[i].y_pos + y_offset);
      }

      if (proj[i].type == magnet) {
        if (game->is_in_boss_fight) {
          update_proj_direction(boss->x_pos + boss->width / 2, boss->y_pos + boss->height / 2, &proj[i]);
          return (1);
        } else {
          for (int j = 0; j < MOBS_BUFFER; j++) {
            SHIP m = mobs[j];
            if (m.hurt_timer) continue;
            if (abs(m.x_pos - proj[i].x_pos + 3) < 30 && abs(m.y_pos - proj[i].y_pos + 3) < 30) {
              update_proj_direction(m.x_pos, m.y_pos, &proj[i]);
              break;
            }
          }
        }
      }
    }
  }
  return 1;
}

int upd_mob_prj(char *canv, GAME *game, PROJ *mobprj, SHIP *plr, int time) {
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
      if (canv[index] == ' ') canv[index] = d == up || d == down ? MPRJ_BS_VER : d == left || d == right ? MPRJ_BS_HOR : d == up_left || d == down_right ? MPRJ_BS_DIAG_L : MPRJ_BS_DIAG_R;
    }

    int is_hor = d == left || d == right;
    if (is_hor && time % 2 == 0) mobprj[i].x_pos += d == left || d == up_left || d == down_left ? -1 : d == up || d == down ? 0 : 1;

    int is_ver = d == up || d == down;
    if (is_ver && time % 4 == 0) mobprj[i].y_pos += d == up || d == up_left || d == up_right ? -1 : d == left || d == right ? 0 : 1;

    // if (game->mv_type != on_grid && game->mv_type != free_slide) mobprj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;

    // MOVE RELATED TO PLAYER CAM
    if (!game->is_in_boss_fight) {
      if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        mobprj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        mobprj[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
    }
  }

  return 1;
}
int upd_wind(char *canv, int *wind, GAME *game, SHIP *shp, BOSS *boss) {
  for (int j = 0; j < (WIND_BUFFER - 1); j++) {
    DIR dir;
    int y_pos, x_pos, x_size, y_size;
    if (j <= PLR_WIND_BUFFER)
      dir = shp->dir, y_pos = shp->y_pos, x_pos = shp->x_pos, x_size = SHIP_W, y_size = SHIP_H;
    else if (game->is_in_boss_fight && boss->action != die)
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
      if (j > PLR_WIND_BUFFER)
        new_rand_index = x_pos + x_size / 2 - 2 + ((y_pos - 1) * (CANV_W + 1)) + (rand() % 3);
      else
        new_rand_index = x_pos + 2 + ((y_pos) * (CANV_W + 1)) + (rand() % x_size - 1);
    }
    if (wind[j] < 0 || wind[j] > CANV_SIZE || canv[wind[j]] != ' ' || rand() % 2 == 0) wind[j] = new_rand_index;
    canv[wind[j]] = rand() % 2 == 0 ? (dir == up || dir == down ? WIND_VER : WIND_HOR) : WIND_DOT;
    wind[j] += dir == up ? CANV_W + 1 : dir == down ? -CANV_W - 1 : dir == left ? 2 : dir == right ? -2 : 1;
    if (wind[j] > CANV_SIZE) wind[j] = new_rand_index;
  }
  return 1;
}
int upd_buildings(char *canv, BLDING *buildings, GAME *game, SHIP plr) {
  (void)game;
  for (int i = 0; i < BLDING_BUFFER - 1; i++) {
    buildings[i].x_pos += plr.dir == left ? 1 : plr.dir == right ? -1 : 0;
    buildings[i].y_pos += plr.dir == up ? 1 : plr.dir == down ? -1 : 0;
    if (buildings[i].x_pos <= CANV_W - 1 && buildings[i].x_pos + buildings[i].x_size > 1 && buildings[i].y_pos <= CANV_H - 1 && buildings[i].y_pos + buildings[i].y_size > 1) {
      char *text = strdup(buildings[i].content);
      if (text != NULL) {
        write_on_canv(text, canv, buildings[i].x_pos, buildings[i].y_pos);
        free(text);
      }
    }
  }
  return 1;
}

int set_plr_death(char *canv, GAME game, SHIP plr) {
  plr.hp = 0;
  int x_size = CANV_X_CENTER, y_size = CANV_Y_CENTER;
  int option_selected = -1;
  char *msg = init_blank_canv(x_size, y_size, 1, '#');
  set_over_canv(msg, x_size / 2 - 7, 2, "GAME OVER \n \n SCORE: %d\n BEST: %d\n \n \n- CONTINUE (c)\n \n- restart (r)\n \n- QUIT (q)\n \n- SHOW_STATS (s)\n \n- PARAM (p)", game.score, game.max_score);
  write_on_canv(msg, canv, (CANV_X_CENTER)-x_size / 2, (CANV_Y_CENTER)-y_size / 2);
  free(msg);
  system("clear");
  render_canvas(canv, &game, 0);
  int inp = -1;
  while (option_selected == -1) {
    inp = getchar();
    if (inp == 'r') option_selected = 2;
    if (inp == 'c') option_selected = 1;
    if (inp == 'q') option_selected = 0;
  }
  return option_selected;
}