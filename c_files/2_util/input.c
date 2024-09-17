#include "../../headers/1_includes.h"

int upd_input(char *canv, BOSS boss, SHIP *plr, int *has_moved, int *show_grid, int *god_mode) {
  (void)boss;

  int c = -1;
  if (GLB_is_framing) {
    while (c == EOF) {
      c = getchar();
      if (c == '<') return (1);
    }
    GLB_is_framing = !GLB_is_framing;
  }

  c = getchar();
  if (is_in_string(c, "wasd") && GLB_cur_blding != NONE) play_sound(AU_thump, 0);
  if (c == 'M') {
    if (GLB_game_state == map_menu)
      GLB_game_state = GLB_game_previous_state;
    else
      GLB_game_state = map_menu;
    return (c);
  }
  GLB_shift_on = (c >= 'A' && c <= 'Z') ? 1 : 0;
  if (GLB_shift_on) {
    c += 32;
    plr->dir = none;
  }
  if (c == 'p' || c == 'q') {
    write_midle_msg(canv, 1, "Press 'q' to quit");
    render_canvas(canv, NULL);
    int ch = getchar();
    while (ch == -1) {
      ch = getchar();
      if (ch == 'q') {
        system("pkill afplay");
        exit(0);
      }
    }
  }
  if (is_in_string(c, "wasd ") && GLB_mv_typ == cutscene) return (c);

  if (c != EOF) {
    if (!is_in_string(c, "wasdpm12gh<q \033\t")) plr->dir = none;
    switch (c) {
      case '\t':
        GLB_cur_proj++;
        if (GLB_cur_proj == PRJ_TYPES_COUNT) GLB_cur_proj = 0;
        return ('t');
      case '\033':
        getchar();
        char arrow = getchar();
        DIR lk_dir = arrow == 'A' ? up : arrow == 'B' ? down : arrow == 'C' ? right : left;
        if (is_dir_allowed(lk_dir, GLB_allowed_lk_dir)) plr->lk_dir = lk_dir;
        return (arrow == 'A' ? '^' : arrow == 'B' ? '$' : arrow == 'C' ? '<' : '>');
      case ' ':
        if (GLB_atk_typ != disabled && !plr->atk_reload) {
          play_sound(AU_high_pitch, 0);
          launch_proj(plr, gmo.proj);
        }
        return (c);
      case 'w':
      case 'a':
      case 's':
      case 'd': {
        DIR d = plr->lk_dir;
        if (c == 'a' && plr->x_pos > 2)
          d = left, *has_moved = PLR_GRID_SPD;
        else if (c == 'd' && plr->x_pos < CANV_W - (CELL_SIZE - 5 + PLR_SHIP_W * 2))
          d = right, *has_moved = PLR_GRID_SPD;
        else if (c == 'w' && plr->y_pos > 2)
          d = up, *has_moved = PLR_GRID_SPD;
        else if (c == 's' && plr->y_pos < CANV_H - PLR_SHIP_H)
          d = down, *has_moved = PLR_GRID_SPD;
        if (is_dir_allowed(d, GLB_allowed_dir)) {
          plr->dir = d;
          if (GLB_cur_blding != NONE) plr->lk_dir = d;
        } else if (GLB_game_state != boss_fight)
          plr->dir = none;
      }
      case 'm':
        *show_grid = !show_grid;
        return (c);
      case '1':
        GLB_mv_typ = normal;
        break;
      case '2':
        GLB_mv_typ = on_grid;
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
        GLB_is_framing = 1;
        return (c);
    }
    return (c);
  }
  return (c);
}

int get_char_and_freeze(char *un_blocking_chars) {
  int can_escape = 0;
  int inp = -1;
  while (!can_escape) {
    inp = getchar();
    if (un_blocking_chars != NULL) {
      if (is_in_string(inp, un_blocking_chars)) can_escape = 1;
    } else if (inp != -1)
      can_escape = 1;
  }
  return (inp);
}
