#include "../headers/includes.h"

BOSS_ACTIONS SET_RAND_STATE(GAME *game, BOSS *boss, int hp_perc);

int upd_boss_fight(char *canv, GAME *game, BOSS *boss, SHIP *mobs, PROJ *mobprj, LOOT *loots, SHIP *plr, int time) {
  {
    (void)loots;
    char *boss_content = strdup(boss->content);
    int hp_perc = PERCENTILE(boss->cur_hp, boss->max_HP);

    HANDLE_CONSTANTS(game, boss, plr, mobprj, time, hp_perc);

    //  CHANGE OF ACTION
    if (boss->time_in_state <= 0) CHANGE_ACTION(game, boss, plr, hp_perc);
    if (boss->action == die)
      DEATH(game, boss, mobprj);
    else if (boss->action == entrance)
      ENTRANCE(game, boss, plr, time);
    else if (boss->action == reposition)
      REPOSITION(game, boss, plr, time);
    else if (boss->action == SP_attack)
      SP_ATTACK(game, boss, plr, mobprj, time);
    else if (boss->action == SP_attack2)
      SP_ATK2(game, boss, plr, time);
    else if (boss->action == attack0)
      ATK0(game, boss, plr, mobprj, time);
    else if (boss->action == attack)
      ATK1(game, boss, plr, mobprj, time, hp_perc);
    else if (boss->action == attack2)
      ATK2(game, boss, plr, mobprj, time, hp_perc);
    else if (boss->action == call)
      CALL(game, boss, plr, mobs, time);
    else if (boss->action == idle)
      IDLE(game, boss, plr, time);
    else if (boss->action == hurt)
      HURT(game, boss, plr, time);
    else if (boss->action == shield)
      SHIELD(game, boss, plr);
    else if (boss->action == retreat)
      plr->dir = none, RETREAT(canv, game, boss, plr, time);

    boss->time_in_state--;
    int has_losthp = HANDLE_HEALTH(boss, hp_perc);
    boss_content = strdup(boss->content);
    if (has_losthp) set_text_red(boss_content);

    int show_boss = 1;
    if (boss->prv_shield != boss->shield) {
      if (boss->prv_shield > boss->shield) show_boss = 0;
      boss->prv_shield = boss->shield;
    }
    if (boss_content != NULL && show_boss) RENDER_BOSS(boss, boss_content, canv);
  }
  return 1;
}

void HANDLE_CONSTANTS(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time, int hp_perc) {
  (void)game, (void)plr, (void)mobprj, (void)time;
  if (hp_perc == 40 || hp_perc == 60) {
    boss->next_action = hurt;
    boss->time_in_state = 0;
    boss->hole_size -= boss->hole_size < SHIP_W + 2 ? 0 : 1;
  } else if (hp_perc == 20) {
    boss->next_action = SP_attack;
    boss->time_in_state = 0;
    boss->hole_size -= boss->hole_size < SHIP_W + 2 ? 0 : 1;
  }

  /*
if (time % 20 == 0) {
  for (int i = 0; i < PRJ_MOB_BUFFER; i++) {
    if (mobprj[i].x_pos == -1 && mobprj[i].y_pos == -1) {
      mobprj[i].x_pos = rand_range(boss->x_pos, boss->x_pos + boss->width + 1);
      mobprj[i].y_pos = rand_range(boss->y_pos, boss->y_pos + boss->height + 1);
      mobprj[i].dir = rand_range(1, 7);
      if (mobprj[i].dir == none) mobprj[i].dir = left;
      break;
    }
  }
}*/
  // handle PLR_DIR_SHIFT

  /*
    if (game->mv_type != on_grid && game->mv_type != free_slide) {
      if (plr->dir == left || plr->dir == right) {
        if (plr->dir == left && boss->x_pos <= CANV_W - boss->width / 2) boss->x_pos += 1;
        if (plr->dir == right && boss->x_pos >= -boss->width / 2) boss->x_pos -= 1;
      }
    }*/
}

