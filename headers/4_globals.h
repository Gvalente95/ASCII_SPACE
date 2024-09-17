#include "1_includes.h"

#ifndef DIALOG_H
#define DIALOG_H

typedef struct {
  SHIP *mobs;
  PROJ *proj;
  PROJ *proj_mob;
  BLDING *buildings;
  ITEM *items;
  STAR *stars;
  BOSS *bosses;
} GAME_OBJECTS;
extern GAME_OBJECTS gmo;

extern GAME game;

// NPCS & dialogs
extern const char *npcs_names[200];
extern const char *slr_dlgs[14];
extern const char *forg_dlgs[13];
extern const char *shpr_dlgs[20];
extern const char *adv_dlgs[20];
extern const char *expl_dlgs[20];

// ENUMS LABELS
extern const char *pwr_ups[];
extern const char *weapons[];
extern const char *consum[];
extern const char *maps[];

extern const char *bonus_types_lb[];
extern const char *effect_types_lb[];
extern const char *target_types_lb[];
extern const char *rarity_types_lb[];
extern const char *itm_types_lb[];

extern const char *mv_type_lb[];
extern const char *atk_type_lb[];
extern const char *boss_act[];
extern const char *dir_types_lb[];
extern const char *prj_types_lb[];
extern const char *bld_types_lb[];
extern const char *game_state_types_lb[];

extern const char *quest_types_lb[];
extern const char *quest_status_lb[];
extern const char *currencies_lb[];

extern const char *npc_types_lb[];
extern const char *proj_types_lb[];

extern int GLB_time, GLB_input, GLB_last_input, GLB_shift_on;

extern CURRENCY GLB_currencies[CURRENCY_COUNT - 1];
extern QUEST *GLB_quest_list, *GLB_quests;
extern ITEM GLB_ship, GLB_weapon, GLB_armor;
extern DIR GLB_allowed_dir, GLB_allowed_lk_dir;
extern ITEM *GLB_itm_ownd, *GLB_itm_list;
extern MOVE_TYPE GLB_mv_typ;
extern ATK_TYPE GLB_atk_typ;
extern PRJ_TYPES GLB_cur_proj;
extern BLD_TYPE GLB_cur_blding;
extern GAME_STATE GLB_game_state;
extern GAME_STATE GLB_game_previous_state;
extern NPC *GLB_cur_seller;
extern char *GLB_minimap;
extern long int GLB_score, GLB_max_score;
extern int GLB_proj_quality, GLB_attr, GLB_shots_fired, GLB_level;
extern float GLB_luck;
extern int GLB_gl_x_pos, GLB_gl_y_pos;
extern int GLB_plr_x_centr, GLB_plr_y_centr, GLB_prj_col_index, GLB_plr_col_index;
extern int GLB_boss_mode, GLB_is_framing;
extern int GLB_seller, GLB_num_items, GLB_num_GLB_quests;
extern int GLB_owned_amnt, GLB_inv_incrmnt;
extern int GLB_cur_bld_index, GLB_is_in_dialog, GLB_cur_floor;
extern int GLB_time;
extern int GLB_input, GLB_last_input, GLB_shift_on;

#endif