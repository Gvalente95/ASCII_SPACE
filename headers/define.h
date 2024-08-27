#include "includes.h"

#ifndef DEFINE_H
#define DEFINE_H

#define SAVEFILE "savefile.txt"

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

// DEBUG
#define SHOW_GRID 0

//  BUFFERS
#define PROJ_BUFFER 200
#define PROJ_TYPES 4

#define PRJ_MOB_BUFFER 3000
#define LOOT_BUFFER 50
#define STAR_BUFFER 5
#define BLDING_BUFFER 5
#define WIND_BUFFER 60
#define PLR_WIND_BUFFER (WIND_BUFFER / 2)
#define BOSS_WIND_MAX (WIND_BUFFER / 2)
#define CELL_AM 8

//  SIZE
#define CANV_W 120
#define UPG_CANV_W (CANV_W - (CANV_W / 6))
#define UPG_CANV_H (CANV_H - (CANV_H / 6))
#define CANV_H (CANV_X_CENTER)
#define CANV_SIZE (CANV_W + (CANV_H * CANV_W))
#define CELL_SIZE (CANV_W / CELL_AM)
#define C_WIDTH (CANV_W / CELL_SIZE)
#define C_HEIGHT (CANV_H / CELL_SIZE)
#define SHIP_W (C_HEIGHT - 2)
#define SHIP_H (C_WIDTH - 2)
#define BLDING_MAX_SIZE 50
#define BLDING_MIN_SIZE 10

//  POSITIONS
#define CANV_X_CENTER CANV_W / 2
#define CANV_Y_CENTER CANV_H / 2
#define PLR_CENTER_X ((CANV_W + 1) / 2) - (SHIP_W / 2)
#define PLR_CENTER_Y (CANV_H / 2 + 2)
#define PLR_CENTER_Y_BOSS (CANV_H - 10)

#define BOSS_CENTER_Y 9
#define CENTER_X(object_width) ((CANV_X_CENTER) - (object_width) / 2)
#define CENTER_Y(object_height) ((CANV_Y_CENTER) - (object_height) / 2)
#define MOVE_TOWARD_CENTER_X(pos, object_width) (pos += (pos < CENTER_X(object_width)) ? 1 : (pos > CENTER_X(object_width)) ? -1 : 0)
#define MOVE_TOWARD_CENTER_Y(pos, object_height) (pos += (pos < CENTER_Y(object_height)) ? 1 : (pos > CENTER_Y(object_height)) ? -1 : 0)
#define PERCENTILE(value, max) (((value) * 100) / (max))
// STATS

#define INCR_Y_DIR(object_direction) (())

// PLAYER
#define PLR_MAX_HP 5
#define BASE_LOOT_CHNC 30
#define HURT_DUR 60
#define shot_lock 50
#define STRT_LOOT_CHNC 10
#define HURT_SLEEPTIME 200000

// BOSS
#define BOSS_SPWNTIME 1000
#define BOSS_ATK0_DUR 600
#define BOSS_ATK1_DUR 50
#define BOSS_ATK2_DUR 600
#define BOSS_HURT_DUR 50
#define BOSS_REGEN_DUR 200
#define BOSS_REPOS_DUR 200
#define BOSS_SP_ATK_DUR 600
#define BOSS_SP2_ATK_DUR 150
#define BOSS_RETREAT_DUR 250
#define BOSS_SHIELD_DUR 200

// MOBS
#define MOB_OUT_LIM 500
#define MOBS_BUFFER 50
#define MOBS1_BUFFER MOBS_BUFFER / 2
#define MOBS2_BUFFER MOBS_BUFFER / 2

#define MOBS_1_MAXHP 1
#define MOBS_2_MAXHP 5
#define MOBS_1_SPD 1
#define MOBS_2_SPD 4

// ICONS
#define SCR_SMB "★"
#define PROJ_SMB "+"
#define SHLD_SMB "⛨"
#define ATTR_SYMB "⚙"
#define LUCK_SMB "$"
#define HEART_SMB "❤️"

#define SHLD_OFF_MOB_IC ';'
#define SHLD_ON_MOB_IC '&'

#define STAR_IC '@'
#define STAR_IC2 '\xA8'
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
#define WIND_VER '\xF3'   // Þ
#define WIND_HOR '\xF4'   // ß
#define WIND_DOT '\xF5'   // ú

#define PRJ_BS_VER '\xAA'    // ±
#define PRJ_BS_HOR '\xAB'    // ÷
#define PRJ_BS_DIAG_L '\xAC' // ¼
#define PRJ_BS_DIAG_R '\xAD' // ½

#define PRJ_MAGN_VER '\xAE'    // ±
#define PRJ_MAGN_HOR '\xAF'    // ÷
#define PRJ_MAGN_DIAG_L '\xBA' // ¼
#define PRJ_MAGN_DIAG_R '\xBB' // ½

#define PRJ_BMB_VER '\xBC'    // ±
#define PRJ_BMB_HOR '\xBD'    // ÷
#define PRJ_BMB_DIAG_L '\xBE' // ¼
#define PRJ_BMB_DIAG_R '\xBF' // ½

#define PRJ_BIG_VER '\xCA'    // ±
#define PRJ_BIG_HOR '\xCB'    // ÷
#define PRJ_BIG_DIAG_L '\xCC' // ¼
#define PRJ_BIG_DIAG_R '\xCD' // ½

#define MPRJ_BS_VER '\xCE'    // ±
#define MPRJ_BS_HOR '\xCF'    // ÷
#define MPRJ_BS_DIAG_L '\xDA' // ¼
#define MPRJ_BS_DIAG_R '\xDB' // ½

#endif