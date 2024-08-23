#ifndef HEADER_H
#define HEADER_H

#include "ascii.h"
#include <fcntl.h>
#include <stdarg.h>
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
#define PROJ_BUFFER 200
#define PRJ_MOB_BUFFER 1000
#define LOOT_BUFFER 50
#define STAR_BUFFER 5
#define BLDING_BUFFER 0
#define WIND_BUFFER 60
#define PLR_WIND_MIN 0
#define PLR_WIND_MAX (WIND_BUFFER / 2)
#define BOSS_WIND_MIN ((WIND_BUFFER / 2) + 1)
#define BOSS_WIND_MAX WIND_BUFFER

//  CANVAS
#define CELL_AM 8
#define CANV_W 120
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
#define PLR_CENTER_X ((CANV_W + 1) / 2) - (SHIP_W / 2)
#define PLR_CENTER_Y (CANV_H - 10)

#define BOSS_CENTER_Y 9

#define CENTER_X(object_width) ((CANV_W / 2) - (object_width) / 2)
#define CENTER_Y(object_height) ((CANV_H / 2) - (object_height) / 2)
#define MOVE_TOWARD_CENTER_X(pos, object_width) (pos += (pos < CENTER_X(object_width)) ? 1 : (pos > CENTER_X(object_width)) ? -1 : 0)
#define MOVE_TOWARD_CENTER_Y(pos, object_height) (pos += (pos < CENTER_Y(object_height)) ? 1 : (pos > CENTER_Y(object_height)) ? -1 : 0)
#define PERCENTILE(value, max) (((value) * 100) / (max))

// PLAYER
#define BASE_LOOT_CHNC 30
#define PLR_MAX_HP 5
#define HURT_DUR 60
#define shot_lock 50
#define STRT_LOOT_CHNC 10

// MOBS
#define BOSS_INTERVAL 500
#define MOB_AM 10
#define MOB_MAX_HP 1
// ICONS

#define SAVEFILE "savefile.txt"

#define SCR_SMB "★"
#define PROJ_SMB "⚡"
#define SHLD_SMB "⛨"
#define ATTR_SYMB "⚙"
#define LUCK_SMB "$"
#define HEART_SMB "❤️"

#define SHLD_OFF_MOB_IC ';'
#define SHLD_ON_MOB_IC '&'

#define STAR_IC '@'
#define STAR_IC2 '^'
#define STAR_SMB "✧"

#define RED_ICON '\xA0'
#define COIN_IC '\xA1'   // ¤
#define WEAP_IC '\xA2'   // Æ
#define SHIELD_IC '\xA3' // ×
#define HEART_IC '\xA4'  // §
#define ATTR_IC '\xA5'   // «
#define LUCK_IC '\xA6'   // ¶
#define STARS_IC '\xA7'  // ¨

#define COL_HOR '\xB1' // Í
#define COL_VER '\xB2' // º

#define CANV_HOR '\xC1'       // Ä
#define CANV_VER '\xC2'       // ³
#define PLR_SHLD_OUTL '\xC3'  // Û
#define PLR_SHLD_OUTL1 '\xC4' // Ü
#define PLR_SHLD_OUTL2 '\xC5' // Ý

#define MOB_LBORD '\xD1' // É
#define MOB_RBORD '\xD2' // »
#define MOB_HOR '\xD3'   // Í
#define MOB_VER '\xD4'   // º

#define SHP_LBORD '\xE1' // Ù
#define SHP_RBORD '\xE2' // À
#define SHP_HOR '\xE3'   // Á
#define SHP_VER '\xE4'   // ´

#define EXPL_START '\xF1' // à
#define PRJ_COL '\xF2'    // à

#define WIND_VER '\xF3' // Þ
#define WIND_HOR '\xF4' // ß
#define WIND_DOT '\xF5' // ú

#define PRJ_VER '\xCA'    // ±
#define PRJ_HOR '\xCB'    // ÷
#define PRJ_DIAG_L '\xCC' // ¼
#define PRJ_DIAG_R '\xCD' // ½

