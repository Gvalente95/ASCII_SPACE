#include "../headers/includes.h"

const char *bld_labs[] = {"SHOP", "ARMORY", "HOUSE", "FORTRESS", "NONE", "BLDING_COUNT"};

int launch_proj(GAME *game, SHIP *plr, PROJ *proj) {
  if (game == NULL || plr == NULL || proj == NULL) return 0;
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

int set_input(char *canv, GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mob_prj, int *has_moved, int time, int *show_grid, int *god_mode) {
  if (game == NULL || plr == NULL || proj == NULL || mob_prj == NULL || has_moved == NULL || god_mode == NULL || show_grid == NULL) return 0;

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
  if (c == 'p') {
    char *pause_canv = init_blank_canv(30, 8, 1, '#');
    int ch = -1;
    write_on_canv("Press 'p' to un-pause", pause_canv, 4, 4);
    write_on_canv(pause_canv, canv, CANV_X_CENTER - 15, CANV_Y_CENTER - 4);
    render_canvas(canv, game, time);
    while (ch != 'p') {
      ch = getchar();
    }
  }
  if (is_in_string(c, "wasd ") && game->mv_type == cutscene) return (c);

  if (c != EOF) {
    if (!is_in_string(c, "wasdpm12g< \033\t")) plr->dir = none;
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
          d = left, *has_moved = PLR_GRID_SPD;
        else if (c == 'd' && plr->x_pos < CANV_W - (CELL_SIZE - 5 + SHIP_W * 2))
          d = right, *has_moved = PLR_GRID_SPD;
        else if (c == 'w' && plr->y_pos > 2)
          d = up, *has_moved = PLR_GRID_SPD;
        else if (c == 's' && plr->y_pos < CANV_H - SHIP_H)
          d = down, *has_moved = PLR_GRID_SPD;
        if (is_dir_allowed(d, game->allowed_dir)) {
          plr->dir = d;
          if (game->cur_blding != NONE) plr->lk_dir = d;
        } else if (game->g_st != boss_fight)
          plr->dir = none;
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

void upd_game_state(char *canv, GAME *game, SHIP *plr, BOSS *boss, SHIP *mobs, PROJ *proj, PROJ *mobprj, ITEM *ITEMs, BLDING *buildings, STAR *stars, int has_moved, int time, int *wind) {
  if (canv == NULL || game == NULL || plr == NULL || boss == NULL || mobs == NULL || proj == NULL || mobprj == NULL || ITEMs == NULL || buildings == NULL || stars == NULL) return;
  // upd player position
  if (game->g_st != boss_fight) {
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
  upd_player(canv, game, plr, proj, has_moved, time);
  upd_mob_pos(canv, game, *boss, mobs, mobprj, ITEMs, plr, time, 0);

  if (game->g_st == boss_fight) upd_boss_fight(canv, game, boss, mobs, mobprj, ITEMs, plr, time);
  if (game->cur_blding == NONE) {
    upd_background(canv, stars, *game, *boss, *plr, has_moved, time);
    upd_ITEMs(canv, game, plr, ITEMs, time, 0);
    upd_plr_proj(canv, game, proj, mobs, boss, plr, time, 0);
    upd_mob_prj(canv, game, mobprj, plr, time, 0);
    if (plr->dir != none) upd_wind(canv, wind, game, plr, boss);
  }
  upd_buildings(canv, buildings, game, plr, time, &has_moved);
}
int upd_player(char *canv, GAME *game, SHIP *plr, PROJ *proj, int has_moved, int time) {
  if (plr->atk_reload > 0) plr->atk_reload--;
  if (game->atk_type == autom && !plr->atk_reload) launch_proj(game, plr, proj);

  int left_col = plr->x_pos - 2, right_col = plr->x_pos + SHIP_W + 2;
  int up_col = plr->y_pos - CANV_W - 1, down_col = plr->y_pos + ((CANV_W + 1) * (SHIP_H + 2));

  // SET PLR POS ON CANVAS
  switch (game->mv_type) {
    case normal:
    case cutscene:
      if (game->cur_blding != NONE && has_moved <= 0) plr->dir = none;
      if (time % 4 == 0) {
        int y_center = game->g_st == boss_fight ? PLR_CENTER_Y_BOSS : PLR_CENTER_Y;
        if (plr->x_pos != PLR_CENTER_X) plr->x_pos += (plr->x_pos < PLR_CENTER_X + 1 ? 1 : -1);
        if (plr->y_pos != y_center) plr->y_pos += (plr->y_pos < y_center ? 1 : -1);
      }
      break;
    case on_grid:
      if (has_moved && is_in_canv(plr->x_pos, plr->y_pos, SHIP_W, SHIP_H, 0, 0, CANV_W, CANV_H, plr->dir)) {
        if (time % 2 == 0) plr->y_pos += plr->dir == down ? 1 : plr->dir == up ? -1 : 0;
        plr->x_pos += plr->dir == right ? 1 : plr->dir == left ? -1 : 0;
        plr->lk_dir = plr->dir;
      }
      if (has_moved <= 0) plr->dir = none;
      break;
    case free_slide:
    case watch_up:
      if (is_in_canv(plr->x_pos, plr->y_pos, SHIP_W, SHIP_H, 0, 0, CANV_W, CANV_H, plr->dir)) plr->x_pos += plr->dir == right ? 1 : plr->dir == left ? -1 : 0;
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
    if (!plr.is_blocked) {
      if (time % stars[i].z_pos == 0) {
        if ((game.mv_type != on_grid && game.mv_type != free_slide) || has_moved) {
          if (time % (plr.dir == left || plr.dir == right ? stars[i].z_pos / 2 : stars[i].z_pos) == 0) {
            stars[i].x_pos += plr.dir == left ? 1 : plr.dir == right ? -1 : 0;
            stars[i].y_pos += plr.dir == up ? 1 : plr.dir == down ? -1 : 0;
          }
        }
        if (game.g_st == boss_fight) {
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
int upd_ITEMs(char *canv, GAME *game, SHIP *plr, ITEM *ITEMs, int time, int reset) {
  // upd ITEMS
  for (int i = 0; i < ITEM_BUFFER; i++) {
    if (reset) ITEMs[i].x_pos = ITEMs[i].y_pos = -1;
    if (ITEMs[i].x_pos == -1 && ITEMs[i].y_pos == -1 || ITEMs[i].content == '\0') continue;

    if (!plr->is_blocked) {
      if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        ITEMs[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        ITEMs[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
    }

    write_on_canv(&ITEMs[i].content, canv, ITEMs[i].x_pos, ITEMs[i].y_pos);
    int x_diff = ITEMs[i].x_pos - plr->x_pos;
    int y_diff = ITEMs[i].y_pos - plr->y_pos;
    if (abs(x_diff) < game->attr && abs(y_diff) < game->attr) {
      int i = 0;
      while (game->itm_ownd[i].content == '\0')
        i++;
      strcpy(game->itm_ownd[i].name, ITEMs[i].name);
      strcpy(game->itm_ownd[i].desc, ITEMs[i].desc);
      game->itm_ownd[i].content = ITEMs[i].content;
      game->itm_ownd[i].dur = ITEMs[i].dur;
      game->itm_ownd[i].price = ITEMs[i].price;
      game->itm_ownd[i].rar = ITEMs[i].rar;
      game->itm_ownd[i].type = ITEMs[i].type;
      game->itm_ownd[i].val_inc = ITEMs[i].val_inc;

      ITEMs[i].x_pos = ITEMs[i].y_pos = -1;
      ITEMs[i].content = '\0';
    }
  }
  return 1;
}

int upd_mob_pos(char *canv, GAME *game, BOSS boss, SHIP *mobs, PROJ *mobprj, ITEM *ITEMs, SHIP *plr, int time, int reset) {
  (void)canv, (void)boss;

  if (game->g_st != boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      if (reset) mobs[i].hp = 0;

      if (time % BOSS_SPWNTIME >= BOSS_SPWNTIME - 100 && time % BOSS_SPWNTIME <= BOSS_SPWNTIME) mobs[i].hp = 0;

      if (!plr->is_blocked) {
        if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
          mobs[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
          mobs[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
        }
      }

      if (mobs[i].hp <= 0 && mobs[i].hurt_timer == 0) { mobs[i].hurt_timer += 10; }

      if (game->cur_blding != NONE && mobs[i].weap > 0) {
        mobs[i].weap = 0;
        mobs[i].dir = rand() % 4;
      }

      if (mobs[i].hurt_timer >= 1) {
        if (mobs[i].hurt_timer == 1) {
          if (rand() % 2 == BASE_ITEM_CHNC + game->ITEM_chance) {
            for (int x = 0; x < ITEM_BUFFER; x++) {
              if (ITEMs[x].content == '\0') {
                ITEM itm = game->itm_list[rand_range(0, game->num_items - 1)];
                ITEMs[x].x_pos = mobs[i].x_pos, ITEMs[x].y_pos = mobs[i].y_pos;
                ITEMs[x].type = itm.type, ITEMs[x].rar = itm.rar;
                strcpy(ITEMs[x].desc, itm.desc);
                strcpy(ITEMs[x].name, itm.name);
                ITEMs[x].content = itm.content;
                ITEMs[x].price = itm.price, ITEMs[x].val_inc = itm.val_inc;
                ITEMs[x].dur = itm.dur;

                break;
              }
            }
          }
          // int new_y = (-SHIP_H - 1) - ((CANV_H / CELL_SIZE) * rand_range(1, 5));
          // int new_x = C_WIDTH * (rand() % CELL_SIZE) + 2;

          mobs[i].y_pos += rand_range(-2000, 2000);
          mobs[i].x_pos += rand_range(-2000, 2000);
          // mobs[i].weap = 0;
          mobs[i].hp = (MOBS_BUFFER <= MOBS1_BUFFER ? MOBS_1_MAXHP : MOBS_2_MAXHP);
          mobs[i].hurt_timer = 0;
          mobs[i].weap = 0;
        } else {
          mobs[i].hurt_timer--;
          mobs[i].weap = 0;
          game->score += 10;
        }
        continue;
      }

      if (plr->hurt_timer == 0 && game->cur_blding == NONE) {
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

      // CHANGE DIR
      if (mobs[i].weap > 0)
        mobs[i].dir = get_dir_to_target(plr->x_pos, plr->y_pos, mobs[i].x_pos, mobs[i].y_pos, 1);
      else if (rand() % 50 == 0)
        mobs[i].dir = rand() % 4;

      // ATTACK
      if (mobs[i].weap && time % mobs[i].atk_speed == 0) {
        DIR d = mobs[i].dir;
        Init_bullet(mobprj, base, get_dir_to_target(plr->x_pos, plr->y_pos, mobs[i].x_pos, mobs[i].y_pos, 0), PRJ_MOB_BUFFER, mobs[i].x_pos + SHIP_W / 2, mobs[i].y_pos + SHIP_H / 2);
      }
      if (mobs[i].dir != none) {
        mobs[i].x_pos += mobs[i].dir == left || mobs[i].dir == up_left || mobs[i].dir == down_left ? -2 : mobs[i].dir == up || mobs[i].dir == down ? 0 : 2;
        mobs[i].y_pos += mobs[i].dir == up || mobs[i].dir == up_left || mobs[i].dir == up_right ? -1 : mobs[i].dir == left || mobs[i].dir == right ? 0 : 1;
      }
    }
  }
  return 1;
}
int upd_plr_proj(char *canv, GAME *game, PROJ *proj, SHIP *mobs, BOSS *boss, SHIP *plr, int time, int reset) {
  for (int i = 0; i < PROJ_BUFFER - 1; i++) {
    if (proj[i].y_pos <= -1 || proj[i].x_pos <= -1 || proj[i].y_pos >= CANV_H || proj[i].x_pos >= CANV_W || reset) {
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
              mobs[j].weap += 100;

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
    if (game->g_st != boss_fight) {
      proj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
      if (time % 2 == 0) proj[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    }
    PRJ_TYPES tp = proj[i].type;
    int speed = tp == base ? 1 : tp == magnet ? 3 : tp == bomb ? 6 : tp == big ? 1 : 1;
    if (game->g_st == boss_fight) speed = 1;

    if (time % speed == 0) {
      if (proj[i].dir != none) {
        proj[i].x_pos += proj[i].dir == left || proj[i].dir == up_left || proj[i].dir == down_left ? -2 : proj[i].dir == up || proj[i].dir == down ? 0 : 2;
        proj[i].y_pos += proj[i].dir == up || proj[i].dir == up_left || proj[i].dir == up_right ? -1 : proj[i].dir == left || proj[i].dir == right ? 0 : 1;
      }

      if (proj[i].type == big && rand() % 5 == 0) {
        int x_offset = 0;
        while (x_offset == 0)
          x_offset = rand_range(-2, 2);
        int y_offset = 0;
        while (y_offset == 0)
          y_offset = rand_range(-2, 2);
        Init_bullet(proj, proj[i].type, proj[i].dir, PROJ_BUFFER, proj[i].x_pos + x_offset, proj[i].y_pos + y_offset);
      }

      if (proj[i].type == magnet) {
        if (game->g_st == boss_fight) {
          proj[i].dir = get_dir_to_target(boss->x_pos, boss->y_pos, proj[i].x_pos, proj[i].y_pos, 1);
          return (1);
        } else {
          for (int j = 0; j < MOBS_BUFFER; j++) {
            SHIP m = mobs[j];
            if (m.hurt_timer) continue;
            if (abs(m.x_pos - proj[i].x_pos + 3) < 30 && abs(m.y_pos - proj[i].y_pos + 3) < 30) {
              proj[i].dir = get_dir_to_target(m.x_pos, m.y_pos, proj[i].x_pos, proj[i].y_pos, 1);
              break;
            }
          }
        }
      }
    }
  }
  return 1;
}

int upd_mob_prj(char *canv, GAME *game, PROJ *mobprj, SHIP *plr, int time, int reset) {
  for (int i = 0; i < PRJ_MOB_BUFFER - 1; i++) {
    int x = mobprj[i].x_pos, y = mobprj[i].y_pos;
    if (y <= -1 || x <= -1 || y >= CANV_H || x >= CANV_W || reset) {
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
        game->prj_col_index = game->g_st == boss_fight ? -2 : i;
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
    if (game->g_st != boss_fight) {
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
    else if (game->g_st == boss_fight && boss->action != die)
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

int upd_buildings(char *canv, BLDING *buildings, GAME *game, SHIP *plr, int time, int *has_moved) {
  (void)game;
  for (int i = 0; i < BLDING_BUFFER - 1; i++) {
    BLDING *bld = &buildings[i];
    bld->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
    if (time % 2 == 0) bld->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    int x = bld->x_pos, y = bld->y_pos, x_s = bld->x_size, y_s = bld->y_size;

    if (plr->x_pos >= x + x_s / 2 - 8 && plr->x_pos <= x + x_s / 2 + 8) {
      if (plr->y_pos == bld->y_pos + bld->y_size - 1 && game->cur_bld_index != -1) {
        game->cur_bld_index = -1;
        game->cur_floor = 0;
        game->cur_blding = NONE;
        game->is_in_dialog = -1;
        game->atk_type = press;
      } else if (plr->y_pos == bld->y_pos + bld->y_size - 2 && game->cur_bld_index == -1) {
        game->cur_bld_index = i;
        game->cur_blding = bld->type;
        game->atk_type = disabled;
        game->cur_floor = 0;
        plr->lk_dir = up;
      }
    }

    if (game->cur_bld_index == i) {
      char *txt = strdup(bld_labs[bld->type]);
      int tx_len = strlen(txt), ens_len = tx_len + 4;
      char *enseigne = init_blank_canv(ens_len, 3, 1, '#');
      write_on_canv(txt, enseigne, 2, 1);
      write_on_canv(enseigne, canv, bld->x_pos + 3, bld->y_pos + y_s - 1);

      if (bld->flr_am > 0) {
        int strt_ind = 0;
        char *flr_ens = init_blank_canv(9, 3, 1, '#');
        while (flr_ens[strt_ind] != '\n')
          strt_ind++;
        strt_ind += 2;
        int *iv = &strt_ind;
        set_label(flr_ens, iv, game->cur_floor == 0 ? "##0" : "##", game->cur_floor);
        *iv -= 4;
        set_label(flr_ens, iv, "/", bld->flr_am);
        write_on_canv(flr_ens, canv, bld->x_pos + 14, bld->y_pos + y_s - 1);
      }

      int x_up_str = game->cur_floor % 2 == 0 ? x_s + x - 10 : x_s + x - (x_s - 10);
      int x_down_str = game->cur_floor % 2 == 0 ? x_s + x - (x_s - 10) : x_s + x - 10;
      int psh_am = 1;

      if (game->cur_floor % 2 == 0) {
        if ((plr->x_pos + SHIP_W + 1 > x_s + x - 10 && plr->y_pos > y_s + y - 10) && bld->floors[game->cur_floor + 1] != NULL) {
          game->cur_floor++;
          bld->x_pos += psh_am;
          plr->lk_dir = left, plr->dir = left;
          has_moved = 0;
        } else if ((plr->x_pos < x_s + x - (x_s - 10) && plr->y_pos > y_s + y - 10) && game->cur_floor > 0) {
          game->cur_floor--;
          bld->x_pos -= psh_am;
          plr->lk_dir = right, plr->dir = right;
          has_moved = 0;
        }
      } else {
        if ((plr->x_pos + SHIP_W + 1 > x_s + x - 10 && plr->y_pos > y_s + y - 10) && game->cur_floor > 0) {
          game->cur_floor--;
          bld->x_pos += psh_am;
          plr->lk_dir = left, plr->dir = left;
          has_moved = 0;
        } else if ((plr->x_pos < x_s + x - (x_s - 10) && plr->y_pos > y_s + y - 10) && bld->floors[game->cur_floor + 1] != NULL) {
          game->cur_floor++;
          bld->x_pos -= psh_am;
          plr->lk_dir = right, plr->dir = right;
          has_moved = 0;
        }
      }
    }
  }
  return 1;
}

int upd_collisions(char *canv, GAME *game, PROJ *proj, BOSS *boss, SHIP *mobs, PROJ *mobprj, SHIP *plr) {

  int is_blocked = 0, col_dist = 1, x_offs = 0, char_to_look;
  int up_char = get_at_index(canv, CANV_W, CANV_H, plr->x_pos + x_offs, plr->y_pos - col_dist);
  int dwn_char = get_at_index(canv, CANV_W, CANV_H, plr->x_pos + x_offs, plr->y_pos + 1 + col_dist);
  int lft_char = get_at_index(canv, CANV_W, CANV_H, plr->x_pos - col_dist + x_offs - 2, plr->y_pos + col_dist);
  int rgt_char = get_at_index(canv, CANV_W, CANV_H, plr->x_pos + SHIP_W + col_dist + x_offs + 1, plr->y_pos + col_dist);

  int up_blk = (up_char == HOR_WALL || up_char == VRT_WALL || up_char == UPRGT_WALL || up_char == UPLFT_WALL || up_char == DWNLFT_WALL || up_char == DWNRGT_WALL);
  if (up_blk) write_on_canv("^", canv, plr->x_pos, plr->y_pos - col_dist);
  int dwn_blk = (dwn_char == HOR_WALL || dwn_char == VRT_WALL || dwn_char == UPRGT_WALL || dwn_char == UPLFT_WALL || dwn_char == DWNLFT_WALL || dwn_char == DWNRGT_WALL);
  if (dwn_blk) write_on_canv("v", canv, plr->x_pos, plr->y_pos + 2 + col_dist);
  int lft_blk = (lft_char == HOR_WALL || lft_char == VRT_WALL || lft_char == UPRGT_WALL || lft_char == UPLFT_WALL || lft_char == DWNLFT_WALL || lft_char == DWNRGT_WALL);
  if (lft_blk) write_on_canv("<", canv, plr->x_pos - col_dist, plr->y_pos + col_dist);
  int rgt_blk = (rgt_char == HOR_WALL || rgt_char == VRT_WALL || rgt_char == UPRGT_WALL || rgt_char == UPLFT_WALL || rgt_char == DWNLFT_WALL || rgt_char == DWNRGT_WALL);
  if (rgt_blk) write_on_canv(">", canv, plr->x_pos + SHIP_W + 1 + col_dist, plr->y_pos + col_dist);
  int hor_blk = lft_blk && rgt_blk;
  int ver_blk = up_blk && dwn_blk;

  if (up_blk && lft_blk)
    game->allowed_dir = down_right;
  else if (up_blk && rgt_blk)
    game->allowed_dir = down_left;
  else if (dwn_blk && lft_blk)
    game->allowed_dir = up_right;
  else if (dwn_blk && rgt_blk)
    game->allowed_dir = up_left;
  else if (hor_blk)
    game->allowed_dir = vert;
  else if (ver_blk)
    game->allowed_dir = hor;
  else if (up_blk)
    game->allowed_dir = hor_down;
  else if (dwn_blk)
    game->allowed_dir = hor_up;
  else if (lft_blk)
    game->allowed_dir = ver_right;
  else if (rgt_blk)
    game->allowed_dir = ver_left;
  else
    game->allowed_dir = all;

  if (!is_dir_allowed(plr->dir, game->allowed_dir)) {
    plr->dir = none;
    plr->is_blocked = 1;
  } else
    plr->is_blocked = 0;

  if (plr->is_blocked) plr->dir = none;
  return plr->is_blocked;
}

int handle_seller_menu(char *canv, GAME *game, NPC *seller, BLDING *buildings, SHIP *plr, SHIP *mobs, BOSS *boss, PROJ *mobprj) {
  int inp = -1;
  int wind_x = CANV_W - 10, wind_y = CANV_H - 10;

  char *window = init_blank_canv(wind_x, wind_y, 1, '#');
  char **items = (char *)malloc(sizeof(char *));

  for (int i = 0; i < SHOP_VIEW_ITEMS; i++) {
    items[i] = init_blank_canv(CANV_W-10, CANV_H-10, 1, '#');
    set_label(items[i], )
  }

  while (inp == -1) {
    inp = getchar();
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