#include "header.h"

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

char *draw_triangle(char interior, DIR dir, int h, int w, int color) {

  if (dir == left || dir == right) {
    char *res;
    res = strdup(dir == left ? SHP_R : SHP_L);
    if (color) {
      int i = 0;
      while (res[i]) {
        if (res[i] == '|')
          res[i] = color == 1 ? MOB_VER : SHP_VER;
        else if (res[i] == '_')
          res[i] = color == 1 ? MOB_HOR : SHP_HOR;
        else if (res[i] == '/')
          res[i] = color == 1 ? MOB_LBORD : SHP_LBORD;
        else if (res[i] == '\\')
          res[i] = color == 1 ? MOB_RBORD : SHP_RBORD;
        i++;
      }
    }
    return (res);
  }

  int i = 0;
  int max_width = h * 2;
  int size = h * (max_width + 1) + 1;
  char *new_try = malloc(size);
  int incr = 0;

  char l_bord = color == 1 ? MOB_LBORD : color == 2 ? SHP_LBORD : '/';
  char r_bord = color == 1 ? MOB_RBORD : color == 2 ? SHP_RBORD : '\\';
  char top = color == 1 ? MOB_HOR : color == 2 ? SHP_HOR : '_';

  if (new_try == NULL) {
    return NULL;
  }

  switch (dir) {
  case up:
  case up_left:
  case up_right:
    for (int row = 0; row < h; row++) {
      int start = h - row - 1;
      int end = h + row + 1;

      for (int col = 0; col < max_width; col++) {
        if (col < start) {
          new_try[i++] = ' '; // Leading spaces
        } else if (col == start) {
          new_try[i++] = l_bord;
        } else if (col > start && col < end - 1) {
          new_try[i++] = (row == h - 1) ? top : interior;
        } else if (col == end - 1) {
          new_try[i++] = r_bord;
        }
      }
      new_try[i++] = '\n';
    }
    break;

  case down:
  case down_left:
  case down_right:
  case left:
  case right:
  case none:
    for (int row = h - 1; row >= 0; row--) {
      int start = h - row - 1;
      int end = h + row + 1;

      for (int col = 0; col < max_width; col++) {
        if (col < start) {
          new_try[i++] = ' '; // Leading spaces
        } else if (col == start) {
          new_try[i++] = r_bord;
        } else if (col > start && col < end - 1) {
          new_try[i++] = (row == h - 1) ? top : interior;
        } else if (col == end - 1) {
          new_try[i++] = l_bord;
        }
      }
      new_try[i++] = '\n';
    }
    break;
  }

  new_try[i] = '\0';
  return new_try;
}

char *init_blank_canv(int w, int h, int has_ext, char interior) {
  char *canv = malloc(w + (w * h) + 1);
  int i = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if ((x == 0 && y == 0) || (x == 0 && y == h - 1) || (x == w - 1 && y == 0) || (x == w - 1 && y == h - 1))
        canv[i++] = ' ';
      else if (x == 0 || x == w - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 1 ? CANV_VER : COL_VER;
      else if (y == 0 || y == h - 1)
        canv[i++] = !has_ext ? ' ' : has_ext == 1 ? CANV_HOR : COL_HOR;
      else
        canv[i++] = interior;
    }
    canv[i++] = '\n';
  }
  canv[i] = '\0';
  return (canv);
}

int get_width(char *src) {
  int i = 0;
  int cur_x = 0;
  int max_x = cur_x;

  while (src[i]) {
    if (src[i] == '\n') {
      if (max_x < cur_x)
        max_x = cur_x;
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
    if (src[i] == '\n')
      y_size++;
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
    if (src[i] == '\n')
      i++;
  }
  return 0;
}

char *erase_on_text(char *src, int new_width) {
  int i = 0, u = 0, cur_x = 0, size = 0;
  while (src[size])
    size++;
  char *new_txt = malloc(size + 1);
  if (!new_txt)
    return NULL;
  while (i < size) {
    if (cur_x > new_width) {
      new_txt[i++] = '\n';
      cur_x = 0;
    } else {
      new_txt[i] = src[i + u];
      if (src[i + u] == '\n')
        cur_x = 0;
      else
        cur_x++;
      i++;
    }
    if (cur_x == new_width && src[i + u] != '\n') {
      while (src[i + u] && src[i + u] != '\n') {
        u++;
      }
    }
  }
  new_txt[i] = '\0';
  return new_txt;
}

int print_on_text(char *src, char *dst, int x_pos, int y_pos) {

  int i = 0;
  int u = 0;
  int width = get_width(src);
  int height = get_height(src);

  int dst_width = get_width(dst);
  int dst_height = get_height(dst);

  if (x_pos >= dst_width || x_pos + width < 0 || y_pos > dst_height || y_pos + height < 0)
    return (0);

  while (y_pos > 0)
    if (dst[i++] == '\n')
      y_pos--;

  while (y_pos < 0) {
    if (src[u++] == '\n')
      y_pos++;
  }

  if (x_pos < 0)
    crop_x(src, abs(x_pos));

  if (x_pos + width > dst_width - 1)
    src = erase_on_text(src, dst_width - 1 - x_pos);

  i += x_pos;

  while (src[u]) {
    if (src[u] && src[u] == '\n') {
      while (dst[i] && dst[i] != '\n')
        i++;
      if (!src[u] || !dst[i])
        break;
      u++;
      i += x_pos + 1;
    }
    if (!src[u] || !dst[i])
      break;
    else if (src[u] != ' ')
      dst[i] = src[u];
    i++;
    u++;
  }
  return (1);
}

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }

int is_position_occupied(SHIP *mobs, int current_mob, int x, int y) {
  for (int i = 0; i < current_mob; i++) {
    if (mobs[i].x_pos == x && mobs[i].y_pos == y) {
      return 1;
    }
  }
  return 0;
}

void print_colored_text(char *c, char cc, char *color) {
  if (color != NULL) {
    if (c != NULL) {
      printf("%s%s\033[0m", color, c);
    }
    if (cc != '\0') {
      printf("%s%c\033[0m", color, cc);
    }
  } else {
    if (c != NULL) {
      printf("%s", c);
    }
    if (cc != '\0') {
      printf("%c", cc);
    }
  }
}

char *draw_circle(int width, int height, char interior, float thickness) {
  int size = (width + 1) * (height + 1) + 1;
  char *dst = (char *)malloc(size);
  int index = 0;
  int centerX = width / 2, centerY = height / 2;

  for (int y = 0; y <= height; y++) {
    for (int x = 0; x <= width; x++) {
      float dx = (float)(x - centerX) / (width / 2);
      float dy = (float)(y - centerY) / (height / 4);
      float distance = (dx * dx) + (dy * dy);

      if (distance >= (1.0f - thickness) && distance <= (1.0f + thickness))
        dst[index++] = interior;
      else
        dst[index++] = ' ';
    }
    dst[index++] = '\n';
  }
  dst[index] = '\0';
  return dst;
}

int contains(Loot_Type *array, int length, Loot_Type value) {
  for (int i = 0; i < length; i++) {
    if (array[i] == value) {
      return 1;
    }
  }
  return 0;
}