void CHANGE_ACTION(GAME *game, BOSS *boss, SHIP *plr, int hp_perc) {
  game->mv_type = normal, game->atk_type = autom;
  boss->x_dir = 1, boss->y_dir = 1;
  boss->hole_size = 0, boss->x_dir = 0, boss->speATK_hole = 0;

  boss->action = boss->next_action;

  switch (boss->action) {
    case idle:
      boss->time_in_state = BOSS_ATK1_DUR;
      boss->next_action = SET_RAND_STATE(game, boss, hp_perc);
      break;
    case hurt:
      boss->time_in_state = BOSS_HURT_DUR;
      boss->next_action = shield;
      break;
    case shield:
      boss->time_in_state = BOSS_SHIELD_DUR;
      boss->next_action = attack0;
      break;
    case attack0:
      boss->time_in_state = BOSS_ATK0_DUR;
      boss->next_action = idle;
      break;
    case attack:
      boss->time_in_state = BOSS_ATK1_DUR;
      boss->next_action = idle;
      break;
    case attack2:
      boss->time_in_state = BOSS_ATK2_DUR;
      boss->next_action = idle;
      game->mv_type = free_slide;
      boss->x_dir = rand() % 2 ? 1 : -1;
      boss->speATK_hole = boss->x_dir ? CANV_W - 50 : 50;
      break;
    case reposition:
      boss->time_in_state = BOSS_REPOS_DUR;
      boss->next_action = idle;
      break;
    case SP_attack:
      boss->time_in_state = BOSS_SP_ATK_DUR;
      boss->next_action = reposition;
      boss->speATK_hole = (plr->x_pos + SHIP_W / 2) + SHIP_W * rand_range(-10, 10);
      break;
    case SP_attack2:
      boss->time_in_state = BOSS_SP2_ATK_DUR;
      boss->next_action = reposition;
      break;
    case retreat:
      boss->time_in_state = BOSS_RETREAT_DUR;
      boss->next_action = idle;
      break;
    case regen:
      boss->time_in_state = BOSS_REGEN_DUR;
      boss->next_action = attack0;
      break;
    default:
      boss->time_in_state = BOSS_ATK1_DUR;
      break;
  }
}

BOSS_ACTIONS SET_RAND_STATE(GAME *game, BOSS *boss, int hp_perc) {
  (void)game;
  if (boss->cur_hp == boss->max_HP / 10) return (regen);
  int r = rand() % 6;
  if (r <= 3 && hp_perc < 30) return (SP_attack);
  if (r == 1 && hp_perc < 60) return (SP_attack2);
  if (r == 2 && hp_perc < 90) return (retreat);
  if (r == 3 && hp_perc >= 60 && hp_perc <= 90) return (attack2);
  return (attack);
}

void ENTRANCE(GAME *game, BOSS *boss, SHIP *plr, int time) {
  game->boss_mode = 2;
  game->mv_type = cutscene, game->allowed_dir = up, game->allowed_lk_dir = up, plr->dir = up, plr->lk_dir = up;
  boss->next_action = shield;
  if (boss->time_in_state <= 0) {
    game->mv_type = watch_up;
    game->boss_mode = 1;
  }
  if (time % 5 == 0 && boss->y_pos < 12) boss->y_pos++;
}

void DEATH(GAME *game, BOSS *boss, PROJ *mobprj) {
  boss->x_pos += boss->x_pos < CENTER_X(boss->width) ? 1 : 0;
  game->boss_mode = 2;
  for (int i = 0; i < PRJ_MOB_BUFFER; i++) {
    if (mobprj[i].x_pos == -1 && mobprj[i].y_pos == -1) {
      mobprj[i].x_pos = rand_range(boss->x_pos, boss->x_pos + boss->width + 1);
      mobprj[i].y_pos = rand_range(boss->y_pos, boss->y_pos + boss->height + 1);
      mobprj[i].dir = rand_range(2, 7);
      if (mobprj[i].dir == none) mobprj[i].dir = left;
      break;
    }
  }
  if (boss->time_in_state <= 1) {
    game->mv_type = normal;
    game->allowed_dir = all, game->allowed_lk_dir = all;
    game->is_in_boss_fight = 0;
    game->boss_mode = 0;
  }
}

void REPOSITION(GAME *game, BOSS *boss, SHIP *plr, int time) {
  game->boss_mode = 2;
  game->mv_type = cutscene, game->allowed_dir = all, game->allowed_lk_dir = all;
  plr->dir = none;
  int BOSS_CENTER_X = CANV_X_CENTER - boss->width / 2;
  int boss_is_set = boss->y_pos == BOSS_CENTER_Y && (boss->x_pos >= BOSS_CENTER_X - 3 && boss->x_pos < BOSS_CENTER_X + 3);
  int plr_is_set = plr->y_pos == PLR_CENTER_Y_BOSS && plr->x_pos == PLR_CENTER_X;

  if (boss_is_set && plr_is_set)
    boss->time_in_state = 0;

  else if (time % 5 == 0) {
    boss->x_dir = boss->x_pos < BOSS_CENTER_X ? 1 : boss->x_pos > BOSS_CENTER_X ? -1 : 0;
    boss->y_dir = boss->y_pos < 6 ? 1 : boss->y_pos > 12 ? -1 : 0;
    boss->y_pos += boss->y_pos > BOSS_CENTER_Y ? -1 : boss->y_pos < BOSS_CENTER_Y ? 1 : 0;
    boss->x_pos += boss->x_pos > BOSS_CENTER_X ? -1 : boss->x_pos < BOSS_CENTER_X ? 1 : 0;
  }
}

