
#ifndef PROTO_H
#define PROTO_H
#include "includes.h"

typedef enum { normal, on_grid, watch_up, cutscene, free_slide, mv_count } MOVE_TYPE;
typedef enum { press, autom, disabled, atk_type_count } ATK_TYPE;
typedef enum { entrance, idle, attack0, attack, attack2, SP_attack, SP_attack2, reposition, regen, hurt, shield, retreat, call, die, null, BOSS_ACTIONS_COUNT } BOSS_ACTIONS;
typedef enum { MOR_SC, MOR_HP, MOR_PRJ, MOR_SHLD, MOR_ATTR, BET_PRJ, MOR_ITEM, MAP, MOR_SPD, MOR_PWR, MOR_XP, MOR_GLD, TELEPORT, AOE, FREEZE, CRAFT_MAT, WEAPON, ARMOR, ITEM_TYPE_COUNT } ITEM_Type;

typedef enum { nm, paused, shopping, boss_fight, dialog, GAME_STATE_COUNT } GAME_STATE;
typedef enum { up, down, right, left, up_left, up_right, down_left, down_right, none, vert, ver_left, ver_right, hor, hor_up, hor_down, all, DIR_COUNT } DIR;
typedef enum { SHOP, ARMORY, HOUSE, FORTRESS, NONE, BLD_TYPE_COUNT } BLD_TYPE;
typedef enum { base, magnet, bomb, big, prj_count, PRJ_TYPES_COUNT } PRJ_TYPES;
typedef enum { SELLER, SHOPPER, QUEST_GIVER, EXPLORER, NPC_TYPES_COUNT } NPC_TYPES;
typedef enum { red, blue, green, yellow, COLOR_COUNT } COLOR;
typedef enum { COMMON, UNCOMMON, RARE, MYSTIC, ARTIFACT, DIVINE, DIABOLICAL, RARITY_COUNT } RARITY;

typedef struct {
  char name[50], content, desc[256];
  RARITY rar;
  int price, val_inc, dur;
  int x_pos, y_pos;
  ITEM_Type type;
} ITEM;

typedef struct {
  char *name, *desc;
  int requested_am;
  int current_am;
  int quest_succedeed;
} QUEST;

typedef struct {
  int hp, shield, weap, speed, atk_speed, atk_pow, atk_am;
  int hurt_timer, atk_reload, is_blocked;
  int x_pos, y_pos;
  DIR dir;
  DIR lk_dir;
} SHIP;

typedef struct {
  NPC_TYPES type;
  DIR dir, lk_dir;
  int x_p, y_p, floor;
  char *lft_bdy, *rgt_bdy, *dwn_bdy, *up_bdy;
  char *text;
  ITEM *items;
  QUEST quest;
} NPC;

typedef struct {
  BLD_TYPE type;
  int x_size, y_size, x_s_ext, y_s_ext;
  int x_pos, y_pos;
  int npc_am, flr_am;
  char *ext_cont;
  NPC *npcs;
  char **floors;
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
  ITEM *itm_ownd, *itm_list;
  MOVE_TYPE mv_type;
  ATK_TYPE atk_type;
  PRJ_TYPES cur_proj;
  BLD_TYPE cur_blding;
  GAME_STATE g_st;
  int prj_qual, attr, shots_fired, level;
  long int score, max_score;
  int ITEM_chance, lv_choices;
  int gl_x_pos, gl_y_pos;
  int plr_x_centr, plr_y_centr, prj_col_index, plr_col_index;
  int boss_mode, is_framing;
  int input, last_input;
  int shift_on;
  int seller;
  int num_items;
  NPC *cur_seller;
  int cur_bld_index, is_in_dialog, cur_floor;
  char *minimap;
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
void initialize_game(GAME *game, SHIP *plr, ITEM *ITEMs, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars, BLDING *building);
void init_boss(GAME *game, BOSS *boss);
PROJ Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos);
char *init_blank_canv(int w, int h, int has_ext, char interior);
void init_new_buildings(GAME *game, BLD_TYPE type, BLDING *bld, int x, int y);

