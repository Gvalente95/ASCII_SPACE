
#ifndef PROTO_H
#define PROTO_H
#include "includes.h"

typedef enum { normal, on_grid, watch_up, cutscene, free_slide, mv_count } MOVE_TYPE;
typedef enum { press, autom, disabled, atk_type_count } ATK_TYPE;
typedef enum { entrance, idle, attack0, attack, attack2, SP_attack, SP_attack2, reposition, regen, hurt, shield, retreat, call, die, null, BOSS_ACTIONS_COUNT } BOSS_ACTIONS;
typedef enum { MOR_SC, MOR_HP, MOR_PRJ, MOR_SHLD, MOR_ATTR, BET_PRJ, MOR_LOOT, LOOT_TYPE_COUNT } Loot_Type;
typedef enum { up, down, right, left, up_left, up_right, down_left, down_right, none, vert, ver_left, ver_right, hor, hor_up, hor_down, all, DIR_COUNT } DIR;
typedef enum { HANGER, FACTORY, OBSERVATORY, BRIDGE, FORTRESS, DOME, PYRAMID, BUNKER, BLD_TYPE_COUNT } BLD_TYPE;
typedef enum { base, magnet, bomb, big, prj_count } PRJ_TYPES;

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

typedef struct {
  int x_pos, y_pos, col_timer;
  DIR dir;
  PRJ_TYPES type;
} PROJ;

typedef struct {
  int x_pos, y_pos, z_pos;
} STAR;

typedef struct {
  DIR allowed_dir, allowed_lk_dir;
  MOVE_TYPE mv_type;
  ATK_TYPE atk_type;
  PRJ_TYPES cur_proj;
  int prj_qual, attr, shots_fired, level;
  long int score, max_score;
  int loot_chance, lv_choices;
  int is_in_boss_fight;
  int gl_x_pos, gl_y_pos;
  int plr_x_centr, plr_y_centr, prj_col_index, plr_col_index;
  int boss_mode, is_framing;
  int input, last_input;
  int shift_on;
} GAME;

typedef struct {
  char *content;
  int cur_hp, prev_HP, max_HP, dmg, shield, speed, atk_spd, max_shield, prv_shield;
  int x_pos, y_pos, height, width, x_dir, y_dir;
  int time_in_state, speATK_hole, hole_xdir, hole_size, shld_ic_count;
  PROJ *proj;
  BOSS_ACTIONS action;
  BOSS_ACTIONS next_action;
} BOSS;

#define true 1;
#define false 0;
typedef int bool;

int main_loop();

// INIT
void initialize_game(GAME *game, SHIP *plr, LOOT *loots, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars, BLDING *building);
void init_boss(GAME *game, BOSS *boss);
PROJ Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos);
char *init_blank_canv(int w, int h, int has_ext, char interior);

// updS
int set_input(GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mob_prj, int *has_moved, int time, int *show_grid, int *god_mode);
void upd_game_state(char *canv, GAME *game, SHIP *plr, BOSS *boss, SHIP *mobs, PROJ *proj, PROJ *mobprj, LOOT *loots, STAR *stars, int has_moved, int time, int *wind);
int upd_player(GAME *game, SHIP *plr, PROJ *proj, int has_moved, int time);
int upd_background(char *canv, STAR *stars, GAME game, BOSS boss, SHIP plr, int has_moved, int time);
int upd_boss_fight(char *canv, GAME *game, BOSS *boss, SHIP *mobs, PROJ *mobprj, LOOT *loots, SHIP *plr, int time);
int upd_loots(char *canv, GAME *game, SHIP *plr, LOOT *loots, int time);
int upd_mob_pos(char *canv, GAME *game, BOSS boss, SHIP *mobs, PROJ *mobprj, LOOT *loots, SHIP *plr, int time);
int upd_plr_proj(char *canv, GAME *game, PROJ *proj, SHIP *mobs, BOSS *boss, SHIP *plr, int time);
int upd_mob_prj(char *canv, GAME *game, PROJ *mobprj, SHIP *plr, int time);
int upd_wind(char *canv, int *wind, GAME *game, SHIP *shp, BOSS *boss);

// BOSS FIGHT
void CHANGE_ACTION(GAME *game, BOSS *boss, SHIP *plr, int hp_perc);
void IDLE(GAME *game, BOSS *boss, SHIP *plr, int time);
void ATK0(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time);
void ATK1(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time, int hp_perc);
void ATK2(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time, int hp_perc);
void SP_ATTACK(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time);
void SP_ATK2(GAME *game, BOSS *boss, SHIP *plr, int time);
void CALL(GAME *game, BOSS *boss, SHIP *plr, SHIP *mobs, int time);
void HURT(GAME *game, BOSS *boss, SHIP *plr, int time);
void SHIELD(GAME *game, BOSS *boss, SHIP *plr);
void RETREAT(char *canv, GAME *game, BOSS *boss, SHIP *plr, int time);
int HANDLE_HEALTH(BOSS *boss, int hp_perc);
void DEATH(GAME *game, BOSS *boss, PROJ *mobprj);
void REPOSITION(GAME *game, BOSS *boss, SHIP *plr, int time);
void ENTRANCE(GAME *game, BOSS *boss, SHIP *plr, int time);
void RENDER_BOSS(BOSS *boss, char *boss_content, char *canv);
void HANDLE_CONSTANTS(GAME *game, BOSS *boss, SHIP *plr, PROJ *mobprj, int time, int hp_perc);

// CANV
void render_game(GAME game, BOSS boss, SHIP *plr, SHIP *mobs, PROJ *proj, PROJ *mobprj, int *wind, char *canv);
int render_canvas(char *canv, GAME *game, int time);
int set_lotterie_menu(char *header, char *canv, GAME *game, SHIP *plr);
char *set_new_width(char *src, int new_width);
int write_on_canv(char *src, char *dst, int x_pos, int y_pos);
char *get_triangle(char interior, DIR dir, int h, int w, int color);
char *get_circle(int width, int height, char interior, float thickness);
int set_over_canv(char *canv, int left_padding, int top_padding, const char *format, ...);
void set_label(char *canv, int *start_index, const char *label, int value);
int set_text_red(char *src);
void render_text_in_color(char *c, char __, char *color);
int is_in_canv(int x_pos, int y_pos, int width, int height, DIR direciton);
char *get_formatted_text(const char *format, ...);

// TOOLS
void set_nonblocking_mode(int enable);
int get_width(char *src);
int get_height(char *src);
int crop_x(char *src, int from_x);
int rand_range(int min, int max);
int is_pos_free(SHIP *mobs, int current_mob, int x, int y);
int contains(Loot_Type *array, int length, Loot_Type value);
int check_overlap(BLDING *a, BLDING *b);
int is_dir_allowed(DIR dir, DIR allowed_directions);
DIR get_prj_dir(int w_prj, int max_prj, SHIP plr, DIR d);
int set_plr_death(char *canv, GAME game, SHIP plr);
int is_in_string(char c, char *str);

// DEBUG
int show_header(char *canv, char *header, GAME game, SHIP plr, SHIP *mobs, BOSS boss, int time, int has_moved, int *plr_shwcl_tm, int *prj_shwcl_tm);
int show_grid_view(char *canv, GAME game, char show_grid);

#endif