#include "../../headers/1_includes.h"

void cleanup_game_objects() {
  if (gmo.mobs) free(gmo.mobs);
  if (gmo.proj) free(gmo.proj);
  if (gmo.proj_mob) free(gmo.proj_mob);
  if (gmo.buildings) free(gmo.buildings);
  if (gmo.items) free(gmo.items);
  if (gmo.stars) free(gmo.stars);
}
void initialize_game_objects() {
  gmo.mobs = malloc(sizeof(SHIP) * MOBS_BUFFER);
  gmo.proj = malloc(sizeof(PROJ) * PROJ_BUFF_SIZE);
  gmo.proj_mob = malloc(sizeof(PROJ) * PROJ_MOB_BUFF_SIZE);
  gmo.buildings = malloc(sizeof(BLDING) * BLDING_BUFF_SIZE);
  gmo.items = malloc(sizeof(ITEM) * ITEM_DROPS_BUFF_SIZE);
  gmo.stars = malloc(sizeof(STAR) * STAR_BUFF_SIZE);
  gmo.bosses = malloc(sizeof(BOSS) * 10);
}

void initialize_game(SHIP *plr) {
  set_nonblocking_mode(1);
  initialize_game_objects();

  for (int i = 0; i < ITEM_DROPS_BUFF_SIZE; i++) {
    reset_item(&gmo.items[i]);
    gmo.items[i].content = '\0';
  }
  init_game();

  plr->x_pos = PLR_CENTER_X + 5;
  plr->y_pos = PLR_CENTER_Y;
  plr->maxHP = PLR_MAX_HP, plr->hp = plr->maxHP;
  plr->hurt_timer = plr->shield = plr->weap = 0, plr->atk_pow = 1, plr->atk_spd = 5, plr->atk_am = 1, plr->is_blocked = 0;
  plr->maxSpd = 1, plr->spd = plr->maxSpd;
  plr->atk_reload = 1;
  plr->dir = up, plr->lk_dir = up;

  SHIP *mobs = gmo.mobs;
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

  for (int i = 0; i < PROJ_BUFF_SIZE; i++) {
    gmo.proj[i].prv_index = gmo.proj[i].x_pos = gmo.proj[i].y_pos = -1, gmo.proj[i].col_timer = 0, gmo.proj[i].angular_velocity = .1f,
    gmo.proj[i].angular_offset = rand_range(1, 50);
    gmo.proj[i].dur = -1;
  }

  PROJ *mobprj = gmo.proj_mob;
  for (int i = 0; i < PROJ_MOB_BUFF_SIZE; i++)
    mobprj[i].x_pos = mobprj[i].y_pos = -1, mobprj[i].col_timer = 0, mobprj[i].dur = -1;

  for (int i = 0; i < STAR_BUFF_SIZE - 1; i++) {
    gmo.stars[i].x_pos = rand_range(1, CANV_W - 1);
    gmo.stars[i].y_pos = rand_range(1, CANV_H - 1);
    gmo.stars[i].z_pos = rand_range(2, 16);
  }
  for (int i = 0; i < BLDING_BUFF_SIZE; i++) {
    int x, y, x_min, x_max, y_min, y_max;
    do {
      x = rand_range(-1000, 1000), y = rand_range(-1000, 1000);
    } while (is_pos_free_BUILDINGS(i, x, y));
    init_building(rand_range(0, BLD_TYPE_COUNT - 2), &gmo.buildings[i], x, y);
  }
}

