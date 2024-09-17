#include "../../headers/1_includes.h"

void set_nonblocking_mode(int enable) {
  static struct termios oldt, newt;
  if (enable) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, 0);
  }
}

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }

DIR get_prj_dir(int w_prj, int max_weap, SHIP plr, DIR d) {
  if (w_prj == max_weap) return (plr.lk_dir);
  if (w_prj == max_weap - 1) return (d == up ? down : d == down ? up : d == left ? right : left);
  if (w_prj == max_weap - 2) return (d == up ? left : d == down ? right : d == left ? up : down);
  if (w_prj == max_weap - 3) return (d == up ? right : d == down ? left : d == left ? down : up);
  if (w_prj == max_weap - 4) return (d == up ? up_right : d == down ? up_left : d == left ? down_left : down_right);
  if (w_prj == max_weap - 5) return (d == up ? up_left : d == down ? up_right : d == left ? down_right : down_left);
  if (w_prj == max_weap - 6) return (d == up ? down_left : d == down ? down_right : d == left ? up_left : up_right);
  if (w_prj == max_weap - 7) return (d == up ? down_right : d == down ? down_left : d == left ? up_right : up_left);
  return up;
}

DIR get_dir_to_target(int target_x, int target_y, int x_pos, int y_pos, int has_diag) {
  DIR dir;
  int diff_x = target_x - x_pos + 3;
  int diff_y = target_y - y_pos + 3;
  if (abs(diff_x) > abs(diff_y)) {
    if (diff_y == 0)
      dir = diff_x > 0 ? right : left;
    else
      dir = diff_x > 0 ? has_diag ? (diff_y > 0 ? down_right : up_right) : right : has_diag ? (diff_y > 0 ? down_left : up_left) : left;
  } else {
    if (diff_x == 0)
      dir = diff_y > 0 ? down : up;
    else
      dir = diff_y > 0 ? has_diag ? (diff_x > 0 ? down_right : down_left) : down : has_diag ? (diff_x > 0 ? up_right : up_left) : up;
  }
  return (dir);
}

int contains(ITEM_Type *array, int length, ITEM_Type value) {
  if (array == NULL) return 0;
  for (int i = 0; i < length; i++)
    if (array[i] == value) return 1;
  return 0;
}

int check_overlap(BLDING *a, BLDING *b) {
  return !(a->x_pos + a->x_size <= b->x_pos || // a is to the left of b
           a->x_pos >= b->x_pos + b->x_size || // a is to the right of b
           a->y_pos + a->y_size <= b->y_pos || // a is above b
           a->y_pos >= b->y_pos + b->y_size);  // a is below b
}

int is_dir_allowed(DIR dir, DIR allowed_directions) {
  if (allowed_directions == none) return 0;
  if (allowed_directions == all || dir == allowed_directions) return 1;
  if (allowed_directions == down_left) return (dir == down || dir == left);
  if (allowed_directions == down_right) return (dir == down || dir == right);
  if (allowed_directions == up_left) return (dir == up || dir == left);
  if (allowed_directions == up_right) return (dir == up || dir == right);
  if (allowed_directions == vert) return (dir == up || dir == down);
  if (allowed_directions == hor) return (dir == left || dir == right);
  if (allowed_directions == hor_up) return (dir == left || dir == right || dir == up);
  if (allowed_directions == hor_down) return (dir == left || dir == right || dir == down);
  if (allowed_directions == ver_left) return (dir == up || dir == down || dir == left);
  if (allowed_directions == ver_right) return (dir == up || dir == down || dir == right);
  return 0;
}

int get_width(const char *src) {
  if (src == NULL) return 0;
  int max_x = 0, cur_x = 0;

  while (*src) {
    if (*src == '\n') {
      if (cur_x > max_x) max_x = cur_x;
      cur_x = 0;
    } else
      cur_x++;
    src++;
  }
  if (cur_x > max_x) max_x = cur_x;
  return max_x;
}

int get_height(const char *src) {
  if (src == NULL) return 0;
  int y_size = 0;
  while (*src) {
    if (*src == '\n') y_size++;
    src++;
  }
  return y_size + 1;
}

char get_index_value(char *canv, int c_width, int c_height, int x, int y) {
  if (canv == NULL) return '\0';
  if (c_width == -1) c_width = get_width(canv);
  if (c_height == -1) c_height = get_height(canv);
  if ((x < 0 || x > c_width) || (y < 0 || y > c_height)) return ('\0');
  return (canv[x + ((c_width + 1) * y)]);
}

int get_index(char *canv, int x_pos, int y_pos) {
  int width = get_width(canv), height = get_height(canv);
  if (x_pos > width || y_pos > height) return -1;
  return (x_pos + (y_pos * (width + 1)));
}

int is_in_string(char c, char *str) {
  int index = 0;
  if (str == NULL) return 0;
  while (str[index])
    if (str[index++] == c) return 1;
  return 0;
}

int is_pos_free(SHIP *mobs, int current_mob, int x, int y) {
  for (int i = 0; i < current_mob; i++)
    if (mobs[i].x_pos == x && mobs[i].y_pos == y) return 1;
  return 0;
}

int is_pos_free_BUILDINGS(int current_mob, int x, int y) {
  BLDING *bldings = gmo.buildings;
  for (int i = 0; i < current_mob; i++)
    if ((bldings[i].x_pos >= x - 50 && bldings[i].x_pos <= x + 50) && bldings[i].y_pos >= y - 50 && bldings[i].y_pos <= y + 50) return 1;
  return 0;
}

void tile_canvas(char *canv, int width, int tile_size) {
  int i = 0;
  if (canv == NULL) return;
  while (canv[i]) {
    int row = i / width;
    int col = i % width;
    int col_half = col / 2;
    if (row % tile_size == 0 && col_half % tile_size == 0 && (canv[i] == ' ' || canv[i] == '#')) {
      canv[i] = '+';
    } else if (col_half % tile_size == 0 && (canv[i] == ' ' || canv[i] == '#')) {
      canv[i] = '|';
    } else if (row % tile_size == 0 && (canv[i] == ' ' || canv[i] == '#')) {
      canv[i] = '_';
    }
    i++;
  }
}