#define MPRJ_VER '\xBA'    // ±
#define MPRJ_HOR '\xBB'    // ÷
#define MPRJ_DIAG_L '\xBC' // ¼
#define MPRJ_DIAG_R '\xBD' // ½

typedef enum { normal, on_grid, watch_up, cutscene, free_slide, mv_count } MOVE_TYPE;
typedef enum { press, autom, disabled, atk_type_count } ATK_TYPE;
typedef enum { entrance, idle, attack, SP_attack, SP_attack2, reposition, regen, hurt, shield, retreat, call, die, null, BOSS_ACTIONS_COUNT } BOSS_ACTIONS;
typedef enum { MOR_SC, MOR_HP, MOR_PRJ, MOR_SHLD, MOR_ATTR, BET_PRJ, MOR_LOOT, LOOT_TYPE_COUNT } Loot_Type;
typedef enum { up, down, right, left, up_left, up_right, down_left, down_right, none, vert, hor, all, DIR_COUNT } DIR;
typedef enum { HANGER, FACTORY, OBSERVATORY, BRIDGE, FORTRESS, DOME, PYRAMID, BUNKER, BLD_TYPE_COUNT } BLD_TYPE;

typedef struct {
  int hp, shield, weap, speed, atk_speed, atk_pow, atk_am;
  int hurt_timer, atk_reload;
  int x_pos, y_pos;
  DIR dir;
  DIR lk_dir;
} SHIP;

typedef struct {
  Loot_Type type;
  char content;
  int x_pos, y_pos;
} LOOT;

typedef struct {
  BLD_TYPE type;
  int x_size, y_size;
  int x_pos, y_pos;
  char *content;
} BLDING;
#define BLDING_MAX_SIZE 50
#define BLDING_MIN_SIZE 10

typedef struct {
  int x_pos, y_pos, col_timer;
  DIR dir;
} PROJ;

typedef struct {
  int x_pos, y_pos, z_pos;
} STAR;

typedef struct {
  int prj_qual;
  int attr;
  int shots_fired;
  int level;
  long int score, max_score;
  int loot_chance, lv_choices;
  int is_in_boss_fight;
  int gl_x_pos;
  int gl_y_pos;
  int plr_x_centr, plr_y_centr, prj_col_index, plr_col_index;
  int boss_mode;
  DIR allowed_dir;
  MOVE_TYPE mv_type;
  ATK_TYPE atk_type;
} GAME;

typedef struct {
  char *content;
  int cur_hp, prev_HP, max_HP, dmg, shield, speed, atk_spd, max_shield, prv_shield;
  int x_pos, y_pos, height, width, x_dir, y_dir;
  int time_in_state, speATK_hole, hole_xdir, hole_size, shld_ic_count;
  PROJ *proj;
  BOSS_ACTIONS action;
} BOSS;

void set_nonblocking_mode(int enable);
char *draw_triangle(char interior, DIR dir, int h, int w, int color);
char *init_blank_canv(int w, int h, int has_ext, char interior);
int get_width(char *src);
int get_height(char *src);
int crop_x(char *src, int from_x);
char *erase_on_text(char *src, int new_width);
int add_to_canv(char *src, char *dst, int x_pos, int y_pos);
int rand_range(int min, int max);
int is_position_o__upied(SHIP *mobs, int current_mob, int x, int y);
void print_colored_text(char *c, char __, char *color);
char *draw_circle(int width, int height, char interior, float thickness);
int contains(Loot_Type *array, int length, Loot_Type value);
int write_on_canv(char *canv, int left_padding, int top_padding, const char *format, ...);
int check_overlap(BLDING *a, BLDING *b);
void write_label_and_value(char *canv, int *start_index, const char *label, int value);
int is_in_canv(int x_pos, int y_pos, int width, int height, DIR direciton);
int is_dir_allowed(DIR dir, DIR allowed_directions);
int print_canvas(char *canv, GAME *game);
int print_in_red(char *src);
#endif