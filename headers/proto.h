
#ifndef PROTO_H
#define PROTO_H
#include "includes.h"

typedef enum { normal, on_grid, watch_up, cutscene, free_slide, mv_count } MOVE_TYPE;
typedef enum { press, autom, disabled, atk_type_count } ATK_TYPE;
typedef enum { entrance, idle, attack0, attack, attack2, SP_attack, SP_attack2, reposition, regen, hurt, shield, retreat, call, die, null, BOSS_ACTIONS_COUNT } BOSS_ACTIONS;
typedef enum { MAP, MATERIAL, WEAPON, ARMOR, EXPANDABLE, ITEM_Type_COUNT } ITEM_Type;
typedef enum { SCORE, HP, PRJ, SHLD, MAGNET, LUCK, SPEED, POWER, WISE, GOLD, Bonus_COUNT } Bonus_Type;
typedef enum { TELEPORT, SHOW_RADAR, AOE, HURT_MOB, HURT_PLR, HEAL_PLR, FREEZE, Effect_COUNT } Effect_Type;
typedef enum { SELF, CLOSEMOB, SELECTED_MOB, ALLMOBS, Taget_COUNT } Target_Type;
typedef enum { COMMON, UNCOMMON, RARE, ARTIFACT, DIVINE, MYSTIC, DIABOLICAL, RARITY_COUNT } RARITY;

typedef enum { nm, paused, shopping, boss_fight, dialog, GAME_STATE_COUNT } GAME_STATE;
typedef enum { up, down, right, left, up_left, up_right, down_left, down_right, none, vert, ver_left, ver_right, hor, hor_up, hor_down, all, circular, DIR_COUNT } DIR;
typedef enum { SHOP, ARMORY, HOUSE, FORTRESS, NONE, BLD_TYPE_COUNT } BLD_TYPE;
typedef enum { base, magnet, bomb, big, ricochet, PRJ_TYPES_COUNT } PRJ_TYPES;
typedef enum { SELLER, SHOPPER, QUEST_GIVER, EXPLORER, NPC_TYPES_COUNT } NPC_TYPES;
typedef enum { green, blue, yellow, orange, red, purple, COLOR_COUNT } COLOR;
typedef enum { CharCoins, Stellaris, Quantums, CosmoCoins, BitCredits, CURRENCY_COUNT } CURRENCY_TYPES;

typedef struct {
  int value, dur;
  Target_Type target;
  char name[50];
} BONUS;

typedef struct {
  int value, dur;
  Target_Type target;
  char name[50];
} EFFECT;

typedef struct {
  char name[50], desc[256], content;
  int price, dur, am;
  int x_pos, y_pos, center_x, center_y;
  RARITY rar;
  ITEM_Type type;
  BONUS bonus[10];
  EFFECT effect[10];
} ITEM;

typedef struct {
  char *name, *desc;
  int requested_am;
  int current_am;
  int quest_succedeed;
} QUEST;

typedef struct {
  int hp, maxHP, spd, maxSpd, shield, weap, atk_spd, atk_pow, atk_am;
  int death_timer, hurt_timer, atk_reload, is_blocked;
  int x_pos, y_pos, width, height;
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
  float angular_velocity;
  int x_pos, y_pos, col_timer, angular_offset;
  int prv_index, dur;
  DIR dir;
  PRJ_TYPES type;
} PROJ;

typedef struct {
  int x_pos, y_pos, z_pos;
} STAR;

typedef struct {
  int amount;
  CURRENCY_TYPES type;
} CURRENCY;

typedef struct {
  DIR allowed_dir, allowed_lk_dir;
  ITEM *itm_ownd, *itm_list;
  MOVE_TYPE mv_type;
  ATK_TYPE atk_type;
  PRJ_TYPES cur_proj;
  BLD_TYPE cur_blding;
  GAME_STATE g_st;
  NPC *cur_seller;
  char *minimap;
  long int score, max_score;
  int prj_qual, attr, shots_fired, level;
  float luck;
  int lv_choices;
  int gl_x_pos, gl_y_pos;
  int plr_x_centr, plr_y_centr, prj_col_index, plr_col_index;
  int boss_mode, is_framing;
  int input, last_input, shift_on;
  int seller, num_items;
  int owned_amnt;
  int inv_incrmnt;
  int cur_bld_index, is_in_dialog, cur_floor;
  CURRENCY currencies[CURRENCY_COUNT - 1];
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
void initialize_game(GAME *game, SHIP *plr, ITEM *items, SHIP *mobs, PROJ *proj, PROJ *mobprj, STAR *stars, BLDING *building);
void init_boss(GAME *game, BOSS *boss);
int Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos);
char *init_blank_canv(int w, int h, int has_ext, char interior);
void init_new_buildings(GAME *game, BLD_TYPE type, BLDING *bld, int x, int y);

