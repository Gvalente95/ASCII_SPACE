#include "../headers/includes.h"

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

int contains(Loot_Type *array, int length, Loot_Type value) {
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
  if (allowed_directions == vert) return (dir == up || dir == down);
  if (allowed_directions == hor) return (dir == left || dir == right);
  if (allowed_directions == hor_up) return (dir == left || dir == right || dir == up);
  if (allowed_directions == hor_down) return (dir == left || dir == right || dir == down);
  if (allowed_directions == ver_left) return (dir == up || dir == down || dir == left);
  if (allowed_directions == ver_right) return (dir == up || dir == down || dir == right);
  return 0;
}

int get_width(char *src) {
  int i = 0;
  int cur_x = 0;
  int max_x = cur_x;

  while (src[i]) {
    if (src[i] == '\n') {
      if (max_x < cur_x) max_x = cur_x;
      cur_x = 0;
    }
    cur_x++;
    i++;
  }
  return (max_x);
}

int get_height(char *src) {
  int i = 0;
  int y_size = 0;
  while (src[i]) {
    if (src[i] == '\n') y_size++;
    i++;
  }
  return (y_size);
}

int crop_x(char *src, int from_x) {
  int i = 0;
  while (src[i]) {
    int next_end = 0;
    while (src[i] && src[i] != '\n' && next_end < from_x) {
      src[i++] = ' ';
      next_end++;
    }
    while (src[i] && src[i] != '\n')
      i++;
    if (src[i] == '\n') i++;
  }
  return 0;
}

int is_in_string(char c, char *str) {
  int index = 0;
  while (str[index])
    if (str[index++] == c) return 1;
  return 0;
}

int is_pos_free(SHIP *mobs, int current_mob, int x, int y) {
  for (int i = 0; i < current_mob; i++)
    if (mobs[i].x_pos == x && mobs[i].y_pos == y) return 1;
  return 0;
}