// updS
int set_input(char *canv, GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mob_prj, int *has_moved, int time, int *show_grid, int *god_mode);
void upd_game_state(char *canv, GAME *game, SHIP *plr, BOSS *boss, SHIP *mobs, PROJ *proj, PROJ *mobprj, ITEM *ITEMs, BLDING *bldings, STAR *stars, int has_moved, int time, int *wind);
int upd_player(char *canv, GAME *game, SHIP *plr, PROJ *proj, int has_moved, int time);
int upd_background(char *canv, STAR *stars, GAME game, BOSS boss, SHIP plr, int has_moved, int time);
int upd_boss_fight(char *canv, GAME *game, BOSS *boss, SHIP *mobs, PROJ *mobprj, ITEM *ITEMs, SHIP *plr, int time);
int upd_ITEMs(char *canv, GAME *game, SHIP *plr, ITEM *ITEMs, int time, int reset);
int upd_mob_pos(char *canv, GAME *game, BOSS boss, SHIP *mobs, PROJ *mobprj, ITEM *ITEMs, SHIP *plr, int time, int reset);
int upd_plr_proj(char *canv, GAME *game, PROJ *proj, SHIP *mobs, BOSS *boss, SHIP *plr, int time, int reset);
int upd_mob_prj(char *canv, GAME *game, PROJ *mobprj, SHIP *plr, int time, int reset);
int upd_wind(char *canv, int *wind, GAME *game, SHIP *shp, BOSS *boss);
int upd_buildings(char *canv, BLDING *buildings, GAME *game, SHIP *plr, int time, int *has_moved);
int upd_collisions(char *canv, GAME *game, PROJ *proj, BOSS *boss, SHIP *mobs, PROJ *mobprj, SHIP *plr);
int handle_seller_menu(char *canv, GAME *game, NPC *Seller, BLDING *buildings, SHIP *plr, SHIP *mobs, BOSS *boss, PROJ *mobprj);

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
void render_game(GAME *game, BOSS boss, SHIP *plr, SHIP *mobs, BLDING *bldings, PROJ *proj, PROJ *mobprj, int *wind, char *canv, int time, int has_moved);
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
int is_in_canv(int x_pos, int y_pos, int width, int height, int canv_x, int canv_y, int canv_w, int canv_h, DIR d);
char *get_formatted_text(const char *format, ...);

// TOOLS
void set_nonblocking_mode(int enable);
int get_width(char *src);
int get_height(char *src);
int crop_x(char *src, int from_x);
int rand_range(int min, int max);
int is_pos_free(SHIP *mobs, int current_mob, int x, int y);
int is_pos_free_BUILDINGS(BLDING *bldings, int current_mob, int x, int y);
int contains(ITEM_Type *array, int length, ITEM_Type value);
int check_overlap(BLDING *a, BLDING *b);
int is_dir_allowed(DIR dir, DIR allowed_directions);
DIR get_prj_dir(int w_prj, int max_prj, SHIP plr, DIR d);
DIR get_dir_to_target(int target_x, int target_y, int x_pos, int y_pos, int has_diag);
int set_plr_death(char *canv, GAME game, SHIP plr);
int is_in_string(char c, char *str);
char get_at_index(char *canv, int c_width, int c_height, int x, int y);
int color_text(COLOR col, char *text, int fill_interior);
void tile_canvas(char *canv, int width, int tile_size);
void Copy_Item(ITEM *to, ITEM from);
int Contains_item(ITEM a, ITEM *items, int size);
void Generate_items(ITEM *list, NPC *n, int amount, int list_size);

// DEBUG
int show_header(char *canv, char *header, GAME *game, SHIP plr, SHIP *mobs, BLDING *bldng, BOSS boss, int time, int has_moved, int *plr_shwcl_tm, int *prj_shwcl_tm);
int show_grid_view(char *canv, GAME game, char show_grid);

// ITEM_PARSER
ITEM *read_items_from_file(const char *filename, int *num_items);
#endif