void init_game() {
  int n_items, n_quests;
  GLB_itm_list = read_items_from_file(ITEMFILE, &n_items);
  GLB_num_items = n_items;
  system("clear");
  for (int x = 0; x < n_items; x++)
    printf("%s %s\n", GLB_itm_list[x].name, itm_types_lb[GLB_itm_list[x].type]);
  while (getchar() == -1) {}

  GLB_quest_list = read_quest_from_file(QSTFILE, &n_quests);
  GLB_num_GLB_quests = n_quests;
  system("clear");
  for (int x = 0; x < n_quests; x++) {
    QUEST q = GLB_quest_list[x];
    if (q.q_type >= QUESTS_TYPE_COUNT) q.q_type = QUESTS_TYPE_COUNT - 1;
    printf("%d : %s %s (%s)\n%s\n", x, q.name, quest_types_lb[q.q_type], rarity_types_lb[q.rar], q.desc);
    printf("lv_rec: %d cur_am_rew:%d xp_rewrd:%d\n\n", q.level_req, q.currency_am_reward, q.xp_reward_am);
  }
  printf("\n\nQSTS AMOUNT: %d", GLB_num_GLB_quests);

  while (getchar() == -1) {}
  GLB_itm_ownd = malloc(INVENTORY_BUFF_SIZE * sizeof(ITEM));
  if (GLB_itm_list == NULL || GLB_itm_ownd == NULL) exit(1);
  for (int i = 0; i < INVENTORY_BUFF_SIZE; i++) {
    reset_item(&GLB_itm_ownd[i]);
    GLB_itm_ownd[i].content = '\0';
  }
  GLB_quests = malloc(QUESTS_BUFF_SIZE * sizeof(QUEST));
  for (int i = 0; i < QUESTS_BUFF_SIZE; i++) {
    reset_quest(&GLB_quests[i]);
  }
  Copy_Item(&GLB_ship, Get_Item_Of_type(SPACESHIP, GLB_itm_list, GLB_num_items));
  Copy_Item(&GLB_armor, Get_Item_Of_type(ARMOR, GLB_itm_list, GLB_num_items));
  Copy_Item(&GLB_weapon, Get_Item_Of_type(WEAPON, GLB_itm_list, GLB_num_items));
  GLB_minimap = NULL, GLB_cur_seller = NULL;
  GLB_game_state = nm, GLB_mv_typ = normal, GLB_atk_typ = press, GLB_allowed_dir = all, GLB_allowed_lk_dir = all;
  GLB_cur_proj = base, GLB_cur_blding = NONE;
  GLB_cur_bld_index = GLB_is_in_dialog = GLB_cur_floor = GLB_prj_col_index = GLB_plr_col_index = -1;
  GLB_score = GLB_max_score = GLB_proj_quality = GLB_luck = GLB_gl_x_pos = GLB_gl_y_pos = 0;
  GLB_is_framing = GLB_boss_mode = GLB_owned_amnt = GLB_inv_incrmnt = 0;
  GLB_level = 1, GLB_attr = 6;
  for (int x = 0; x < CURRENCY_COUNT - 1; x++) {
    GLB_currencies[x].amount = 500;
    GLB_currencies[x].type = x;
  }
}

void init_building(BLD_TYPE type, BLDING *bld, int x, int y) {
  int strt_ind = 0;
  int y_s = (type == HOUSE) ? rand_range(25, 35) : (type == SHOP) ? 33 : (type == FORTRESS) ? 35 : rand_range(30, 40), x_s = y_s * 2;
  int y_s_ext = y_s / 2, x_s_ext = y_s_ext * 2;
  bld->y_size = y_s, bld->x_size = x_s;
  bld->x_pos = x, bld->y_pos = y;
  bld->type = type;
  bld->floors = (char **)malloc(sizeof(char *) * 10);
  bld->y_s_ext = y_s_ext, bld->x_s_ext = x_s_ext;
  bld->npc_am = rand_range(2, 5), bld->npcs = malloc(sizeof(NPC) * bld->npc_am);

  char *txt = strdup(bld_types_lb[type]);
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
  if (bld->type == SHOP || bld->type == GLB_armorY) {
    char *GLB_ship = strdup(ship_full);
    bld->ext_cont = init_blank_canv(get_width(GLB_ship), y_s, 0, ' ');
    write_on_canv(GLB_ship, bld->ext_cont, 0, y_s - get_height(GLB_ship) - 1);
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

      char *item = strdup(rs == 0 ? luck_GFX_SMALL : rs == 1 ? SCOR_GFX_SMALL : rs == 2 ? SHIELD_GFX_SMALL : rs == 3 ? GUN_GFX_SMALL : rs == 4 ? attr_GFX_SMALL : HEART_GFX_SMALL);
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

    n->type = GLB_seller;
    n->name = strdup(npcs_names[rand_range(0, 199)]);
    n->floor = 0;
    n->text = strdup(bld->type == SHOP ? slr_dlgs[rand_range(0, 13)] : forg_dlgs[rand_range(0, 12)]);
    n->x_p = (x_s / 2) - 5, n->y_p = 5;
    n->dwn_bdy = strdup(seller_idle_down1), n->up_bdy = strdup(seller_idle_up);
    n->lft_bdy = strdup(seller_idle_left), n->rgt_bdy = strdup(seller_idle_right);
    n->lk_dir = rand_range(0, 4), n->dir = none;
    n->items = malloc(sizeof(ITEM) * SHOP_BUFF_SIZE);
    Generate_items(GLB_itm_list, n, SHOP_BUFF_SIZE, GLB_num_items - 1);
    n->items[SHOP_BUFF_SIZE - 1].content = '\0';

    system("clear");
    printf("NPC items:\n\n");
    for (int x = 0; x < SHOP_BUFF_SIZE - 1; x++) {
      n->items[x].am = rand_range(5, 99);
      printf("%d: %s am:%d\n", x, n->items[x].name, n->items[x].am);
    }
    usleep(10000);

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
        if (bld->type != SHOP && bld->type != GLB_armorY) bld->ext_cont[int_i] = y == height - 1 ? '-' : x == -size || x == size - 1 ? '|' : '#';
      }
    }
  }

  for (int i = strt_ind; i < bld->npc_am; i++) {
    bld->npcs[i].name = strdup(npcs_names[rand_range(0, 199)]);
    bld->npcs[i].type = rand_range(1, 3);
    bld->npcs[i].text = strdup(type == FORTRESS ? adv_dlgs[rand_range(0, 19)] : type == SHOP || type == GLB_armorY ? shpr_dlgs[rand_range(0, 19)] : expl_dlgs[rand_range(0, 19)]);
    bld->npcs[i].x_p = rand_range(5, bld->x_size - 5), bld->npcs[i].y_p = rand_range(5, bld->y_size - 5);
    bld->npcs[i].dwn_bdy = strdup(plr_idle_down1), bld->npcs[i].up_bdy = strdup(plr_idle_up), bld->npcs[i].lft_bdy = strdup(plr_idle_left),
    bld->npcs[i].rgt_bdy = strdup(plr_idle_right);
    bld->npcs[i].dir = rand_range(0, 4);
    bld->npcs[strt_ind].floor = rand_range(0, bld->flr_am - 1);
    QUEST q = Get_random_quest(GLB_quest_list, GLB_num_GLB_quests - 1);
    copy_quest(&bld->npcs[i].quest, q);
    bld->npcs[i].quest.giver = &bld->npcs[i];
  }
}

