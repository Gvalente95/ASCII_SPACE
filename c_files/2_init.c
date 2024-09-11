#include "../headers/includes.h"

const char *BLDING_LABELS[] = {"SHOP", "ARMORY", "HOUSE", "FORTRESS", "NONE", "BLDING_COUNT"};

void initialize_game(GAME *game, SHIP *plr, ITEM *items, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars, BLDING *buildings) {
  set_nonblocking_mode(1);

  game->luck = 0;
  game->level = game->score = game->max_score = game->prj_qual = 0;
  game->lv_choices = 3, game->attr = 2;
  game->gl_x_pos = 0, game->gl_y_pos = 0, game->prj_col_index = -1, game->plr_col_index = -1, game->is_framing = 0;
  game->boss_mode = 0, game->g_st = nm;
  game->mv_type = normal, game->atk_type = press, game->allowed_dir = all, game->allowed_lk_dir = all;
  game->cur_proj = base, game->cur_blding = NONE, game->cur_bld_index = -1;
  game->is_in_dialog = -1, game->cur_floor = -1, game->owned_amnt = 0, game->inv_incrmnt = 0;
  game->minimap = NULL, game->cur_seller = NULL;
  for (int x = 0; x < CURRENCY_COUNT - 1; x++) {
    game->currencies[x].amount = 500;
    game->currencies[x].type = x;
  }

  plr->x_pos = PLR_CENTER_X + 5;
  plr->y_pos = PLR_CENTER_Y;
  plr->maxHP = PLR_MAX_HP, plr->hp = plr->maxHP;
  plr->hurt_timer = plr->shield = plr->weap = 0, plr->atk_pow = 1, plr->atk_spd = 5, plr->atk_am = 1, plr->is_blocked = 0;
  plr->maxSpd = 1, plr->spd = plr->maxSpd;
  plr->atk_reload = 1;
  plr->dir = up, plr->lk_dir = up;

  for (int i = 0; i < items_OWNED_BUFFER; i++)
    reset_item(&game->itm_ownd[i]);

  for (int i = 0; i < ITEM_BUFFER; i++)
    reset_item(&items[i]);

  for (int i = 0; i < MOBS_BUFFER; i++) {
    int is_2 = i > MOBS1_BUFFER;
    int x, y;
    do {
      x = rand_range(-500, 500);
      y = rand_range(-500, 500);
    } while (is_pos_free(mobs, i, x, y));
    mobs[i].x_pos = x, mobs[i].y_pos = y, mobs[i].is_blocked = 0;
    mobs[i].shield = mobs[i].weap = mobs[i].hurt_timer = mobs[i].death_timer = 0;
    mobs[i].maxHP = is_2 ? MOBS_2_MAXHP : MOBS_1_MAXHP, mobs[i].hp = mobs[i].maxHP;
    mobs[i].width = is_2 ? 5 : 3;
    mobs[i].height = is_2 ? 5 : 2;
    mobs[i].maxSpd = is_2 ? MOBS_2_SPD : MOBS_1_SPD, mobs[i].spd = mobs[i].maxSpd;
    mobs[i].dir = rand_range(0, 3);
    mobs[i].atk_spd = is_2 ? 50 : 25;
  }

  for (int i = 0; i < PROJ_BUFFER; i++) {
    proj[i].prv_index = proj[i].x_pos = proj[i].y_pos = -1, proj[i].col_timer = 0, proj[i].angular_velocity = .1f, proj[i].angular_offset = rand_range(1, 50);
    proj[i].dur = -1;
  }

  for (int i = 0; i < PRJ_MOB_BUFFER; i++)
    mobprj[i].x_pos = mobprj[i].y_pos = -1, mobprj[i].col_timer = 0, mobprj[i].dur = -1;

  for (int i = 0; i < STAR_BUFFER - 1; i++) {
    stars[i].x_pos = rand_range(1, CANV_W - 1);
    stars[i].y_pos = rand_range(1, CANV_H - 1);
    stars[i].z_pos = rand_range(2, 16);
  }
  for (int i = 0; i < BLDING_BUFFER; i++) {
    int x, y, x_min, x_max, y_min, y_max;
    do {
      x = rand_range(-1000, 1000), y = rand_range(-1000, 1000);
    } while (is_pos_free_BUILDINGS(buildings, i, x, y));
    init_new_buildings(game, rand_range(0, BLD_TYPE_COUNT - 2), &buildings[i], x, y);
  }
}

