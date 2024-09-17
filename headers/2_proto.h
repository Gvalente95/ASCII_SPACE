
#ifndef PROTO_H
#define PROTO_H

#include "1_includes.h"

typedef enum { normal, on_grid, watch_up, cutscene, free_slide, mv_count } MOVE_TYPE;

typedef enum { press, autom, disabled, atk_type_count } ATK_TYPE;

typedef enum { entrance, idle, attack0, attack, attack2, SP_attack, SP_attack2, reposition, regen, hurt, shield, retreat, call, die, null, BOSS_ACTIONS_COUNT } BOSS_ACTIONS;
typedef enum { FETCH, INVESTIGATION, DELIVERY, EXTERMINATION, NEGOTIATION, EXPLORATION, ANY_QUEST, QUESTS_TYPE_COUNT } QUESTS_TYPES;
typedef enum { MAP, MATERIAL, WEAPON, ARMOR, EXPANDABLE, SPACESHIP, QUEST_ITEM, ITEM_Type_COUNT } ITEM_Type;
typedef enum { SCORE, HP, PRJ, SHLD, MAGNET, LUCK, SPEED, POWER, WISE, GOLD, Bonus_COUNT } Bonus_Type;
typedef enum { TELEPORT, SHOW_RADAR, AOE, HURT_MOB, HURT_PLR, HEAL_PLR, FREEZE, Effect_COUNT } Effect_Type;
typedef enum { SELF, CLOSEMOB, SELECTED_MOB, ALLMOBS, Taget_COUNT } Target_Type;
typedef enum { COMMON, UNCOMMON, RARE, ARTIFACT, DIVINE, MYSTIC, DIABOLICAL, RARITY_COUNT } RARITY;

typedef enum { nm, paused, shopping, boss_fight, dialog, map_menu, GAME_STATE_COUNT } GAME_STATE;
typedef enum { up, down, right, left, up_left, up_right, down_left, down_right, none, vert, ver_left, ver_right, hor, hor_up, hor_down, all, circular, DIR_COUNT } DIR;
typedef enum { SHOP, GLB_armorY, HOUSE, FORTRESS, NONE, BLD_TYPE_COUNT } BLD_TYPE;
typedef enum { base, magnet, bomb, big, ricochet, PRJ_TYPES_COUNT } PRJ_TYPES;
typedef enum { seller, SHOPPER, QUEST_GIVER, EXPLORER, NPC_TYPES_COUNT } NPC_TYPES;
typedef enum { green, blue, yellow, orange, red, purple, COLOR_COUNT } COLOR;
typedef enum { CharCoins, Stellaris, Quantums, CosmoCoins, BitCredits, CURRENCY_COUNT } CURRENCY_TYPES;
typedef enum { NEW, PAUSED, IN_PROGRESS, COMPLETE, QUEST_STATUS_COUNT } QUEST_STATUS;

typedef struct NPC NPC;
typedef struct QUEST QUEST;

typedef struct {
  Target_Type target;
  char name[50];
  int value, dur;
} BONUS;

typedef struct {
  Target_Type target;
  char name[50];
  int value, dur;
} EFFECT;

typedef struct {
  ITEM_Type type;
  RARITY rar;
  BONUS bonus[10];
  EFFECT effect[10];
  char name[50], desc[256], content;
  int price, dur, am;
  int x_pos, y_pos, center_x, center_y;
} ITEM;

struct QUEST {
  NPC *giver, *recever;
  QUESTS_TYPES q_type;
  QUEST_STATUS q_status;
  RARITY rar;
  ITEM req_itm, itm_reward;
  char name[50], desc[256];
  int x_pos, y_pos, level_req;
  int req_am, cur_am;
  int currency_am_reward, reward_am, xp_reward_am;
};

typedef struct {
  DIR dir;
  DIR lk_dir;
  int hp, maxHP, spd, maxSpd, shield, weap, atk_spd, atk_pow, atk_am;
  int death_timer, hurt_timer, atk_reload, is_blocked;
  int x_pos, y_pos, width, height;
} SHIP;

struct NPC {
  NPC_TYPES type;
  DIR dir, lk_dir;
  ITEM *items;
  QUEST quest;
  int x_p, y_p, floor;
  char *lft_bdy, *rgt_bdy, *dwn_bdy, *up_bdy;
  char *text;
  char *name;
};