void IDLE(GAME *game, BOSS *boss, SHIP *plr, int time) {
  game->mv_type = watch_up, game->atk_type = autom;
  game->allowed_dir = hor_up, game->allowed_lk_dir = up, plr->lk_dir = up;
  if (time % boss->speed == 0) {
    boss->x_dir = boss->x_pos < 3 ? 1 : boss->x_pos + boss->width > CANV_W - 3 ? -1 : boss->x_dir;
    boss->y_dir = boss->y_pos < 6 ? 1 : boss->y_pos > 12 ? -1 : boss->y_dir;
    boss->x_pos += boss->x_dir, boss->y_pos += boss->y_dir;
  }
}

void ATK0(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time) {
  game->mv_type = watch_up, game->atk_type = autom;
  game->allowed_dir = hor_up, game->allowed_lk_dir = up, plr->lk_dir = up;
  if (time % boss->atk_spd == 0) {
    for (int i = 0; i < PRJ_MOB_BUFFER; i++) {
      if (mobprj[i].x_pos == -1 && mobprj[i].y_pos == -1) {
        mobprj[i].x_pos = rand_range(boss->x_pos, boss->x_pos + boss->width + 1);
        mobprj[i].y_pos = rand_range(boss->y_pos, boss->y_pos + boss->height + 1);
        mobprj[i].dir = down;
        break;
      }
    }
  }
}