void init_new_buildings(GAME *game, BLD_TYPE type, BLDING *bld, int x, int y) {
  int strt_ind = 0;
  int y_s = (type == HOUSE) ? rand_range(25, 35) : (type == SHOP) ? 33 : (type == FORTRESS) ? 55 : rand_range(30, 40), x_s = y_s * 2;
  int y_s_ext = y_s / 2, x_s_ext = y_s_ext * 2;
  bld->y_size = y_s, bld->x_size = x_s;
  bld->x_pos = x, bld->y_pos = y;
  bld->type = type;
  bld->floors = (char **)malloc(sizeof(char *) * 10);
  bld->y_s_ext = y_s_ext, bld->x_s_ext = x_s_ext;
  bld->npc_am = rand_range(2, 5), bld->npcs = malloc(sizeof(NPC) * bld->npc_am);

  char *txt = strdup(BLDING_LABELS[type]);
  int tx_len = strlen(txt);
  int ens_len = tx_len + 4;
  char *enseigne = init_blank_canv(ens_len, 4, 2, '#');
  color_text(type == HOUSE ? blue : type == SHOP ? green : type == FORTRESS ? red : yellow, enseigne, 0);
  write_on_canv(txt, enseigne, 2, 2);
  free(txt);

  // SET FLOORS
  char *stairdown, *stairup;
  int rs = rand_range(0, 6);
  int floors_am = bld->type == HOUSE ? rand_range(0, 4) : bld->type == FORTRESS ? rand_range(0, 7) : 0;
  bld->flr_am = floors_am;
  int i = 0;
  while (i < 10) {
    if (i > floors_am)
      bld->floors[i] = NULL;
    else {
      bld->floors[i] = init_blank_canv(x_s, y_s, 2, '#');
      tile_canvas(bld->floors[i], x_s + 1, 6);
      color_text(bld->type == FORTRESS ? red : blue, bld->floors[i], 0);

      if (bld->type == HOUSE) {
        int ras = rand() % 2;
        char *sofa = strdup(ras == 0 ? SOFA : SOFA_HOR), *tv_sofa = strdup(SOFA_UP), *tv = strdup(TV), *frd = strdup(FRIDGE);
        color_text(red, sofa, 0), color_text(red, tv_sofa, 0);
        int rand_displ = rand_range(-5, 5);
        write_on_canv(sofa, bld->floors[i], 3, 4 + rand_displ), free(sofa);
        write_on_canv(tv, bld->floors[i], x_s - 16 - rand_displ, 3), free(tv);
        write_on_canv(tv_sofa, bld->floors[i], x_s - 25 - rand_displ, 10), free(tv_sofa);
        write_on_canv(frd, bld->floors[i], x_s - 30 - rand_displ, 1), free(frd);
      }

      if (floors_am > 0) {
        if (i == floors_am) {
          stairdown = strdup(stair_down);
          write_on_canv(stairdown, bld->floors[i], i % 2 == 0 ? 3 : x_s - 11, y_s - 6);
          free(stairdown);
        } else {
          // LEFT_STAIRS
          if (i > 0 && i < floors_am) {
            stairdown = strdup(i % 2 == 0 ? stair_down : stair_up);
            write_on_canv(stairdown, bld->floors[i], 3, y_s - 6);
            free(stairdown);
          }
          // RIGHT STAIRS
          if (i <= floors_am && floors_am > 0) {
            stairup = strdup(i % 2 == 0 ? stair_up : stair_down);
            write_on_canv(stairup, bld->floors[i], x_s - 11, y_s - 6);
            free(stairup);
          }
        }
      }
    }

    i++;
  }
  // SET SHOP
  if (bld->type == SHOP || bld->type == ARMORY) {
    char *ship = strdup(ship_full);
    bld->ext_cont = init_blank_canv(get_width(ship), y_s, 0, ' ');
    write_on_canv(ship, bld->ext_cont, 0, y_s - get_height(ship) - 1);
    x_s_ext = get_width(bld->ext_cont), y_s_ext = get_height(bld->ext_cont);
    bld->x_s_ext = x_s_ext, bld->y_s_ext = y_s_ext;
    int u = 0;
    while (enseigne[u]) {
      if (enseigne[u] == ' ' || enseigne[u] == '#') enseigne[u] = rand() % 2 == 0 ? WIND_ALL_DOT : rand() % 3 == 0 ? WIND_ALL_HOR : WIND_ALL_VER;
      u++;
    }
    write_on_canv(enseigne, bld->ext_cont, (x_s_ext / 2) - ens_len / 2, y_s_ext - 12);

    char *comptoir = init_blank_canv(get_width(bld->floors[0]), 3, 2, '#');
    int x_add, y_add = -8;
    for (int i = 0; i < 5; i++) {
      y_add += 8;
      if (i == 3) y_add = 0;
      int x_add = i > 2 ? x_s - 30 : 0;
      char *vitrine = init_blank_canv(10, 10, 0, ' ');
      char *box = init_blank_canv(9, 3, 2, '#');

      char *item = strdup(rs == 0 ? LUCK_GFX_SMALL : rs == 1 ? SCOR_GFX_SMALL : rs == 2 ? SHIELD_GFX_SMALL : rs == 3 ? GUN_GFX_SMALL : rs == 4 ? ATTR_GFX_SMALL : HEART_GFX_SMALL);
      rs++;
      if (rs > 5) rs = 0;
      write_on_canv(box, vitrine, 0, 2);
      write_on_canv(item, vitrine, 3, 1);
      write_on_canv(vitrine, bld->floors[0], x_add + 7, 6 + y_add);
      free(vitrine), free(item);
    }
    write_on_canv(comptoir, bld->floors[0], 0, 8);
    free(comptoir);

    NPC *n = &bld->npcs[strt_ind];
    strt_ind++;

    n->type = SELLER;
    n->floor = 0;
    n->text = strdup(bld->type == SHOP ? slr_dlgs[rand_range(0, 13)] : forg_dlgs[rand_range(0, 12)]);
    n->x_p = (x_s / 2) - 5, n->y_p = 5;
    n->dwn_bdy = strdup(seller_idle_down1), n->up_bdy = strdup(seller_idle_up);
    n->lft_bdy = strdup(seller_idle_left), n->rgt_bdy = strdup(seller_idle_right);
    n->lk_dir = rand_range(0, 4), n->dir = none;

    n->items = malloc(sizeof(ITEM) * SHOP_INVENTORY);
    Generate_items(game->itm_list, n, SHOP_INVENTORY, game->num_items);
    system("clear");
    printf("NPC items:\n\n");
    for (int x = 0; x < SHOP_INVENTORY - 1; x++) {
      printf("%d: %s am:%d\n", x, n->items[x].name, n->items[x].am);
      n->items[x].am = rand_range(5, 99);
    }

  } else {
    bld->ext_cont = init_blank_canv(x_s, y_s, 0, ' ');
    char *ext_in = init_blank_canv(x_s_ext, y_s_ext, 2, '_');
    char *ext_roof = init_blank_canv(x_s_ext - 4, y_s_ext - 2, 2, '.');
    write_on_canv(ext_roof, ext_in, 2, 0);
    write_on_canv(enseigne, ext_in, (x_s_ext / 2) - ens_len / 2, y_s_ext - 8);
    write_on_canv(ext_in, bld->ext_cont, x_s / 2 - x_s_ext / 2, y_s - y_s_ext);
  }
  free(enseigne);

  // ENTRANCE
  int size = 6, height = 3;
  int x_cent = x_s / 2;
  int cur_y = ((x_s + 1) * (y_s));
  int int_i = x_cent + cur_y + x;
  for (int y = 0; y < height; y++) {
    cur_y -= ((x_s + 1));
    for (int x = -size; x < size; x++) {
      int_i = x_cent + cur_y + x;
      if (int_i >= 0) {
        bld->floors[0][int_i] = (y <= 0 && (x == -size || x == size - 1)) ? '|' : '#';
        if (bld->type != SHOP && bld->type != ARMORY) bld->ext_cont[int_i] = y == height - 1 ? '-' : x == -size || x == size - 1 ? '|' : '#';
      }
    }
  }

  for (int i = strt_ind; i < bld->npc_am; i++) {
    bld->npcs[i].type = rand_range(1, 3);
    bld->npcs[i].text = strdup(forg_dlgs[rand_range(0, 12)]);
    bld->npcs[i].x_p = rand_range(5, bld->x_size - 5), bld->npcs[i].y_p = rand_range(5, bld->y_size - 5);
    bld->npcs[i].dwn_bdy = strdup(plr_idle_down1), bld->npcs[i].up_bdy = strdup(plr_idle_up), bld->npcs[i].lft_bdy = strdup(plr_idle_left), bld->npcs[i].rgt_bdy = strdup(plr_idle_right);
    bld->npcs[i].dir = rand_range(0, 4);
    bld->npcs[strt_ind].floor = rand_range(0, bld->flr_am - 1);
  }
}