// updS
int set_input(char *canv, GAME *game, BOSS boss, SHIP *plr, PROJ *proj, PROJ *mob_prj, int *has_moved, int time, int *show_grid, int *god_mode);
void upd_game_state(char *canv, GAME *game, SHIP *plr, BOSS *boss, SHIP *mobs, PROJ *proj, PROJ *mobprj, ITEM *items, BLDING *bldings, STAR *stars, int has_moved, int time, int *wind);
int upd_player(char *canv, GAME *game, SHIP *plr, PROJ *proj, int has_moved, int time);
int upd_background(char *canv, STAR *stars, GAME game, BOSS boss, SHIP plr, int has_moved, int time);
int upd_boss_fight(char *canv, GAME *game, BOSS *boss, SHIP *mobs, PROJ *mobprj, ITEM *items, SHIP *plr, int time);
int upd_items(char *canv, GAME *game, SHIP *plr, ITEM *items, int time, int reset);
int upd_mob_pos(char *canv, GAME *game, BOSS boss, SHIP *mobs, PROJ *mobprj, ITEM *items, SHIP *plr, int time, int reset);
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
char *set_new_width(char *src, int new_width);
char *write_on_canv(char *src, char *dst, int x_pos, int y_pos);
char *get_triangle(char interior, DIR dir, int h, int w, int color);
char *get_circle(int width, int height, char interior, float thickness);
int set_over_canv(char *canv, int left_padding, int top_padding, const char *format, ...);
void set_label(char *canv, int *start_index, const char *label, int value);
int set_text_red(char *src);
void render_text_in_color(char *c, char __, char *color);
int is_in_canv(int x_pos, int y_pos, int width, int height, int canv_x, int canv_y, int canv_w, int canv_h, DIR d);
char *get_formatted_text(const char *format, ...);
char *swap_line(char *txt, int i, DIR d);
char *mirror_txt(char *txt, int x_center, DIR d);
char *get_text_bubble(char *txt);

// TOOLS
void set_nonblocking_mode(int enable);
int get_index(char *canv, int x_pos, int y_pos);
int get_width(const char *src);
int get_height(const char *src);
char get_index_value(char *canv, int c_width, int c_height, int x, int y);
void reset_item(ITEM *itm);
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
int color_text(COLOR col, char *text, int fill_interior);
void Color_from_index(COLOR col, char **text);
void tile_canvas(char *canv, int width, int tile_size);
void Copy_Item(ITEM *to, ITEM from);
int Get_duplicate_index(ITEM a, ITEM *items, int size);
void Generate_items(ITEM *list, NPC *n, int amount, int list_size);
ITEM get_item(RARITY rar, ITEM_Type type, ITEM *items, int buffer_size);
int determineRarity(float luck);
void reorder_list(ITEM *list, int max_size);

// DEBUG
int show_header(char *canv, char *header, GAME *game, SHIP plr, SHIP *mobs, BLDING *bldng, BOSS boss, int time, int has_moved, int *plr_shwcl_tm, int *prj_shwcl_tm);
int show_grid_view(char *canv, GAME game, char show_grid);
int show_arrays_content(GAME game, PROJ *mobprj, PROJ *plrPRJ, SHIP *mobs, BLDING *bldings, ITEM *in_buff_itms, ITEM *in_bag_itms);
void handle_mini_map(char *canv, GAME *game, SHIP plr, SHIP *mobs, BLDING *bldng, BOSS boss, ITEM *items, int time);

// ITEM_PARSER
ITEM *read_items_from_file(const char *filename, int *num_items);

// AUDIO
void stop_sound(pid_t pid);
pid_t play_sound(const char *filename, int loop);

#endif