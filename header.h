#ifndef HEADER_H
#define HEADER_H

#include "ascii.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// COLORS
#define RED "\033[31m"
#define BRIGHT_RED "\033[91m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define LIGHT_BLUE "\033[38;5;117m"
#define DIM_WHITE "\033[38;5;250m"
#define PALE_YELLOW "\033[38;5;229m"
#define SOFT_BLUE "\033[38;5;153m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define ORANGE "\033[38;5;214m"
#define BROWN "\033[38;5;94m"
#define DARK_GREEN "\033[38;5;22m"
#define FULL_BLOCK "\u25A0"
#define BLACK_SQUARE "\u25FC"
#define WHITE_SQUARE "\u25FB"
#define LIGHT_SHADE "\u2591"
#define MEDIUM_SHADE "\u2592"
#define DARK_SHADE "\u2593"

//  BUFFERS
#define PROJ_BUFFER 20
#define PRJ_MOB_BUFFER 50
#define LOOT_BUFFER 2
#define STAR_BUFFER 50
#define WIND_BUFFER 20

//  CANVAS
#define stars_AM 20
#define CELL_AM 8
#define CANV_W 100
#define UPG_CANV_W (CANV_W - (CANV_W / 6))
#define UPG_CANV_H UPG_CANV_W / 2

#define CANV_H (CANV_W / 2)
#define CANV_SIZE (CANV_W + (CANV_H * CANV_W))
#define CELL_SIZE (CANV_W / CELL_AM)
#define C_WIDTH (CANV_W / CELL_SIZE)
#define C_HEIGHT (CANV_H / CELL_SIZE)
#define SHIP_W (C_HEIGHT - 2)
#define SHIP_H (C_WIDTH - 2)
#define SHOW_GRID 0
#define WIND_AM 50

// PLAYER
#define PLR_MAX_HP 5
#define HURT_DUR 25
#define shot_lock 50
#define STRT_LOOT_CHNC 10

// MOBS
#define MOB_AM 10
#define MOB_MAX_HP 1
// ICONS

#define SAVEFILE "savefile.txt"

#define HEART_CODE "\u2764"

#define COIN_CODE "\u2666"
#define WEAP_CODE "\u2022"
#define SHIELD_CODE "\u26E8"
#define HEALTH_CODE "\u2764"
#define ATTR_CODE "\u2691"

#define COIN_IC '='
#define WEAP_IC '!'
#define SHIELD_IC '?'
#define HEART_IC '>'
#define ATTR_IC '<'
#define MORLOOT_IC ':'

#define COL_HOR 'c'
#define COL_VER 'l'

#define CANV_HOR '$'
#define CANV_VER '@'
#define PLR_SHLD_OUTL '('
#define PLR_SHLD_OUTL1 '{'
#define PLR_SHLD_OUTL2 '['

#define STARS_IC '*'

#define MOB_LBORD 'm'
#define MOB_RBORD 'M'
#define MOB_HOR 'x'
#define MOB_VER 'X'

#define SHP_LBORD '&'
#define SHP_RBORD '%'
#define SHP_HOR '~'
#define SHP_VER '\''

#define EXPL_START 'b'
#define WIND_VER 'V'
#define WIND_HOR 'h'
#define WIND_DOT 'i'

#define PRJ_VER 'z'
#define PRJ_HOR 'Z'
#define PRJ_DIAG_L 'y'
#define PRJ_DIAG_R 'Y'

#define LOOT_COUNT 7
typedef enum { MOR_SC, MOR_HP, MOR_PRJ, MOR_SHLD, MOR_ATTR, BET_PRJ, MOR_LOOT } Loot_Type;
typedef enum { up, down, right, left, none, up_left, up_right, down_left, down_right } DIR;

typedef struct {
  int hp;
  int hurt_timer;
  int x_pos;
  int y_pos;
  int height;
  int weight;
  int shield;
  int weap;
  DIR dir;
} SHIP;

typedef struct {
  Loot_Type type;
  char content;
  int x_pos;
  int y_pos;
} LOOT;

typedef struct {
  int x_pos;
  int y_pos;
  DIR dir;
} PROJ;

typedef struct {
  int prj_qual;
  int attr;
  int shots_fired;
  int level;
  long int score;
  long int max_score;
  int loot_chance;
  int lv_choices;
} GAME;

void set_nonblocking_mode(int enable);
char *draw_triangle(char interior, DIR dir, int h, int w, int color);
char *init_blank_canv(int w, int h, int has_ext, char interior);
int get_width(char *src);
int get_height(char *src);
int crop_x(char *src, int from_x);
char *erase_on_text(char *src, int new_width);
int print_on_text(char *src, char *dst, int x_pos, int y_pos);
int rand_range(int min, int max);
int is_position_occupied(SHIP *mobs, int current_mob, int x, int y);
void print_colored_text(char *c, char cc, char *color);
char *draw_circle(int width, int height, char interior, float thickness);
int contains(Loot_Type *array, int length, Loot_Type value);
#endif