typedef struct {
  NPC *npcs;
  BLD_TYPE type;
  int x_size, y_size, x_s_ext, y_s_ext;
  int x_pos, y_pos;
  int npc_am, flr_am;
  char *ext_cont;
  char **floors;
} BLDING;

typedef struct {
  DIR dir;
  PRJ_TYPES type;
  float angular_velocity;
  int x_pos, y_pos, col_timer, angular_offset;
  int prv_index, dur;
} PROJ;

typedef struct {
  int x_pos, y_pos, z_pos;
} STAR;

typedef struct {
  CURRENCY_TYPES type;
  int amount;
} CURRENCY;

typedef struct {
  /*
  CURRENCY GLB_currencies[CURRENCY_COUNT - 1];
  QUEST *GLB_quest_list, *GLB_quests;
  ITEM SHIP, GLB_weapon, GLB_armor;
  DIR GLB_allowed_dir, GLB_allowed_lk_dir;
  ITEM *GLB_itm_ownd, *GLB_itm_list;
  MOVE_TYPE GLB_mv_typ;
  GLB_atk_typ GLB_atk_typ;
  PRJ_TYPES GLB_cur_proj;
  BLD_TYPE GLB_cur_blding;
  GAME_STATE GLB_game_state;
  GAME_STATE GLB_game_previous_state;
  NPC *GLB_cur_seller;
  char *GLB_minimap;
  long int GLB_score, GLB_max_score;
  int GLB_proj_quality, GLB_attr, GLB_shots_fired, GLB_level;
  float GLB_luck;
  int GLB_gl_x_pos, GLB_gl_y_pos;
  int GLB_plr_x_centr, GLB_plr_y_centr, GLB_prj_col_index, GLB_plr_col_index;
  int GLB_boss_mode, GLB_is_framing;
  int GLB_seller, GLB_num_items, GLB_num_GLB_quests;
  int GLB_owned_amnt, GLB_inv_incrmnt;
  int GLB_cur_bld_index, GLB_is_in_dialog, GLB_cur_floor;*/
} GAME;

typedef struct {
  PROJ *proj;
  BOSS_ACTIONS action, next_action;
  char *content;
  int cur_hp, prev_HP, max_HP, dmg, shield, speed, atk_spd, max_shield, prv_shield;
  int x_pos, y_pos, height, width, x_dir, y_dir;
  int time_in_state, speATK_hole, hole_xdir, hole_size, shld_ic_count;
} BOSS;

#define true 1;
#define false 0;
typedef int bool;

int main_loop();

// INIT
void initialize_game(SHIP *plr);
void init_boss(BOSS *boss);
int Init_bullet(PROJ *prj, PRJ_TYPES type, DIR dir, int BUFFER_SIZE, int x_pos, int y_pos);
char *init_blank_canv(int w, int h, int has_ext, char interior);
void init_building(BLD_TYPE type, BLDING *bld, int x, int y);
void init_game();
void initialize_game_objects();
void cleanup_game_objects();

// updS
int upd_input(char *canv, BOSS boss, SHIP *plr, int *has_moved, int *show_grid, int *god_mode);
void upd_game_state(char *canv, SHIP *plr, BOSS *boss, int has_moved, int *wind);
int upd_player(char *canv, SHIP *plr, int has_moved);
int upd_background(char *canv, BOSS boss, SHIP plr, int has_moved);
int upd_boss_fight(char *canv, BOSS *boss, SHIP *plr);
int upd_items(char *canv, SHIP *plr, int reset);
int upd_mobs(SHIP *plr, int reset);
int upd_plr_proj(char *canv, BOSS *boss, SHIP *plr, int reset);
int upd_mob_prj(char *canv, SHIP *plr, int reset);
int upd_wind(char *canv, int *wind, SHIP *shp, BOSS *boss);
int upd_buildings(char *canv, SHIP *plr, int *has_moved);
int upd_collisions(char *canv, SHIP *plr);
int handle_seller_menu(char *canv, NPC *GLB_seller, SHIP *plr);
void handle_inventory_menu(char *canv);
int launch_proj(SHIP *plr, PROJ *proj);

// RENDERING
void render_game(BOSS boss, SHIP *plr, int *wind, char *canv, char *prv_canv, int has_moved);
int render_canvas(char *canv, char *prev_canv);

