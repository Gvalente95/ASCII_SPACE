#include "../headers/includes.h"

const char *bld_labs[] = {"SHOP", "ARMORY", "HOUSE", "FORTRESS", "NONE", "BLDING_COUNT"};
const char *rar_labs[] = {"COMMON", "UNCOMMON", "RARE", "MYSTIC", "ARTIFACT", "DIVINE", "DIABOLICAL", "RARITY_COUNT"};

const char *itm_type_lbs[] = {"MAP", "MATERIAL", "WEAPON", "ARMOR", "EXPANDABLE", "ITEM_TYPE_COUNT"};

int launch_proj(GAME *game, SHIP *plr, PROJ *proj) {
  if (game == NULL || plr == NULL || proj == NULL) return 0;
  plr->atk_reload = plr->atk_spd;
  if (plr->weap > 0 && game->shots_fired > 0) game->shots_fired--;
  if (game->shots_fired == 0) {
    if (plr->weap > 0) plr->weap--;
    game->shots_fired = shot_lock;
  }
  int prj_x;
  int prj_y;
  int x_dist_offs = 2;
  int y_dist_offs = 1;

  DIR dir = get_prj_dir(plr->atk_am, plr->atk_am, *plr, plr->lk_dir);
  prj_x = (dir == up || dir == down ? plr->x_pos + SHIP_W / 2 : dir == left || dir == up_left ? plr->x_pos - 3 - x_dist_offs : plr->x_pos + SHIP_W + 3 + x_dist_offs);
  prj_y = dir == up || dir == up_left || dir == up_right         ? plr->y_pos - 1 - y_dist_offs
          : dir == down || dir == down_left || dir == down_right ? plr->y_pos + SHIP_H / 2 + y_dist_offs
          : dir == left || dir == right                          ? plr->y_pos + 1
                                                                 : plr->y_pos - 1;
  Init_bullet(proj, game->cur_proj, game->mv_type == watch_up ? up : dir, PROJ_BUFFER, prj_x, prj_y);

  int w_prj = plr->atk_am - 1;
  int i = 0;
  for (int x = w_prj; x > 0; x++) {
    if (w_prj <= 0 || i + x > PROJ_BUFFER - 1) break;
    if (proj[i + x].x_pos == -1 && proj[i + x].y_pos == -1) continue;
    DIR dir = get_prj_dir(w_prj, plr->atk_am, *plr, plr->lk_dir);
    prj_x = proj->x_pos + (dir == up || dir == down ? 1 : 0);
    prj_y = proj->y_pos + (dir == left || dir == right ? 1 : 0);
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
    if (!is_in_string(c, "wasdpm12gh<q \033\t")) plr->dir = none;
    switch (c) {
      case '\t':
        game->cur_proj++;
        if (game->cur_proj == PRJ_TYPES_COUNT) game->cur_proj = 0;
        return ('t');
      case '\033':
        getchar();
        char arrow = getchar();
        DIR lk_dir = arrow == 'A' ? up : arrow == 'B' ? down : arrow == 'C' ? right : left;
        if (is_dir_allowed(lk_dir, game->allowed_lk_dir)) plr->lk_dir = lk_dir;
        return (arrow == 'A' ? '^' : arrow == 'B' ? '$' : arrow == 'C' ? '<' : '>');
      case ' ':
        if (game->atk_type != disabled && !plr->atk_reload) {
          play_sound(AU_change_sel, 0);
          launch_proj(game, plr, proj);
        }
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
          plr->atk_am = 2, plr->atk_pow = 3, plr->atk_spd = 3, plr->hp = 50, plr->weap = 8;
        else
          plr->atk_am = 1, plr->atk_pow = 1, plr->atk_spd = 1, plr->hp = 5, plr->weap = 1;
        return (c);
      case 'q':
        return (c);
      case '<':
        game->is_framing = 1;
        return (c);
    }
    return (c);
  }
  return (c);
}

