#include "../../headers/1_includes.h"

int launch_proj(SHIP *plr, PROJ *proj) {
  if (plr == NULL || proj == NULL) return 0;
  plr->atk_reload = plr->atk_spd;
  if (plr->weap > 0 && GLB_shots_fired > 0) GLB_shots_fired--;
  if (GLB_shots_fired == 0) {
    if (plr->weap > 0) plr->weap--;
    GLB_shots_fired = shot_lock;
  }
  int prj_x;
  int prj_y;
  int x_dist_offs = 2;
  int y_dist_offs = 1;

  DIR dir = get_prj_dir(plr->atk_am, plr->atk_am, *plr, plr->lk_dir);
  prj_x = (dir == up || dir == down ? plr->x_pos + PLR_SHIP_W / 2 : dir == left || dir == up_left ? plr->x_pos - 3 - x_dist_offs : plr->x_pos + PLR_SHIP_W + 3 + x_dist_offs);
  prj_y = dir == up || dir == up_left || dir == up_right         ? plr->y_pos - 1 - y_dist_offs
          : dir == down || dir == down_left || dir == down_right ? plr->y_pos + PLR_SHIP_H / 2 + y_dist_offs
          : dir == left || dir == right                          ? plr->y_pos + 1
                                                                 : plr->y_pos - 1;
  Init_bullet(proj, GLB_cur_proj, GLB_mv_typ == watch_up ? up : dir, PROJ_BUFF_SIZE, prj_x, prj_y);

  int w_prj = plr->atk_am - 1;
  int i = 0;
  for (int x = w_prj; x > 0; x++) {
    if (w_prj <= 0 || i + x > PROJ_BUFF_SIZE - 1) break;
    if (proj[i + x].x_pos == -1 && proj[i + x].y_pos == -1) continue;
    DIR dir = get_prj_dir(w_prj, plr->atk_am, *plr, plr->lk_dir);
    prj_x = proj->x_pos + (dir == up || dir == down ? 1 : 0);
    prj_y = proj->y_pos + (dir == left || dir == right ? 1 : 0);
    Init_bullet(proj, GLB_cur_proj, dir, PROJ_BUFF_SIZE, prj_x, prj_y);
    w_prj--;
  }
  return (1);
}

void upd_game_state(char *canv, SHIP *plr, BOSS *boss, int has_moved, int *wind) {
  int is_in_fight = 0;

  // if ( % circ_add_interval == 0) Init_bullet(proj, GLB_cur_proj, circular, PROJ_BUFF_SIZE, 0, 0);
  upd_player(canv, plr, has_moved);
  upd_mobs(plr, 0);

  upd_boss_fight(canv, boss, plr);
  if (GLB_cur_blding == NONE) {
    upd_background(canv, *boss, *plr, has_moved);
    upd_items(canv, plr, 0);
    is_in_fight = upd_plr_proj(canv, boss, plr, 0) == 2;
    upd_mob_prj(canv, plr, 0);
    if (plr->dir != none) upd_wind(canv, wind, plr, boss);
  }
  int r = upd_buildings(canv, plr, &has_moved);
}
int upd_player(char *canv, SHIP *plr, int has_moved) {
  if (plr->atk_reload > 0) plr->atk_reload--;
  if (GLB_atk_typ == autom && !plr->atk_reload) launch_proj(plr, gmo.proj);

  int left_col = plr->x_pos - 2, right_col = plr->x_pos + PLR_SHIP_W + 2;
  int up_col = plr->y_pos - CANV_W - 1, down_col = plr->y_pos + ((CANV_W + 1) * (PLR_SHIP_H + 2));

  // SET PLR POS ON CANVAS
  switch (GLB_mv_typ) {
    case normal:
    case cutscene:
      if (GLB_cur_blding != NONE && has_moved <= 0) plr->dir = none;
      if (GLB_time % 4 == 0) {
        int y_center = GLB_game_state == boss_fight ? PLR_CENTER_Y_BOSS : PLR_CENTER_Y;
        if (plr->x_pos != PLR_CENTER_X) plr->x_pos += (plr->x_pos < PLR_CENTER_X + 1 ? 1 : -1);
        if (plr->y_pos != y_center) plr->y_pos += (plr->y_pos < y_center ? 1 : -1);
      }
      break;
    case on_grid:
      if (has_moved && is_in_canv(plr->x_pos, plr->y_pos, PLR_SHIP_W, PLR_SHIP_H, 0, 0, CANV_W, CANV_H, plr->dir)) {
        if (GLB_time % 2 == 0) plr->y_pos += plr->dir == down ? 1 : plr->dir == up ? -1 : 0;
        plr->x_pos += plr->dir == right ? 1 : plr->dir == left ? -1 : 0;
        plr->lk_dir = plr->dir;
      }
      if (has_moved <= 0) plr->dir = none;
      break;
    case free_slide:
    case watch_up:
      if (is_in_canv(plr->x_pos, plr->y_pos, PLR_SHIP_W, PLR_SHIP_H, 0, 0, CANV_W, CANV_H, plr->dir)) plr->x_pos += plr->dir == right ? 1 : plr->dir == left ? -1 : 0;
      break;
    default:
      break;
  }
  // SET GLOB POSITION
  if (GLB_mv_typ != on_grid) {
    GLB_gl_x_pos += plr->dir == right ? plr->spd : plr->dir == left ? -plr->spd : 0;
    GLB_gl_y_pos += plr->dir == down ? plr->spd : plr->dir == up ? -plr->spd : 0;
  }
  return (1);
}