void ATK1(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time, int hp_perc) {
  game->mv_type = watch_up, game->atk_type = autom;
  game->allowed_dir = hor_up, game->allowed_lk_dir = up, plr->lk_dir = up;
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

void ATK2(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time, int hp_perc) {
  (void)time;
  game->mv_type = free_slide;
  game->allowed_dir = hor, game->allowed_lk_dir = up, plr->lk_dir = up;
  int speed = 3;
  int hole_size = (SHIP_W + 4) * 2;
  int prj_x = boss->x_pos + boss->width / 2;
  int l_reach = prj_x <= 0, r_reach = prj_x > CANV_W - 2;
  int min_hole_start = hole_size * 2, max_hole_start = CANV_W - hole_size * 2;
  if (prj_x >= boss->speATK_hole - speed && prj_x <= boss->speATK_hole + speed) boss->hole_size = hole_size;
  if (!boss->hole_size)
    Init_bullet(mobprj, base, down, PRJ_MOB_BUFFER, prj_x, boss->y_pos + boss->height);
  else
    boss->hole_size--;

  if (boss->hole_xdir)
    boss->hole_xdir--;
  else {
    if (hp_perc < 20) {
      boss->y_dir = boss->y_pos < 6 ? 1 : boss->y_pos > 12 ? -1 : boss->y_dir;
      boss->y_pos += boss->y_dir;
    }
    boss->x_dir = l_reach ? 1 : r_reach ? -1 : boss->x_dir;
    boss->x_pos += boss->x_dir;
  }
  if (l_reach || r_reach) {
    boss->speATK_hole = rand_range(min_hole_start, max_hole_start);
  }
}

void SP_ATTACK(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time) {
  int bul_amnt = CANV_X_CENTER;
  game->mv_type = on_grid, game->atk_type = press;
  game->allowed_dir = all, game->allowed_lk_dir = all;

  if (plr->y_pos <= boss->y_pos + boss->height + 1 && boss->time_in_state > 60) boss->time_in_state = 60;
  if (boss->time_in_state >= 60 && time % (boss->atk_spd) == 0) {
    if (boss->time_in_state <= 100)
      boss->x_dir = 0, boss->hole_size = SHIP_W + 2;
    else {
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
    if (boss->y_pos < CANV_Y_CENTER - 3) boss->y_pos += boss->x_dir;

    if (boss->time_in_state > 130) {
      int x_dist = (CANV_W) / bul_amnt;
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

void SP_ATK2(GAME *game, BOSS *boss, SHIP *plr, int time) {
  game->mv_type = on_grid, game->atk_type = disabled;
  game->allowed_dir = down, game->allowed_lk_dir = down;
  int max_dist = 50;
  int y_dist = max_dist - abs(boss->y_pos - plr->y_pos);
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

void CALL(GAME *game, BOSS *boss, SHIP *plr, SHIP *mobs, int time) {
  (void)time, (void)plr;
  game->mv_type = on_grid, game->atk_type = disabled;
  game->allowed_dir = down, game->allowed_lk_dir = down;
  for (int i = 0; i < MOBS_BUFFER; i++) {
    if (mobs[i].x_pos != -1 && mobs[i].y_pos != -1) continue;
    mobs[i].x_pos = CANV_X_CENTER - SHIP_W / 2, mobs[i].y_pos = boss->y_pos + boss->height + 1;
    mobs[i].speed = 1, mobs[i].dir = down;
    break;
  }
}

void HURT(GAME *game, BOSS *boss, SHIP *plr, int time) {
  (void)plr;
  game->mv_type = watch_up, game->atk_type = autom;
  boss->x_dir = time % 2 == 0 ? -1 : 1;
  boss->y_dir = time % 2 == 0 ? -1 : 1;
  boss->x_pos += boss->x_dir;
  boss->y_pos += boss->y_dir;

  if (time % 30 == 0 && boss->time_in_state > 50) {
    int ind = rand_range(1, strlen(boss->content) - 1);
    if (boss->content[ind] != '\n') boss->content[ind] = EXPL_START;
  }
}

void SHIELD(GAME *game, BOSS *boss, SHIP *plr) {
  (void)game, (void)plr;
  boss->shield++;
  boss->time_in_state = 2;
  if (boss->shield == boss->max_shield) boss->time_in_state = 0;
}

void RETREAT(char *canv, GAME *game, BOSS *boss, SHIP *plr, int time) {
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
    char *mark = strdup(QUESTIONMARK);
    write_on_canv(mark, canv, boss->x_pos + boss->width / 2, CANV_H - 10);
  }
  // QUICKLY MOVE DOWN DURING ATTACK
  if (boss->time_in_state > 100 && boss->time_in_state <= 150 && boss->y_pos < plr->y_pos) {
    if (boss->y_pos < 3) {
      int center_pos = boss->x_pos + boss->width / 2;
      int plr_center = plr->x_pos;
      boss->x_pos += (center_pos < plr_center ? 2 : center_pos > plr_center ? -2 : 0);
    }

    char *mark = strdup(QUESTIONMARK);
    write_on_canv(mark, canv, boss->x_pos + boss->width / 2, CANV_H - 10);
    free(mark);
    boss->y_pos += 3;
  }
  // MOVE BACK UP AFTER ATTACK
  else if (boss->time_in_state < 100) {
    boss->y_dir = boss->y_pos > BOSS_CENTER_Y ? -2 : boss->y_pos < BOSS_CENTER_Y ? 1 : 0;
    boss->x_dir = boss->x_pos > (CANV_X_CENTER - boss->width / 2) ? -1 : boss->x_pos < (CANV_X_CENTER - boss->width / 2) ? 1 : 0;
    boss->y_pos += boss->y_dir, boss->x_pos += boss->x_dir;
    if (boss->y_pos <= BOSS_CENTER_Y) boss->time_in_state = 0;
  }
}

int HANDLE_HEALTH(BOSS *boss, int hp_perc) {
  (void)hp_perc;

  if (boss->action == die) return 1;

  int has_losthp = false;
  if (boss->prev_HP > boss->cur_hp) {
    has_losthp = true;
    if (rand() % boss->cur_hp / 10 <= 0) {
      int index = rand_range((boss->width + 1) * 2, (boss->width + 1) * (boss->height - 1));
      if (boss->content[index] != '\n') boss->content[index] = PRJ_COL;
    }
  }
  boss->prev_HP = boss->cur_hp;
  if (boss->cur_hp < 0) boss->action = die, boss->cur_hp = 0;

  return (has_losthp);
}

void RENDER_BOSS(BOSS *boss, char *boss_content, char *canv) {
  if (boss->shield) {
    int active_shields = (boss->shield * boss->shld_ic_count) / 100;
    int incr = 0;
    for (unsigned int i = 0; i < strlen(boss->content); i++) {
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
  write_on_canv(boss_content, canv, boss->x_pos, boss->y_pos);
  free(boss_content);
}