void upd_game_state(char *canv, GAME *game, SHIP *plr, BOSS *boss, SHIP *mobs, PROJ *proj, PROJ *mobprj, ITEM *items, BLDING *buildings, STAR *stars, int has_moved, int time, int *wind) {
  if (canv == NULL || game == NULL || plr == NULL || boss == NULL || mobs == NULL || proj == NULL || mobprj == NULL || items == NULL || buildings == NULL || stars == NULL) return;

  if (time % circ_add_interval == 0) Init_bullet(proj, game->cur_proj, circular, PROJ_BUFFER, 0, 0);
  upd_player(canv, game, plr, proj, has_moved, time);
  upd_mob_pos(canv, game, *boss, mobs, mobprj, items, plr, time, 0);

  upd_boss_fight(canv, game, boss, mobs, mobprj, items, plr, time);
  if (game->cur_blding == NONE) {
    upd_background(canv, stars, *game, *boss, *plr, has_moved, time);
    upd_items(canv, game, plr, items, time, 0);
    upd_plr_proj(canv, game, proj, mobs, boss, plr, time, 0);
    upd_mob_prj(canv, game, mobprj, plr, time, 0);
    if (plr->dir != none) upd_wind(canv, wind, game, plr, boss);
  }
  int r = upd_buildings(canv, buildings, game, plr, time, &has_moved);
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
    game->gl_x_pos += plr->dir == right ? plr->spd : plr->dir == left ? -plr->spd : 0;
    game->gl_y_pos += plr->dir == down ? plr->spd : plr->dir == up ? -plr->spd : 0;
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
int upd_items(char *canv, GAME *game, SHIP *plr, ITEM *items, int time, int reset) {
  for (int i = 0; i < ITEM_BUFFER; i++) {
    ITEM *itm = &items[i];
    if (reset) itm->x_pos = itm->y_pos = -1;
    if ((itm->x_pos == -1 && itm->y_pos == -1) || itm->content == '\0') continue;

    if (!plr->is_blocked) {
      if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        itm->center_x += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        itm->center_y += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
    }
    double radius = 1;
    double angular_velocity = 0.1;
    itm->x_pos = itm->center_x + radius * cos(angular_velocity * time);
    itm->y_pos = itm->center_y + (radius * 0.5) * sin(angular_velocity * time);

    char *itm_cube = malloc(4);
    itm_cube[0] = '\'';
    itm_cube[1] = itm->content;
    itm_cube[2] = '\'';
    itm_cube[3] = '\0';

    int rar_value = itm->rar;
    COLOR col = rar_value;
    Color_from_index(rar_value, &itm_cube);
    write_on_canv(itm_cube, canv, itm->x_pos, itm->y_pos);
    free(itm_cube);
    int x_diff = itm->x_pos - plr->x_pos;
    int y_diff = itm->y_pos - plr->y_pos;

    if (abs(x_diff) < game->attr && abs(y_diff) < game->attr) {
      if (game->owned_amnt < items_OWNED_BUFFER - 1) {
        int inv = 0, is_dupl = 0;
        while (game->itm_ownd[inv].content != '\0' && inv < items_OWNED_BUFFER - 1) {
          if (game->itm_ownd[inv].content == itm->content) {
            game->itm_ownd[inv].am++;
            is_dupl = 1;
            break;
          }
          inv++;
        }
        if (!is_dupl) {
          Copy_Item(&game->itm_ownd[game->owned_amnt], *itm);
          game->itm_ownd[game->owned_amnt].am = 1;
          game->owned_amnt++;
        }
        game->inv_incrmnt = inv;
        game->itm_ownd[inv].center_x += 30;
        reset_item(itm);
        play_sound(AU_high_pitch, 0);
      }
    }
  }
  return 1;
}

int upd_mob_pos(char *canv, GAME *game, BOSS boss, SHIP *mobs, PROJ *mobprj, ITEM *items, SHIP *plr, int time, int reset) {
  (void)canv, (void)boss;

  if (game->g_st != boss_fight) {
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      SHIP *m = &mobs[i];
      if (reset) m->hp = 0;

      /*
            if (time % 500 == 0) {
              m->hp++;
              m->atk_spd--, m->spd--;
              if (m->spd <= 6) m->spd = 7;
              if (m->atk_spd <= 0) m->atk_spd = 1;
            }*/

      if (time % BOSS_SPWNTIME >= BOSS_SPWNTIME - 100 && time % BOSS_SPWNTIME <= BOSS_SPWNTIME) m->hp = 0;

      if (!plr->is_blocked) {
        if (time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
          m->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
          m->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
        }
      }
      // HANDLE DEATH
      if (m->hp <= 0 && m->death_timer == 0) { m->death_timer += MOB_DEATH_DUR; }
      if (m->death_timer >= 1) {
        if (m->death_timer == 1) {
          play_sound(AU_expl2, 0);
          int x_offs = 0, y_offs = 0;
          for (int x = 0; x < ITEM_BUFFER - 1; x++) {
            int dist_btwn = 10;
            if (items[x].content == '\0' && items[x].x_pos == -1 && items[x].y_pos == -1) {
              int itm_rar = determineRarity(game->luck);
              ITEM new_item;
              new_item.content = '\0';
              int tries = 0;
              do {
                (new_item = get_item(itm_rar, rand_range(0, ITEM_Type_COUNT - 1), game->itm_list, game->num_items));
                tries++;
              } while (new_item.content == '\0' && tries < 200);
              Copy_Item(&items[x], new_item);
              items[x].center_x = m->x_pos + x_offs, items[x].center_y = m->y_pos + y_offs;
              items[x].x_pos = m->x_pos + y_offs, items[i].y_pos = m->y_pos;
              x_offs += rand_range(-3, 3), y_offs += rand_range(-3, 3);
              if (rand_range(0, game->luck) == 0) break;
            }
          }

          m->y_pos += rand_range(-2000, 2000);
          m->x_pos += rand_range(-2000, 2000);
          m->hp = (MOBS_BUFFER <= MOBS1_BUFFER ? MOBS_1_MAXHP : MOBS_2_MAXHP);
          m->death_timer = 0;
          m->weap = 0;
        } else {
          m->death_timer--;
          m->weap = 0;
          game->score += 10;
        }
        continue;
      }

      // HANDLE PLR COLLISION
      if (plr->hurt_timer == 0 && game->cur_blding == NONE) {
        if (abs(m->y_pos - plr->y_pos) < 2 && abs(m->x_pos - plr->x_pos) < 2) {
          if (plr->shield) {
            plr->shield--;
            m->hp -= 2;
          } else
            plr->hp--;
          game->plr_col_index = i;
          plr->hurt_timer = HURT_DUR;
        }
      }

      if (game->cur_blding != NONE && m->weap > 0) {
        m->weap = 0;
        m->dir = rand() % 4;
      }

      if (time % m->spd != 0) continue;

      // CHANGE DIR
      if (m->weap > 0)
        m->dir = get_dir_to_target(plr->x_pos, plr->y_pos, m->x_pos, m->y_pos, 1);
      else if (rand() % 50 == 0)
        m->dir = rand() % 4;

      // ATTACK
      if (m->weap && time % m->atk_spd == 0) {
        DIR d = m->dir;
        Init_bullet(mobprj, base, get_dir_to_target(plr->x_pos, plr->y_pos, m->x_pos, m->y_pos, 0), PRJ_MOB_BUFFER, m->x_pos + SHIP_W / 2, m->y_pos + SHIP_H / 2);
      }
      if (m->dir != none) {
        m->x_pos += m->dir == left || m->dir == up_left || m->dir == down_left ? -2 : m->dir == up || m->dir == down ? 0 : 2;
        m->y_pos += m->dir == up || m->dir == up_left || m->dir == up_right ? -1 : m->dir == left || m->dir == right ? 0 : 1;
      }
    }
  }
  return 1;
}
int upd_plr_proj(char *canv, GAME *game, PROJ *proj, SHIP *mobs, BOSS *boss, SHIP *plr, int time, int reset) {
  for (int i = 0; i < PROJ_BUFFER - 1; i++) {
    PROJ *p = &proj[i];
    if (p->y_pos <= -1 || p->x_pos <= -1 || p->y_pos >= CANV_H || p->x_pos >= CANV_W || reset || p->dur == 0) {
      p->x_pos = p->y_pos = -1, p->dur = -1;
      continue;
    }
    if (p->dur > -1) p->dur--;
    int index = p->x_pos + ((CANV_W + 1) * p->y_pos);
    if (p->col_timer) {
      p->col_timer--;
      if (p->type == bomb) {
        if (p->dur == -1) {
          p->col_timer = 0;
          int radius = 2;
          play_sound(AU_expl4, 0);

          for (int y = -2; y <= 2; y++) {
            for (int x = -4; x <= 4; x++) {
              if (x == 0 && y == 0 || rand() % 5 == 0) continue;
              if ((x / 2) * (x / 2) + y * y > radius * radius) continue;

              int x_p = p->x_pos + x, y_p = p->y_pos + y;
              int new_p = Init_bullet(proj, bomb, none, PROJ_BUFFER, x_p, y_p);
              proj[new_p].dur = rand_range(20, 50);
            }
          }
        } else {
          if (rand() % 20 == 0) p->dur = 0;
          if (rand() % 50 == 0) {
            int new_p = Init_bullet(proj, p->type, none, PROJ_BUFFER, p->x_pos + rand_range(-2, 2), p->y_pos + rand_range(-2, 2));
            proj[new_p].dur = rand_range(5, 20);
          }
        }
      }
      if (p->col_timer == 0) {
        p->x_pos = p->y_pos = -1, p->type = base, p->dir = none, p->dur = -1;
        continue;
      }
      if (time % 5 == 0) {
        p->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        p->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
      canv[index] = PRJ_COL;
      continue;
    }
    if (canv[index] && canv[index] != '\n') {
      if (p->type == bomb && canv[index] != ' ') p->col_timer = 15;
      if (abs(boss->y_pos - p->y_pos) < boss->height && abs((boss->x_pos + (boss->width / 2)) - p->x_pos) < boss->width / 2) {
        int dmg = plr->atk_pow + (boss->action == SP_attack2 || boss->action == SP_attack ? 10 : 0);
        if (boss->shield > 0) {
          boss->shield -= dmg;
        } else {
          boss->cur_hp -= dmg;
          boss->shield = 0;
        }
        game->prj_col_index = -2;
        p->col_timer = 15;
      } else {
        for (int j = 0; j < MOBS_BUFFER; j++) {

          if (mobs[j].hurt_timer || mobs[j].x_pos == -1) continue;
          if (abs((mobs[j].y_pos + 2) - p->y_pos) < 5 && abs((mobs[j].x_pos + 2) - p->x_pos) < 2) {
            if (p->type == ricochet) {
              play_sound(AU_high_pitch, 0);
              p->dur += 10;
            } else {
              p->col_timer = 15;
              play_sound(AU_click2, 0);
              if (p->type == bomb) { p->x_pos = mobs[j].x_pos + 2, p->y_pos = mobs[j].y_pos + 2; }
              p->dir = none;
            }

            mobs[j].hp -= plr->atk_pow;
            mobs[j].weap += 100;
            mobs[j].hurt_timer += MOB_HURT_DUR;

            break;
          }
        }
      }
      char ic;
      PRJ_TYPES t = p->type;
      switch (p->dir) {
        case up:
        case down:
          ic = t == base ? PRJ_BS_VER : t == magnet ? PRJ_MAGN_VER : t == bomb ? PRJ_BMB_VER : t == big ? PRJ_BIG_VER : t == ricochet ? PRJ_RIC_VER : PRJ_BS_VER;
          break;
        case left:
        case right:
          ic = t == base ? PRJ_BS_HOR : t == magnet ? PRJ_MAGN_HOR : t == bomb ? PRJ_BMB_HOR : t == big ? PRJ_BIG_HOR : t == ricochet ? PRJ_RIC_HOR : PRJ_BS_HOR;
          break;
        case up_left:
        case down_right:
          ic = t == base ? PRJ_BS_DIAG_L : t == magnet ? PRJ_MAGN_DIAG_L : t == bomb ? PRJ_BMB_DIAG_L : t == big ? PRJ_BIG_DIAG_L : t == ricochet ? PRJ_RIC_DIAG_L : PRJ_BS_DIAG_L;
          break;
        default:
          ic = t == base ? PRJ_BS_DIAG_R : t == magnet ? PRJ_MAGN_DIAG_R : t == bomb ? PRJ_BMB_DIAG_R : t == big ? PRJ_BIG_DIAG_R : t == ricochet ? PRJ_RIC_DIAG_R : PRJ_BS_DIAG_R;
          break;
      }
      canv[index] = ic;
      if (p->prv_index > 0 && p->prv_index < strlen(canv))
        if (canv[p->prv_index != ' ' && canv[p->prv_index] != '_']) canv[p->prv_index] = ic;
      p->prv_index = index;
    }

    if (game->g_st != boss_fight) {
      p->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
      if (time % 2 == 0) p->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    }
    PRJ_TYPES tp = p->type;
    int speed = tp == base ? 1 : tp == magnet ? 1 : tp == bomb ? 6 : tp == big ? 1 : 1;
    if (game->g_st == boss_fight) speed = 1;

    if (p->dir == circular) {
      double radius = 10;
      p->x_pos = plr->x_pos + (SHIP_W / 2) + radius * cos(p->angular_velocity * (time + p->angular_offset));
      p->y_pos = plr->y_pos + (SHIP_H / 2 - 2) + (radius * 0.5) * sin(p->angular_velocity * (time + p->angular_offset));
    }
    if (time % speed == 0) {
      if (p->dir != none) {
        p->x_pos += p->dir == left || p->dir == up_left || p->dir == down_left ? -2 : p->dir == up || p->dir == down ? 0 : 2;
        p->y_pos += p->dir == up || p->dir == up_left || p->dir == up_right ? -1 : p->dir == left || p->dir == right ? 0 : 1;
      }
      if (p->type == big && rand() % 20 == 0 && i < (p->dir == circular ? 10 : PROJ_BUFFER - 1)) {
        int r = rand_range(-2, 2);
        int new_p = Init_bullet(proj, p->type, p->dir, PROJ_BUFFER, p->x_pos + (r == 0 ? -1 : r), p->y_pos + (r == 0 ? -1 : r));
        proj[new_p].dur = rand_range(5, 20);
      }

      if (p->type == magnet || (p->type == ricochet && p->dur)) {
        if (game->g_st == boss_fight) {
          p->dir = get_dir_to_target(boss->x_pos, boss->y_pos, p->x_pos, p->y_pos, 1);
          return (1);
        } else {
          for (int j = 0; j < MOBS_BUFFER; j++) {
            SHIP m = mobs[j];
            if (m.hurt_timer) continue;
            if (abs(m.x_pos - p->x_pos + 3) < 30 && abs(m.y_pos - p->y_pos + 3) < 30) {
              p->dir = get_dir_to_target(m.x_pos, m.y_pos, p->x_pos, p->y_pos, 1);
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
  int ret_value = 1;

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
        ret_value = 2;
      } else if (plr->y_pos == bld->y_pos + bld->y_size - 2 && game->cur_bld_index == -1) {
        game->cur_bld_index = i;
        game->cur_blding = bld->type;
        game->atk_type = disabled;
        game->cur_floor = 0;
        plr->lk_dir = up;
        ret_value = 3;
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
  return ret_value;
}

int upd_collisions(char *canv, GAME *game, PROJ *proj, BOSS *boss, SHIP *mobs, PROJ *mobprj, SHIP *plr) {

  int is_blocked = 0, col_dist = 1, x_offs = 0, char_to_look;
  int up_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos + x_offs, plr->y_pos - col_dist);
  int dwn_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos + x_offs, plr->y_pos + 1 + col_dist);
  int lft_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos - col_dist + x_offs - 2, plr->y_pos + col_dist);
  int rgt_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos + SHIP_W + col_dist + x_offs + 1, plr->y_pos + col_dist);

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

ITEM **filter_items_by_type(ITEM *base, int type_sel, int max_items, int *filtered_count) {
  ITEM **itms = (ITEM **)malloc(max_items * sizeof(ITEM *));
  if (!itms) return NULL;
  int bs_index = 0, itm_i = 0;
  while (itm_i < max_items && base[bs_index].content) {
    if (base[bs_index].type == type_sel && base[bs_index].am >= 1) itms[itm_i++] = &base[bs_index];
    bs_index++;
  }
  if (filtered_count) *filtered_count = itm_i;
  return itms;
}

int handle_seller_menu(char *canv, GAME *game, NPC *seller, BLDING *buildings, SHIP *plr, SHIP *mobs, BOSS *boss, PROJ *mobprj) {
  int wind_x = 6, wind_y = 3;
  int wind_width = (CANV_W - (wind_x * 2)), wind_height = CANV_H - (wind_y * 2);
  int msg_width = wind_width - 15, msg_height = 6;
  int itm_zone_xP = 5, itm_zone_yP = 5;
  int y_dist = -1, x_dist = 5, inp = -1;
  int sel = 0, page = 0, obj_sel = 0, type_sel = 0;
  int is_at_end = 0;
  ITEM *sel_itm;

  while (inp != 'q') {
    char *window = init_blank_canv(wind_width, wind_height, 1, '/');
    char *interior = init_blank_canv(wind_width - 2, wind_height - 2, 1, '#');
    write_on_canv(interior, window, 1, 1);

    char *buy_SQ = init_blank_canv(5, 3, sel == -1 ? 1 : page == 0 ? 2 : 0, ' '), *sell_SQ = init_blank_canv(6, 3, sel == -2 ? 1 : page == 1 ? 2 : 0, ' ');
    char *discuss_SQ = init_blank_canv(6, 3, sel == -3 ? 1 : page == 2 ? 2 : 0, ' ');
    write_on_canv("BUY", buy_SQ, 1, 1), write_on_canv("SELL", sell_SQ, 1, 1), write_on_canv("DISCUSS", discuss_SQ, 1, 1);
    write_on_canv(buy_SQ, window, 5, 2), write_on_canv(sell_SQ, window, 15, 2), write_on_canv(discuss_SQ, window, 25, 2);
    free(buy_SQ), free(sell_SQ), free(discuss_SQ);

    if (sel >= 0) {
      char *itm_zone = init_blank_canv(wind_width - itm_zone_xP * 2, wind_height - itm_zone_yP - 3, 1, '#');
      write_on_canv(itm_zone, window, itm_zone_xP, itm_zone_yP);
    }

    char fortune[16];
    sprintf(fortune, "$%d", game->currencies[0].amount);
    write_on_canv(fortune, window, wind_width - 10, 3);

    if (page == 2) {
      int seller_x = wind_width / 2, seller_y = wind_height / 2;
      char *text = strdup(slr_dlgs[rand_range(0, 12)]);
      char *bub = get_text_bubble(text);
      write_on_canv(bub, window, seller_x - get_width(text) / 2, seller_y - get_height(bub));
      write_on_canv(seller_idle_down1, window, seller_x, seller_y);
      char *table = init_blank_canv(20, 5, 2, '.');
      write_on_canv(table, window, seller_x - 10, seller_y + 3);
      free(bub), free(text), free(table);
    } else {
      for (int i = 0; i < ITEM_Type_COUNT; i++) {
        int x_dist = 15;
        char *lbl_txt = strdup(itm_type_lbs[i]);
        int size = strlen(lbl_txt);
        char *lbl_sq = init_blank_canv(x_dist - 2, 3, i == type_sel ? 1 : 0, '#');
        write_on_canv(lbl_txt, lbl_sq, ((x_dist - 2) / 2) - (size / 2), 1);
        write_on_canv(lbl_sq, window, itm_zone_xP + 2 + i * x_dist, itm_zone_yP + 1);
        free(lbl_sq), free(lbl_txt);
      }
      int filtered_count;
      ITEM *base = page == 0 ? seller->items : game->itm_ownd;
      ITEM **itms = filter_items_by_type(base, type_sel, page == 0 ? SHOP_INVENTORY - 1 : items_OWNED_BUFFER - 1, &filtered_count);
      if (sel >= filtered_count) sel = MAX(filtered_count - 1, 0);

      int show_am = 13;
      for (int i = 0; i < show_am && i + obj_sel < filtered_count; i++) {
        obj_sel = MAX(sel - show_am + 1, 0);
        ITEM *itm = itms[i + obj_sel];

        if (itm->content == '\0') {
          if (sel == i) is_at_end = 1;
          continue;
        } else {
          int is_sel = i == sel || (i == show_am - 1 && sel >= show_am - 1) ? 1 : 0;
          if (is_sel) sel_itm = itms[i + obj_sel];
          char *item_bg = init_blank_canv(msg_width, is_sel ? msg_height : msg_height - 2, is_sel ? 1 : 0, '#');
          int x = is_sel ? 6 : 3;
          int st_index = get_index(item_bg, x, 1);
          char *type_txt = strdup(itm_type_lbs[itm->type]);
          char *name = strdup(itm->name);
          char *desc = strdup(itm->desc);
          Color_from_index((int)itm->rar, &name);
          set_label(item_bg, &st_index, "x", itm->am);
          set_label(item_bg, &st_index, name, '\0');
          st_index = get_index(item_bg, x, 2);
          set_label(item_bg, &st_index, type_txt, '\0');
          if (is_sel) {
            st_index = get_index(item_bg, x, 3);
            set_label(item_bg, &st_index, desc, '\0');
            st_index = get_index(item_bg, x, 4);
            set_label(item_bg, &st_index, "$", page == 0 ? itm->price : itm->price / 2);
          }
          int x_pos = itm_zone_xP + 3;
          int y_pos = 4 + itm_zone_yP + (i * (y_dist + msg_height - 2));
          if (i > sel) y_pos += 2;
          write_on_canv(item_bg, window, x_pos, y_pos);
          free(item_bg), free(type_txt), free(name), free(desc);
        }
      }
      if (itms != NULL) free(itms);
    }

    write_on_canv(window, canv, wind_x, wind_y);
    render_canvas(canv, game, 0);
    inp = -1;
    while (inp == -1)
      inp = getchar();
    if (is_in_string(inp, "wasd")) play_sound(AU_click1, 0);
    if (inp == 'q') play_sound(AU_high_pitch, 0);
    if (inp == ' ') play_sound(AU_change_sel, 0);

    sel += inp == 's' ? 1 : inp == 'w' ? sel == 0 ? -1 - page : -1 : 0;
    if (sel <= -1) {
      sel += inp == 'd' ? -1 : inp == 'a' ? 1 : 0;
      if (inp == 's') sel = 0;
    } else if (inp == 'd' || inp == 'a')
      type_sel += inp == 'd' ? 1 : inp == 'a' ? -1 : 0;
    if (is_at_end && inp == 's') { sel--, is_at_end = 0; }
    sel = MAX(sel, -3);
    type_sel = MAX(type_sel, 0), type_sel = MIN(type_sel, ITEM_Type_COUNT - 1);

    if (sel == -1) page = 0;
    if (sel == -2) page = 1;
    if (sel == -3) page = 2;

    if (inp == ' ') {

      if (sel >= 0 && sel_itm->content != '\0') {
        char action[5];
        page == 0 ? strcpy(action, "buy") : strcpy(action, "sell");
        int price = sel_itm->price;
        char txt[100] = "Huh? You're too broke for that item!";
        char *name = strdup(sel_itm->name);
        Color_from_index((int)sel_itm->rar, &name);
        name[10] = '\0';
        if ((game->currencies[0].amount >= price && page == 0) || page == 1) sprintf(txt, "%s %s for %d$ ?", action, name, sel_itm->price);
        char *box = init_blank_canv(get_width(txt) + 4, 5, 1, '#');
        write_on_canv(txt, box, 2, 2);
        write_on_canv(box, window, wind_width / 2 - get_width(box) / 2, wind_height / 2 - get_height(box) / 2);
        write_on_canv(window, canv, wind_x, wind_y);
        render_canvas(canv, game, 0);
        free(box), free(name);
        int np = -1;
        if ((game->currencies[0].amount < price && page == 0)) {
          usleep(500000);
          np = '?';
        }
        while (np == -1)
          np = getchar();

        if (is_in_string(inp, "wasd")) play_sound(AU_click1, 0);
        if (inp == 'q') play_sound(AU_high_pitch, 0);
        if (inp == ' ') play_sound(AU_change_sel, 0);

        if (np == ' ') {
          if (page == 0) {
            int dupl = (Get_duplicate_index(*sel_itm, game->itm_ownd, items_OWNED_BUFFER - 1));
            if (dupl)
              game->itm_ownd[dupl].am++;
            else
              Copy_Item(&game->itm_ownd[game->owned_amnt], *sel_itm);
            game->currencies[0].amount -= sel_itm->price;
            game->owned_amnt++;
            sel_itm->am--;
          }
          if (page == 1) {
            game->currencies[0].amount += sel_itm->price / 2;
            for (int x = 0; x < game->owned_amnt; x++) {
              ITEM *itm = &game->itm_ownd[x];
              if (itm->name == sel_itm->name) itm->am--;
            }
          }
        }
      }
    }

    if (inp == 'q' && sel >= 0) {
      inp = '?';
      sel = -1 - page;
    }

    usleep(50000);
    if (window != NULL) free(window);
    if (interior != NULL) free(interior);
  }
  // reorder_list(game->itm_ownd, items_OWNED_BUFFER - 1);
  // reorder_list(seller->items, SHOP_INVENTORY - 1);
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