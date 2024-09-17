#include "../../headers/1_includes.h"

const int wind_x = 6, wind_y = 3;
const int wind_width = (CANV_W - (wind_x * 2)), wind_height = CANV_H - (wind_y * 2);
const int msg_width = wind_width - 15, msg_height = 5;
const int itm_zone_xP = 5, itm_zone_yP = 5;

const int type_menu = 0;
const int page_0 = -1;
const int page_1 = -2;
const int page_2 = -3;

typedef enum {
  STATE_INVENTORY, // Page 0
  STATE_STATS,     // Page 1
  STATE_quests,    // Page 2
  STATE_ITEM_SELECTION,
  STATE_NONE // End or unrecognized state
} MenuState;

int handle_seller_menu(char *canv, NPC *seller, SHIP *plr) {
  int wind_x = 6, wind_y = 3;
  int wind_width = (CANV_W - (wind_x * 2)), wind_height = CANV_H - (wind_y * 2);
  int msg_width = wind_width - 15, msg_height = 6;
  int itm_zone_xP = 5, itm_zone_yP = 5;
  int y_dist = -1, x_dist = 5, inp = -1;
  int sel = -1, page = 0, obj_sel = 0, type_sel = 0;
  int is_at_end = 0;
  int has_done_something = 0;
  ITEM *sel_itm;
  GLB_game_previous_state = GLB_game_state;

  while (inp != 'q') {
    char *window = init_blank_canv(wind_width, wind_height, 1, '/');
    char *interior = init_blank_canv(wind_width - 2, wind_height - 2, 1, '#');
    write_on_canv(interior, window, 1, 1);

    char *buy_SQ = init_blank_canv(5, 3, sel == -1 ? 1 : page == 0 ? 2 : 0, ' '), *sell_SQ = init_blank_canv(6, 3, sel == -2 ? 1 : page == 1 ? 2 : 0, ' ');
    char *discuss_SQ = init_blank_canv(10, 3, sel == -3 ? 1 : page == 2 ? 2 : 0, ' ');
    write_on_canv("BUY", buy_SQ, 1, 1), write_on_canv("SELL", sell_SQ, 1, 1), write_on_canv("DISCUSS", discuss_SQ, 1, 1);
    write_on_canv(buy_SQ, window, 5, 2), write_on_canv(sell_SQ, window, 15, 2), write_on_canv(discuss_SQ, window, 25, 2);
    free(buy_SQ), free(sell_SQ), free(discuss_SQ);

    if (sel >= 0) {
      char *itm_zone = init_blank_canv(wind_width - itm_zone_xP * 2, wind_height - itm_zone_yP - 3, 1, '#');
      write_on_canv(itm_zone, window, itm_zone_xP, itm_zone_yP);
      free(itm_zone);
    }

    char fortune[16];
    snprintf(fortune, 15, "$%d", GLB_currencies[0].amount);
    write_on_canv(fortune, window, wind_width - 10, 3);

    if (page == 2) {
      int seller_x = wind_width / 2, seller_y = wind_height / 2;
      char *text = strdup(slr_dlgs[rand_range(0, 12)]);
      char *bub = get_text_bubble(text, seller->name);
      write_on_canv(bub, window, seller_x - get_width(text) / 2, seller_y - get_height(bub));
      write_on_canv(seller_idle_down1, window, seller_x, seller_y);
      char *table = init_blank_canv(20, 5, 2, '.');
      write_on_canv(table, window, seller_x - 10, seller_y + 3);
      free(bub), free(text), free(table);
    } else {
      for (int i = 0; i < ITEM_Type_COUNT - 1; i++) {
        int x_dist = 15;
        char *lbl_txt = strdup(itm_types_lb[i]);
        int size = strlen(lbl_txt);
        char *lbl_sq = init_blank_canv(x_dist - 2, 3, i == type_sel ? 1 : 0, '#');
        write_on_canv(lbl_txt, lbl_sq, ((x_dist - 2) / 2) - (size / 2), 1);
        write_on_canv(lbl_sq, window, itm_zone_xP + 2 + i * x_dist, itm_zone_yP + 1);
        free(lbl_sq), free(lbl_txt);
      }
      int filtered_count;
      ITEM *base = page == 0 ? seller->items : GLB_itm_ownd;
      ITEM **itms = filter_items_by_type(base, type_sel, page == 0 ? SHOP_BUFF_SIZE - 1 : INVENTORY_BUFF_SIZE - 1, &filtered_count);
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
          char *type_txt = strdup(itm_types_lb[itm->type]);
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
    render_canvas(canv, NULL);
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
    type_sel = MAX(type_sel, 0), type_sel = MIN(type_sel, ITEM_Type_COUNT - 2);

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
        if ((GLB_currencies[0].amount >= price && page == 0) || page == 1) snprintf(txt, 99, "%s %s for %d$ ?", action, name, sel_itm->price);
        char *box = init_blank_canv(get_width(txt) + 4, 5, 1, '#');
        write_on_canv(txt, box, 2, 2);
        write_on_canv(box, window, wind_width / 2 - get_width(box) / 2, wind_height / 2 - get_height(box) / 2);
        write_on_canv(window, canv, wind_x, wind_y);
        render_canvas(canv, NULL);
        free(box), free(name);
        int np = -1;
        if ((GLB_currencies[0].amount < price && page == 0)) {
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
            int dupl = (Get_itm_duplicate_index(*sel_itm, GLB_itm_ownd, INVENTORY_BUFF_SIZE - 1));
            if (dupl != -1)
              GLB_itm_ownd[dupl].am++;
            else
              Copy_Item(&GLB_itm_ownd[GLB_owned_amnt], *sel_itm);
            GLB_currencies[0].amount -= sel_itm->price;
            GLB_owned_amnt++;
            sel_itm->am--;
            has_done_something = 1;
          }
          if (page == 1) {
            GLB_currencies[0].amount += sel_itm->price / 2;
            for (int x = 0; x < GLB_owned_amnt; x++) {
              ITEM *itm = &GLB_itm_ownd[x];
              if (itm->name == sel_itm->name) itm->am--;
            }
            has_done_something = 1;
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
  GLB_game_state = GLB_game_previous_state;
  GLB_is_in_dialog = -1;
  if (has_done_something) {
    reorder_list(GLB_itm_ownd, INVENTORY_BUFF_SIZE - 1);
    reorder_list(seller->items, SHOP_BUFF_SIZE - 1);
  }

  return 1;
}

void handle_page_zero(ITEM *sel_itm, ITEM_Type type_sel, char *window, int *is_at_end, int *sel_index, int page, int y_dist) {
  for (int i = 0; i < ITEM_Type_COUNT; i++) {
    int x_dist = 13;
    char *lbl_txt = strdup(itm_types_lb[i]);
    int size = strlen(lbl_txt);
    char *lbl_sq = init_blank_canv(x_dist - 2, 3, i == type_sel && *sel_index >= type_menu ? 1 : 0, '#');
    write_on_canv(lbl_txt, lbl_sq, ((x_dist - 2) / 2) - (size / 2), 1);
    write_on_canv(lbl_sq, window, itm_zone_xP + 2 + i * x_dist, itm_zone_yP + 1);
    free(lbl_sq), free(lbl_txt);
  }
  int filtered_count;
  ITEM **itms = filter_items_by_type(GLB_itm_ownd, type_sel, INVENTORY_BUFF_SIZE - 1, &filtered_count);

  if (itms == NULL) return;
  //*sel_index = MINMAX(*sel_index, 0, filtered_count - 1);

  int show_am = 13;
  int item_found = 0;

  int obj_sel = MAX(*sel_index - show_am + 1, 0);
  for (int i = 0; i < show_am && i + obj_sel < filtered_count; i++) {
    ITEM *itm = itms[i + obj_sel];
    if (!itm || itm->name[0] == '\0' || itm->desc[0] == '\0') continue;

    *is_at_end = (*sel_index == i && itm->content == '\0');

    if (itm->content == '\0') {
      continue;
    } else {
      int x_pos = itm_zone_xP + 3;
      int y_pos = 4 + itm_zone_yP + (i * (y_dist + msg_height - 2));
      int is_sel = i == *sel_index || (i == show_am - 1 && *sel_index >= show_am - 1) ? 1 : 0;
      char *item_bg = init_blank_canv(msg_width, is_sel ? msg_height : msg_height - 2, is_sel ? 1 : 0, '#');
      item_found = 1;

      char *name = strdup(itm->name);
      Color_from_index((int)itm->rar, &name);

      char *txt = malloc(500);
      snprintf(txt, 499, "x%d %s", itm->am, name);
      if (is_sel) {
        sel_itm = itms[i + obj_sel];
        snprintf(txt + strlen(txt), 499, "\n%s\n$%d", itm->desc, itm->price);
      }
      if (i > *sel_index) y_pos += 2;
      write_on_canv(txt, item_bg, is_sel ? 6 : 3, 1);
      write_on_canv(item_bg, window, x_pos, y_pos);
      free(item_bg), free(name), free(txt);
    }
  }
  if (itms != NULL) free(itms);
  if (!item_found) {
    int st_index = get_index(window, wind_width / 2 - 10, wind_height / 2);
    set_label(window, &st_index, "NO ITEMS OF THIS TYPE", '\0');
    *is_at_end = 1;
  }
}

void handle_page_one(ITEM *sel_itm, int *sel_index, int *is_at_end, char *window) {
  int num_values = 3;
  int y_pos = 10;
  for (int x = 0; x < num_values; x++) {
    int x_pos = 20;
    y_pos += 2;
    sel_itm = *sel_index == 0 ? &GLB_ship : *sel_index == 1 ? &GLB_weapon : *sel_index == 2 ? &GLB_armor : &GLB_ship;
    ITEM itm = x == 0 ? GLB_ship : x == 1 ? GLB_weapon : x == 2 ? GLB_armor : GLB_ship;
    char *value = strdup(itm.name), *label = strdup(itm_types_lb[itm.type]);
    Color_from_index((int)itm.rar, &value);
    char *blok = init_blank_canv((get_width(itm.name)) + 1, 1, *sel_index == x ? 1 : 0, '#');
    int st_index = get_index(blok, 1, 1);
    set_label(blok, &st_index, value, '\0');
    write_on_canv(label, window, x_pos, y_pos);
    write_on_canv(value, window, x_pos + 15, y_pos);
    write_on_canv(blok, window, x_pos + 14, y_pos + 1);
    free(blok), free(value), free(label);
  }
  *is_at_end = (*sel_index >= num_values - 1);

  char *wp_ic = malloc(2);
  wp_ic[0] = GLB_weapon.content, wp_ic[1] = '\0';
  Color_from_index((int)GLB_weapon.rar, &wp_ic);
  write_on_canv(&wp_ic[0], window, wind_width / 2 - 3, wind_height / 2 + 3);
  write_on_canv(plr_idle_down1, window, wind_width / 2, wind_height / 2);
}

void handle_page_two(QUEST *selected_qst, int *sel_index, int *sel_type, int *is_at_end, char *window) {
  *sel_type = MINMAX(*sel_type, 0, QUEST_STATUS_COUNT - 1);
  for (int i = 0; i < QUEST_STATUS_COUNT; i++) {
    int x_dist = 13;
    char *lbl_txt = strdup(quest_status_lb[i]);
    int size = strlen(lbl_txt);
    char *lbl_sq = init_blank_canv(x_dist - 2, 3, i == *sel_type && *sel_index >= type_menu ? 1 : 0, '#');
    write_on_canv(lbl_txt, lbl_sq, ((x_dist - 2) / 2) - (size / 2), 1);
    write_on_canv(lbl_sq, window, itm_zone_xP + 2 + i * x_dist, itm_zone_yP + 1);
    free(lbl_sq), free(lbl_txt);
  }
  int filtered_count;
  QUEST **quests = filter_quests_by_status(GLB_quests, *sel_index, INVENTORY_BUFF_SIZE - 1, &filtered_count);

  // if (quests == NULL || filtered_count <= 0) return;

  int show_am = 13;
  int quest_found = 0;

  int qst_sel = MAX(*sel_index - show_am + 1, 0);
  for (int i = 0; i < show_am && i + qst_sel < filtered_count; i++) {
    QUEST *q = quests[i + qst_sel];
    if (!q || q->name[0] == '\0' || q->q_status != *sel_type) continue;

    *is_at_end = (*sel_index == i && q->name[0] == '\0');

    quest_found = 1;
    int is_sel = i + 1 == *sel_index || (i == show_am - 1 && *sel_index >= show_am - 1) ? 1 : 0;

    if (is_sel) selected_qst = q;
    char *qst_bg = init_blank_canv(msg_width, is_sel ? msg_height : msg_height - 2, is_sel ? 1 : 0, '#');
    int x = is_sel ? 6 : 3;
    int st_index = get_index(qst_bg, x, 1);
    char *type_txt = strdup(quest_types_lb[q->q_type]);
    char *name = strdup(q->name);
    char *desc = strdup(q->desc);
    Color_from_index((int)q->rar, &name);
    set_label(qst_bg, &st_index, "x", q->cur_am);
    set_label(qst_bg, &st_index, name, '\0');
    st_index = get_index(qst_bg, x, 2);
    set_label(qst_bg, &st_index, type_txt, '\0');
    if (is_sel) {}
    int x_pos = itm_zone_xP + 3;
    int y_pos = 4 + itm_zone_yP + (i * (3 + msg_height - 2));
    if (i > *sel_index) y_pos += 2;
    write_on_canv(qst_bg, window, x_pos, y_pos);
    free(qst_bg), free(type_txt), free(name), free(desc);
  }
  if (quests != NULL) free(quests);
  if (!quest_found) {
    int st_index = get_index(window, wind_width / 2 - 10, wind_height / 2);
    set_label(window, &st_index, "NO quests OF THIS TYPE", '\0');
    *is_at_end = 1;
  }
}

MenuState handle_navigation_input(int *inp, int *sel_index, int *page, int *type_sel, int *is_at_end) {
  *inp = getchar();
  int prv_sel = *sel_index;
  int prv_type = *type_sel;
  int prv_page = *page;

  // Update based on input
  if (*inp == 's') {
    if (*is_at_end) {
      *sel_index = *sel_index - 1;
      *is_at_end = 0;
    }
    if (*sel_index <= page_0)
      *sel_index = type_menu;
    else
      *sel_index += 1;
  }
  if (*inp == 'w') {
    if (*sel_index == type_menu)
      *sel_index = -*page - 1;
    else if (*sel_index > type_menu)
      *sel_index -= 1;
  }
  if (*inp == 'd') {
    if (*sel_index < type_menu) {
      *sel_index -= 1;
      *page += 1;
    }
    if (*sel_index == type_menu) {
      *type_sel += 1;
      int max = *page == 2 ? QUEST_STATUS_COUNT - 1 : ITEM_Type_COUNT - 1;
      if (*type_sel > max) *type_sel = max;
    }
  }
  if (*inp == 'a') {
    if (*sel_index < page_0) {
      *page -= 1;
      *sel_index += 1;
    }
    if (*sel_index == type_menu) {
      *type_sel -= 1;
      if (*type_sel < 0) *type_sel = 0;
    }
  }

  if (prv_type != *type_sel || prv_sel != *sel_index) play_sound(AU_click1, 0);
  if (*inp == 'q') {
    play_sound(AU_high_pitch, 0);
    return STATE_NONE;
  } else if (*inp == ' ') {
    play_sound(AU_change_sel, 0);
    return STATE_ITEM_SELECTION;
  }

  *page = MINMAX(*page, 0, 2);

  *sel_index = MAX(*sel_index, page_2);
  if (prv_page != *page) *type_sel = 0;
  // Update page and type selector

  return (*inp == 'q') ? STATE_NONE : *page;
}

void render_navigation_bar(char *window, int page, int sel, int type) {

  char *itm_zone = init_blank_canv(wind_width - itm_zone_xP * 2, wind_height - itm_zone_yP - 3, sel >= 0 ? 1 : 0, '#');
  char *invtry_SQ = init_blank_canv(5, 3, sel == -1 ? 1 : page == 0 ? 2 : 0, ' ');
  char *stats_SQ = init_blank_canv(8, 3, sel == -2 ? 1 : page == 1 ? 2 : 0, ' ');
  char *quests_SQ = init_blank_canv(10, 3, sel == -3 ? 1 : page == 2 ? 2 : 0, ' ');

  write_on_canv("INV", invtry_SQ, 1, 1);
  write_on_canv("STATS", stats_SQ, 1, 1);
  write_on_canv("quests", quests_SQ, 1, 1);
  int index = 0;
  set_label(window, &index, "sel:", sel), set_label(window, &index, "pg:", page), set_label(window, &index, "typ:", type);

  write_on_canv(invtry_SQ, window, 5, 2);
  write_on_canv(stats_SQ, window, 15, 2);
  write_on_canv(quests_SQ, window, 25, 2);
  write_on_canv(itm_zone, window, itm_zone_xP, itm_zone_yP);

  free(invtry_SQ);
  free(stats_SQ);
  free(quests_SQ);
  free(itm_zone);
}

void handle_item_selection(ITEM *sel_itm, char *window, char *canv) {
  if (sel_itm == NULL || sel_itm->content == '\0') return;

  char *txt = malloc(100);
  snprintf(txt, 99, "%s %s?", sel_itm->type == WEAPON || sel_itm->type == ARMOR ? "EQUIP" : "USE", sel_itm->name);

  char *box = init_blank_canv(get_width(txt) + 4, 5, 1, '#');
  write_on_canv(txt, box, 2, 2);
  write_on_canv(box, window, wind_width / 2 - get_width(box) / 2, wind_height / 2 - get_height(box) / 2);
  write_on_canv(window, canv, wind_x, wind_y);
  render_canvas(canv, NULL);

  free(box);
  free(txt);

  // Handle item use/equip action
  while (1) {
    int np = getchar();
    if (np == ' ') {
      // Apply item logic here...
      break;
    }
  }
}

void handle_inventory_menu(char *canv) {
  int sel = 0, page = 0, type_sel = 0, is_at_end = 0;
  int inp = -1;
  ITEM *sel_itm = NULL;
  QUEST *sel_qst = NULL;
  MenuState state = STATE_INVENTORY;

  while (state != STATE_NONE) {
    char *window = init_blank_canv(wind_width, wind_height, 1, '/');
    char *interior = init_blank_canv(wind_width - 2, wind_height - 2, 1, '#');
    write_on_canv(interior, window, 1, 1);

    // Display navigation bar
    render_navigation_bar(window, page, sel, type_sel);

    // Handle current state
    switch (state) {
      case STATE_INVENTORY:
        handle_page_zero(sel_itm, type_sel, window, &is_at_end, &sel, page, -1);
        break;
      case STATE_STATS:
        handle_page_one(sel_itm, &sel, &is_at_end, window);
        break;
      case STATE_quests:
        handle_page_two(sel_qst, &sel, &type_sel, &is_at_end, window);
        break;
      case STATE_ITEM_SELECTION:
        handle_item_selection(sel_itm, window, canv);
        state = STATE_INVENTORY;
        break;
      default:
        break;
    }

    // Write to canvas
    write_on_canv(window, canv, wind_x, wind_y);
    render_canvas(canv, window);

    // Handle navigation input and state transition
    state = handle_navigation_input(&inp, &sel, &page, &type_sel, &is_at_end);

    // Clean up
    free(window);
    free(interior);
    usleep(50000); // Add delay to make the input less sensitive
  }
  GLB_game_state = GLB_game_previous_state; // Restore previousstate
}