// CANV
void fit_text_to_width(char *txt, int max_width);
char *set_new_width(char *src, int new_width);
char *write_on_canv(char *src, char *dst, int x_pos, int y_pos);
char *get_triangle(char interior, DIR dir, int h, int w, int color);
char *get_circle(int width, int height, char interior, float thickness);
void set_label(char *canv, int *start_index, const char *label, int value);
int set_text_red(char *src);
void render_text_in_color(char *c, char __, char *color);
int is_in_canv(int x_pos, int y_pos, int width, int height, int canv_x, int canv_y, int canv_w, int canv_h, DIR d);
char *get_formatted_text(const char *format, ...);
char *swap_line(char *txt, int i, DIR d);
char *get_text_bubble(char *txt, char *label);
int write_midle_msg(char *canv, int has_exterior, const char *format, ...);

// TOOLS
void set_nonblocking_mode(int enable);
int get_index(char *canv, int x_pos, int y_pos);
int get_width(const char *src);
int get_height(const char *src);
char get_index_value(char *canv, int c_width, int c_height, int x, int y);
int crop_x(char *src, int from_x);
int rand_range(int min, int max);
int is_pos_free(SHIP *mobs, int current_mob, int x, int y);
int is_pos_free_BUILDINGS(int current_mob, int x, int y);
int check_overlap(BLDING *a, BLDING *b);
int is_dir_allowed(DIR dir, DIR allowed_directions);
DIR get_prj_dir(int w_prj, int max_prj, SHIP plr, DIR d);
DIR get_dir_to_target(int target_x, int target_y, int x_pos, int y_pos, int has_diag);
int set_plr_death(char *canv, SHIP plr);
int is_in_string(char c, char *str);
int color_text(COLOR col, char *text, int fill_interior);
void Color_from_index(COLOR col, char **text);
void tile_canvas(char *canv, int width, int tile_size);
int get_char_and_freeze(char *allowed_chars);

// DEBUG
int show_header(char *canv, char *header, SHIP plr, BOSS boss, int has_moved);
int show_grid_view(char *canv, char show_grid);
int arrays_menu(GAME game);
void handle_mini_map(char *canv, SHIP plr, BOSS boss);
void handle_signal(int signal);

// AUDIO
void stop_sound(pid_t pid);
pid_t play_sound(const char *filename, int loop);

// ITEMS
ITEM *read_items_from_file(const char *filename, int *num_items);
ITEM **filter_items_by_type(ITEM *base, int type_sel, int max_items, int *filtered_count);
ITEM get_item(RARITY rar, ITEM_Type type, ITEM *items, int buffer_size);
ITEM Get_Item_Of_type(ITEM_Type Type, ITEM *list, int l_size);
char *get_ITEM_information(ITEM *t);
int contains(ITEM_Type *array, int length, ITEM_Type value);
int Get_itm_duplicate_index(ITEM a, ITEM *items, int size);
int determineRarity(float GLB_luck);
void reset_item(ITEM *itm);
void Copy_Item(ITEM *to, ITEM from);
void Generate_items(ITEM *list, NPC *n, int amount, int list_size);
void reorder_list(ITEM *list, int max_size);

// GLB_quests
QUEST *read_quest_from_file(const char *filename, int *num_quests);
QUEST **filter_quests_by_status(QUEST *base, QUEST_STATUS status_sel, int max_quests, int *filtered_count);
QUEST Get_Quest_of_type(QUESTS_TYPES Type, QUEST *list, int l_size);
QUEST Get_random_quest(QUEST *list, int l_size);
QUEST *set_in_list(QUEST *list, QUEST quest, int list_size);
int QST_Get_itm_duplicate_index(QUEST a, QUEST *quests_list, int list_size);
int set_new_quest(QUEST *GLB_quests_buffer, int buffer_size, QUEST ref);
int is_same_quest(QUEST a, QUEST b);
int get_empty_quest_index(QUEST *GLB_quest_list, int list_size);
void reorder_qst_list(QUEST *list, int max_size);
void reset_quest(QUEST *q);
void copy_quest(QUEST *to, QUEST from);
char *get_quest_description(QUEST q);
char *get_quest_debug_info(QUEST *q);

// SDL
SDL_Window *Init_window();
TTF_Font *init_ttf_fond();
SDL_Renderer *init_renderer(SDL_Window *window);

#endif