char *init_blank_canv(int w, int h, int has_ext, char interior) {
  // Allocate memory for the canvas: (width + 1 newline) * height, plus null terminator
  char *canv = malloc((w + 1) * h + 1);
  if (!canv) return NULL; // Check if malloc fails

  int i = 0;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if (x == 0 && y == 0) {
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? UPLFT_WALL : ' ';
      } else if (x == 0 && y == h - 1) {
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? DWNLFT_WALL : ' ';
      } else if (x == w - 1 && y == 0) {
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? UPRGT_WALL : ' ';
      } else if (x == w - 1 && y == h - 1) {
        canv[i++] = has_ext == 3 ? '.' : has_ext == 2 ? DWNRGT_WALL : ' ';
      } else if (x == 0 || x == w - 1) {
        canv[i++] = !has_ext ? ' ' : has_ext == 3 ? '|' : has_ext == 2 ? VRT_WALL : has_ext == 1 ? CANV_VER : COL_VER;
      } else if (y == 0) {
        canv[i++] = !has_ext ? ' ' : has_ext == 3 ? '_' : has_ext == 2 ? HOR_WALL : has_ext == 1 ? CANV_HOR : COL_HOR;
      } else if (y == h - 1) {
        canv[i++] = !has_ext ? ' ' : has_ext == 3 ? '-' : has_ext == 2 ? HOR_WALL : has_ext == 1 ? CANV_HOR : COL_HOR;
      } else {
        canv[i++] = interior;
      }
    }
    canv[i++] = '\n'; // Add a newline after each row
  }

  canv[i] = '\0'; // Null-terminate the string

  return canv;
}

int Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (prj[i].col_timer || (prj[i].x_pos > -1 && prj[i].y_pos > -1)) continue;
    prj[i].x_pos = x_pos, prj[i].y_pos = y_pos, prj[i].dir = dir, prj[i].type = type;
    return i;
  }
  return -1;
}

void init_boss(BOSS *boss) {
  GLB_game_state = boss_fight, GLB_boss_mode = 2;

  boss->content = strdup(SHIELD_GFX);
  boss->height = get_height(boss->content), boss->width = get_width(boss->content);
  boss->cur_hp = 200, boss->max_HP = 200, boss->prev_HP = 200, boss->max_shield = 50, boss->prv_shield = 50;
  boss->speed = 8, boss->shield = 0, boss->dmg = 5, boss->atk_spd = 20, boss->speATK_hole = 0, boss->hole_xdir = 1, boss->hole_size = PLR_SHIP_H + 4;
  boss->x_dir = 0, boss->y_dir = 0;
  boss->x_pos = CANV_X_CENTER - boss->width / 2, boss->y_pos = -boss->height + 1;
  boss->action = entrance, boss->next_action = shield, boss->time_in_state = 120;
  boss->shld_ic_count = 0;
  int i = 0;
  while (boss->content[i])
    if (boss->content[i++] == SHLD_OFF_MOB_IC) boss->shld_ic_count++;
}