char *init_blank_canv(int w, int h, int has_ext, char interior) {
  char *canv = malloc(w + (w * h) + 1);
  int i = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if (x == 0 && y == 0)
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? UPLFT_WALL : ' ';
      else if (x == 0 && y == h - 1)
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? DWNLFT_WALL : ' ';
      else if (x == w - 1 && y == 0)
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? UPRGT_WALL : ' ';
      else if (x == w - 1 && y == h - 1)
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? DWNRGT_WALL : ' ';
      else if (x == 0 || x == w - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 3 ? '|' : has_ext == 2 ? VRT_WALL : has_ext == 1 ? CANV_VER : COL_VER;
      else if (y == 0)
        canv[i++] = !has_ext ? ' ' : has_ext == 3 ? '_' : has_ext == 2 ? HOR_WALL : has_ext == 1 ? CANV_HOR : COL_HOR;
      else if (y == h - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 3 ? '-' : has_ext == 2 ? HOR_WALL : has_ext == 1 ? CANV_HOR : COL_HOR;
      else
        canv[i++] = interior;
    }
    canv[i++] = '\n';
  }
  canv[i] = '\0';
  return (canv);
}

int Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (prj[i].col_timer || (prj[i].x_pos > -1 && prj[i].y_pos > -1)) continue;
    prj[i].x_pos = x_pos, prj[i].y_pos = y_pos, prj[i].dir = dir, prj[i].type = type;
    return i;
  }
  return -1;
}

void init_boss(GAME *game, BOSS *boss) {
  game->g_st = boss_fight, game->boss_mode = 2;

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