int upd_background(char *canv, BOSS boss, SHIP plr, int has_moved) {
  STAR *stars = gmo.stars;
  for (int i = 0; i < STAR_BUFF_SIZE - 1; i++) {
    if (!plr.is_blocked) {
      if (GLB_time % stars[i].z_pos == 0) {
        if ((GLB_mv_typ != on_grid && GLB_mv_typ != free_slide) || has_moved) {
          if (GLB_time % (plr.dir == left || plr.dir == right ? stars[i].z_pos / 2 : stars[i].z_pos) == 0) {
            stars[i].x_pos += plr.dir == left ? 1 : plr.dir == right ? -1 : 0;
            stars[i].y_pos += plr.dir == up ? 1 : plr.dir == down ? -1 : 0;
          }
        }
        if (GLB_game_state == boss_fight) {
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

int upd_items(char *canv, SHIP *plr, int reset) {
  ITEM *items = gmo.items;
  for (int i = 0; i < ITEM_DROPS_BUFF_SIZE; i++) {
    ITEM *itm = &items[i];
    if (reset) itm->x_pos = itm->y_pos = -1;
    if ((itm->x_pos == -1 && itm->y_pos == -1) || itm->content == '\0') continue;

    if (!plr->is_blocked) {
      if (GLB_time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        itm->center_x += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        itm->center_y += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
    }
    double radius = 1;
    double angular_velocity = 0.1;
    itm->x_pos = itm->center_x + radius * cos(angular_velocity * GLB_time);
    itm->y_pos = itm->center_y + (radius * 0.5) * sin(angular_velocity * GLB_time);

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

    if (abs(x_diff) < GLB_attr && abs(y_diff) < GLB_attr) {
      if (GLB_owned_amnt < INVENTORY_BUFF_SIZE - 1) {
        int inv = 0, is_dupl = 0;
        while (GLB_itm_ownd[inv].content != '\0' && inv < INVENTORY_BUFF_SIZE - 1) {
          if (GLB_itm_ownd[inv].content == itm->content) {
            GLB_itm_ownd[inv].am++;
            is_dupl = 1;
            break;
          }
          inv++;
        }
        if (!is_dupl) {
          Copy_Item(&GLB_itm_ownd[GLB_owned_amnt], *itm);
          GLB_itm_ownd[GLB_owned_amnt].am = 1;
          GLB_owned_amnt++;
        }
        GLB_inv_incrmnt = inv;
        GLB_itm_ownd[inv].center_x += 30;
        play_sound(AU_high_pitch, 0);
        reset_item(itm);
      }
    }
  }
  return 1;
}

int upd_mobs(SHIP *plr, int reset) {
  if (GLB_game_state != boss_fight) {
    SHIP *mobs = gmo.mobs;
    ITEM *items = gmo.items;
    for (int i = 0; i < MOBS_BUFFER - 1; i++) {
      SHIP *m = &mobs[i];
      int m_center_x = m->x_pos + m->width / 2;
      int m_center_y = m->y_pos + m->height / 2;
      if (reset) m->hp = 0;
      if (GLB_time % BOSS_SPWNTIME >= BOSS_SPWNTIME - 100 && GLB_time % BOSS_SPWNTIME <= BOSS_SPWNTIME) m->hp = 0;
      if (!plr->is_blocked && GLB_time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        m->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        m->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
      // HANDLE DEATH
      if (m->hp <= 0 && m->death_timer == 0) { m->death_timer += MOB_DEATH_DUR; }
      if (m->death_timer >= 1) {
        if (m->death_timer == 1) {
          play_sound(AU_expl2, 0);
          int x_offs = 0, y_offs = 0;
          for (int x = 0; x < ITEM_DROPS_BUFF_SIZE - 1; x++) {
            int dist_btwn = 10;
            if (items[x].content == '\0' && items[x].x_pos == -1 && items[x].y_pos == -1) {
              int itm_rar = determineRarity(GLB_luck);
              ITEM new_item;
              new_item.content = '\0';
              int tries = 0;
              do {
                (new_item = get_item(itm_rar, rand_range(0, ITEM_Type_COUNT - 2), GLB_itm_list, GLB_num_items));
                tries++;
              } while (new_item.content == '\0' && new_item.type > ITEM_Type_COUNT - 2 && tries < 1200);
              Copy_Item(&items[x], new_item);
              items[x].center_x = m_center_x + x_offs, items[x].center_y = m_center_y + y_offs;
              items[x].x_pos = m->x_pos + y_offs, items[i].y_pos = m->y_pos;
              x_offs += rand_range(-3, 3), y_offs += rand_range(-3, 3);
              if (rand_range(0, GLB_luck) == 0) break;
            }
          }
          m->y_pos += rand_range(-2000, 2000), m->x_pos += rand_range(-2000, 2000);
          m->hp = (MOBS_BUFFER <= MOBS1_BUFFER ? MOBS_1_MAXHP : MOBS_2_MAXHP);
          m->death_timer = 0, m->weap = 0;
        } else {
          m->death_timer--, m->weap = 0;
          GLB_score++;
          GLB_currencies[0].amount += 50;
        }
        continue;
      }

      // HANDLE PLR COLLISION
      if (plr->hurt_timer == 0 && GLB_cur_blding == NONE) {
        if (abs(m_center_x - plr->x_pos) < m->width / 2 && abs(m_center_y - plr->y_pos) < m->height / 2) {
          if (plr->shield) {
            plr->shield--;
            m->hp -= 2;
          } else {
            usleep(HURT_DUR * 10);
            plr->hp--;
          }
          GLB_plr_col_index = i;
          plr->hurt_timer = HURT_DUR;
        }
      }
      if (GLB_cur_blding != NONE && m->weap > 0) {
        m->weap = 0;
        m->dir = rand() % 4;
      }

      // ATTACK
      if (m->weap && GLB_time % m->atk_spd == 0) {
        DIR d = m->dir;
        Init_bullet(gmo.proj_mob, base, get_dir_to_target(plr->x_pos, plr->y_pos, m_center_x, m_center_y, 0), PROJ_MOB_BUFF_SIZE, m_center_x, m_center_y);
      }
      int mv_spd = MAX(m->weap ? m->spd : m->spd * 2, 6);
      if (GLB_time % mv_spd != 0) continue;
      // CHANGE DIR
      if (m->weap > 0) {
        m->dir = get_dir_to_target(plr->x_pos, plr->y_pos, m_center_x, m_center_y, 1);
      } else if (rand() % 50 == 0)
        m->dir = rand() % 4;
      if (m->dir != none) {
        m->x_pos += m->dir == left || m->dir == up_left || m->dir == down_left ? -2 : m->dir == up || m->dir == down ? 0 : 2;
        m->y_pos += m->dir == up || m->dir == up_left || m->dir == up_right ? -1 : m->dir == left || m->dir == right ? 0 : 1;
      }
    }
  }
  return 1;
}
int upd_plr_proj(char *canv, BOSS *boss, SHIP *plr, int reset) {
  int return_res = 1;
  PROJ *proj = gmo.proj;
  SHIP *mobs = gmo.mobs;
  for (int i = 0; i < PROJ_BUFF_SIZE - 1; i++) {
    PROJ *p = &gmo.proj[i];
    if (p->y_pos <= -1 || p->x_pos <= -1 || p->y_pos >= CANV_H || p->x_pos >= CANV_W || reset || p->dur == 0) {
      if (p->dur == 0 && p->type == bomb)
        p->col_timer = 10;
      else
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
              int new_p = Init_bullet(proj, bomb, none, PROJ_BUFF_SIZE, x_p, y_p);
              proj[new_p].dur = rand_range(20, 50);
            }
          }
        } else {
          if (rand() % 20 == 0) p->dur = 0;
          if (rand() % 50 == 0) {
            int new_p = Init_bullet(proj, p->type, none, PROJ_BUFF_SIZE, p->x_pos + rand_range(-2, 2), p->y_pos + rand_range(-2, 2));
            proj[new_p].dur = rand_range(5, 20);
          }
        }
      }
      if (p->col_timer == 0) {
        p->x_pos = p->y_pos = -1, p->type = base, p->dir = none, p->dur = -1;
        continue;
      }
      if (GLB_time % 5 == 0) {
        p->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        p->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
      canv[index] = PRJ_COL;
      continue;
    }
    if (canv[index] && canv[index] != '\n') {
      if (abs(boss->y_pos - p->y_pos) < boss->height && abs((boss->x_pos + (boss->width / 2)) - p->x_pos) < boss->width / 2) {
        int dmg = plr->atk_pow + (boss->action == SP_attack2 || boss->action == SP_attack ? 10 : 0);
        if (boss->shield > 0) {
          boss->shield -= dmg;
        } else {
          boss->cur_hp -= dmg;
          boss->shield = 0;
        }
        GLB_prj_col_index = -2;
        p->col_timer = 15;
      } else {
        for (int j = 0; j < MOBS_BUFFER; j++) {
          if (mobs[j].hurt_timer || mobs[j].x_pos == -1) continue;
          if (abs((mobs[j].y_pos + (mobs[j].height / 2)) - p->y_pos) <= mobs[j].height / 2 && abs((mobs[j].x_pos + mobs[j].width / 2) - p->x_pos) <= mobs[j].width / 2) {
            if (p->type == ricochet) {
              play_sound(AU_high_pitch, 0);
              p->dur += 10;
            } else {
              p->col_timer = 15;
              play_sound(AU_click2, 0);
              p->dir = none;
            }
            mobs[j].hp -= plr->atk_pow;
            mobs[j].weap += 100;
            mobs[j].hurt_timer += MOB_HURT_DUR;
            return_res = 2;
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

    if (GLB_game_state != boss_fight) {
      p->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
      if (GLB_time % 2 == 0) p->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    }
    PRJ_TYPES tp = p->type;
    int speed = tp == base ? 1 : tp == magnet ? 1 : tp == bomb ? 6 : tp == big ? 1 : 1;
    if (GLB_game_state == boss_fight) speed = 1;

    if (p->dir == circular) {
      double radius = 10;
      p->x_pos = plr->x_pos + (PLR_SHIP_W / 2) + radius * cos(p->angular_velocity * (+p->angular_offset));
      p->y_pos = plr->y_pos + (PLR_SHIP_H / 2 - 2) + (radius * 0.5) * sin(p->angular_velocity * (+p->angular_offset));
    }

    // HANDLE PROPULSION
    if (GLB_time % speed == 0) {
      if (p->dir != none) {
        p->x_pos += p->dir == left || p->dir == up_left || p->dir == down_left ? -2 : p->dir == up || p->dir == down ? 0 : 2;
        p->y_pos += p->dir == up || p->dir == up_left || p->dir == up_right ? -1 : p->dir == left || p->dir == right ? 0 : 1;
      }
      if (p->type == big && rand() % 10 == 0 && i < (p->dir == circular ? 10 : PROJ_BUFF_SIZE - 1)) {
        int r = rand_range(-2, 3);
        int new_p = Init_bullet(proj, p->type, p->dir, PROJ_BUFF_SIZE, p->x_pos + (r == 0 ? -1 : r), p->y_pos + (r == 0 ? -1 : r));
        proj[new_p].dur = rand_range(5, 20);
      }

      if (p->type == magnet || (p->type == ricochet && p->dur)) {
        if (GLB_game_state == boss_fight) {
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
  return return_res;
}
int upd_mob_prj(char *canv, SHIP *plr, int reset) {
  PROJ *mobprj = gmo.proj_mob;
  for (int i = 0; i < PROJ_MOB_BUFF_SIZE - 1; i++) {
    int x = mobprj[i].x_pos, y = mobprj[i].y_pos;
    if (y <= -1 || x <= -1 || y >= CANV_H || x >= CANV_W || reset) {
      mobprj[i].x_pos = mobprj[i].y_pos = -1;
      continue;
    }
    if (GLB_mv_typ == cutscene) mobprj[i].x_pos = mobprj[i].y_pos = -1;
    DIR d = mobprj[i].dir;
    int index = x + ((CANV_W + 1) * y);
    if (index > 0 && index < CANV_SIZE) {
      if (abs(plr->y_pos - mobprj[i].y_pos) < 2 && abs(plr->x_pos - mobprj[i].x_pos) < 2 && plr->hurt_timer == 0) {
        mobprj[i].x_pos = mobprj[i].y_pos = -1;
        if (plr->shield)
          plr->shield--;
        else
          plr->hp--;
        GLB_prj_col_index = GLB_game_state == boss_fight ? -2 : i;
        plr->hurt_timer += HURT_DUR;
        continue;
      }
      if (canv[index] == ' ')
        canv[index] = d == up || d == down ? MPRJ_BS_VER : d == left || d == right ? MPRJ_BS_HOR : d == up_left || d == down_right ? MPRJ_BS_DIAG_L : MPRJ_BS_DIAG_R;
    }

    int is_hor = d == left || d == right;
    if (is_hor && GLB_time % 2 == 0) mobprj[i].x_pos += d == left || d == up_left || d == down_left ? -1 : d == up || d == down ? 0 : 1;
    int is_ver = d == up || d == down;
    if (is_ver && GLB_time % 4 == 0) mobprj[i].y_pos += d == up || d == up_left || d == up_right ? -1 : d == left || d == right ? 0 : 1;

    // if (GLB_mv_typ != on_grid && GLB_mv_typ != free_slide) mobprj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;

    // MOVE RELATED TO PLAYER CAM
    if (GLB_game_state != boss_fight) {
      if (GLB_time % (plr->dir == left || plr->dir == right ? 1 : 2) == 0) {
        mobprj[i].x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
        mobprj[i].y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
      }
    }
  }
  return 1;
}

int upd_wind(char *canv, int *wind, SHIP *shp, BOSS *boss) {
  for (int j = 0; j < (WIND_BUFF_SIZE - 1); j++) {
    DIR dir;
    int y_pos, x_pos, x_size, y_size;
    if (j <= PLR_WIND_BUFF_SIZE)
      dir = shp->dir, y_pos = shp->y_pos, x_pos = shp->x_pos, x_size = PLR_SHIP_W, y_size = PLR_SHIP_H;
    else if (GLB_game_state == boss_fight && boss->action != die)
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
      if (j > PLR_WIND_BUFF_SIZE)
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

int upd_buildings(char *canv, SHIP *plr, int *has_moved) {
  (void)game;
  int ret_value = 1;
  BLDING *buildings = gmo.buildings;

  for (int i = 0; i < BLDING_BUFF_SIZE - 1; i++) {
    BLDING *bld = &buildings[i];
    bld->x_pos += plr->dir == left ? 1 : plr->dir == right ? -1 : 0;
    if (GLB_time % 2 == 0) bld->y_pos += plr->dir == up ? 1 : plr->dir == down ? -1 : 0;
    int x = bld->x_pos, y = bld->y_pos, x_s = bld->x_size, y_s = bld->y_size;

    if (plr->x_pos >= x + x_s / 2 - 8 && plr->x_pos <= x + x_s / 2 + 8) {
      if (plr->y_pos == bld->y_pos + bld->y_size - 1 && GLB_cur_bld_index != -1) {
        GLB_cur_bld_index = -1;
        GLB_cur_floor = 0;
        GLB_cur_blding = NONE;
        GLB_is_in_dialog = -1;
        GLB_atk_typ = press;
        ret_value = 2;
      } else if (plr->y_pos == bld->y_pos + bld->y_size - 2 && GLB_cur_bld_index == -1) {
        GLB_cur_bld_index = i;
        GLB_cur_blding = bld->type;
        GLB_atk_typ = disabled;
        GLB_cur_floor = 0;
        plr->lk_dir = up;
        ret_value = 3;
      }
    }

    if (GLB_cur_bld_index == i) {
      char *txt = strdup(bld_types_lb[bld->type]);
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
        set_label(flr_ens, iv, GLB_cur_floor == 0 ? "##0" : "##", GLB_cur_floor);
        *iv -= 4;
        set_label(flr_ens, iv, "/", bld->flr_am);
        write_on_canv(flr_ens, canv, bld->x_pos + 14, bld->y_pos + y_s - 1);
      }

      int x_up_str = GLB_cur_floor % 2 == 0 ? x_s + x - 10 : x_s + x - (x_s - 10);
      int x_down_str = GLB_cur_floor % 2 == 0 ? x_s + x - (x_s - 10) : x_s + x - 10;
      int psh_am = 1;

      if (GLB_cur_floor % 2 == 0) {
        if ((plr->x_pos + PLR_SHIP_W + 1 > x_s + x - 10 && plr->y_pos > y_s + y - 10) && bld->floors[GLB_cur_floor + 1] != NULL) {
          GLB_cur_floor++;
          bld->x_pos += psh_am;
          plr->lk_dir = left, plr->dir = left;
          has_moved = 0;
        } else if ((plr->x_pos < x_s + x - (x_s - 10) && plr->y_pos > y_s + y - 10) && GLB_cur_floor > 0) {
          GLB_cur_floor--;
          bld->x_pos -= psh_am;
          plr->lk_dir = right, plr->dir = right;
          has_moved = 0;
        }
      } else {
        if ((plr->x_pos + PLR_SHIP_W + 1 > x_s + x - 10 && plr->y_pos > y_s + y - 10) && GLB_cur_floor > 0) {
          GLB_cur_floor--;
          bld->x_pos += psh_am;
          plr->lk_dir = left, plr->dir = left;
          has_moved = 0;
        } else if ((plr->x_pos < x_s + x - (x_s - 10) && plr->y_pos > y_s + y - 10) && bld->floors[GLB_cur_floor + 1] != NULL) {
          GLB_cur_floor++;
          bld->x_pos -= psh_am;
          plr->lk_dir = right, plr->dir = right;
          has_moved = 0;
        }
      }
    }
  }
  return ret_value;
}

int upd_collisions(char *canv, SHIP *plr) {
  int is_blocked = 0, col_dist = 1, x_offs = 0, char_to_look;
  int up_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos + x_offs, plr->y_pos - col_dist);
  int dwn_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos + x_offs, plr->y_pos + 1 + col_dist);
  int lft_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos - col_dist + x_offs - 2, plr->y_pos + col_dist);
  int rgt_char = get_index_value(canv, CANV_W, CANV_H, plr->x_pos + PLR_SHIP_W + col_dist + x_offs + 1, plr->y_pos + col_dist);

  int up_blk = (up_char == HOR_WALL || up_char == VRT_WALL || up_char == UPRGT_WALL || up_char == UPLFT_WALL || up_char == DWNLFT_WALL || up_char == DWNRGT_WALL);
  if (up_blk) write_on_canv("^", canv, plr->x_pos, plr->y_pos - col_dist);
  int dwn_blk = (dwn_char == HOR_WALL || dwn_char == VRT_WALL || dwn_char == UPRGT_WALL || dwn_char == UPLFT_WALL || dwn_char == DWNLFT_WALL || dwn_char == DWNRGT_WALL);
  if (dwn_blk) write_on_canv("v", canv, plr->x_pos, plr->y_pos + 2 + col_dist);
  int lft_blk = (lft_char == HOR_WALL || lft_char == VRT_WALL || lft_char == UPRGT_WALL || lft_char == UPLFT_WALL || lft_char == DWNLFT_WALL || lft_char == DWNRGT_WALL);
  if (lft_blk) write_on_canv("<", canv, plr->x_pos - col_dist, plr->y_pos + col_dist);
  int rgt_blk = (rgt_char == HOR_WALL || rgt_char == VRT_WALL || rgt_char == UPRGT_WALL || rgt_char == UPLFT_WALL || rgt_char == DWNLFT_WALL || rgt_char == DWNRGT_WALL);
  if (rgt_blk) write_on_canv(">", canv, plr->x_pos + PLR_SHIP_W + 1 + col_dist, plr->y_pos + col_dist);
  int hor_blk = lft_blk && rgt_blk;
  int ver_blk = up_blk && dwn_blk;

  if (up_blk && lft_blk)
    GLB_allowed_dir = down_right;
  else if (up_blk && rgt_blk)
    GLB_allowed_dir = down_left;
  else if (dwn_blk && lft_blk)
    GLB_allowed_dir = up_right;
  else if (dwn_blk && rgt_blk)
    GLB_allowed_dir = up_left;
  else if (hor_blk)
    GLB_allowed_dir = vert;
  else if (ver_blk)
    GLB_allowed_dir = hor;
  else if (up_blk)
    GLB_allowed_dir = hor_down;
  else if (dwn_blk)
    GLB_allowed_dir = hor_up;
  else if (lft_blk)
    GLB_allowed_dir = ver_right;
  else if (rgt_blk)
    GLB_allowed_dir = ver_left;
  else
    GLB_allowed_dir = all;

  if (!is_dir_allowed(plr->dir, GLB_allowed_dir)) {
    plr->dir = none;
    plr->is_blocked = 1;
  } else
    plr->is_blocked = 0;

  if (plr->is_blocked) plr->dir = none;
  return plr->is_blocked;
}

int set_plr_death(char *canv, SHIP plr) {
  plr.hp = 0;
  system("pkill afplay");
  write_midle_msg(canv, 1, "GAME OVER\n GLB_score: %d\n BEST: %d\n \n- CONTINUE (c)\n- RESTART (r)\n- QUIT (q)\n- SHOW_STATS (s)\n- PARAM (p)", GLB_score);
  system("clear");
  render_canvas(canv, NULL);
  int inp = get_char_and_freeze("rcq");
  return (inp == 'c' ? 1 : inp == 'r' ? 2 : 0);
}