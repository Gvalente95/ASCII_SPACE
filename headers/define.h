#include "includes.h"

#ifndef DEFINE_H
#define DEFINE_H

#define SAVEFILE "savefile.txt"
#define ITEMFILE "items.txt"

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
#define PROJ_BUFFER 1000
#define PROJ_TYPES 4
#define PRJ_MOB_BUFFER 3000
#define ITEM_BUFFER 50
#define ITEMS_OWNED_BUFFER 200
#define STAR_BUFFER 20
#define BLDING_BUFFER 100
#define WIND_BUFFER 60
#define PLR_WIND_BUFFER (WIND_BUFFER / 2)
#define BOSS_WIND_MAX (WIND_BUFFER / 2)
#define CELL_AM 8

//  SIZE
#define CANV_W 100
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
#define MAX(value_A, value_B) (value_A > value_B ? value_A : value_B)
#define MIN(value_A, value_B) (value_A < value_B ? value_A : value_B)

// STATS

#define INCR_Y_DIR(object_direction) (())

// PLAYER
#define PLR_MAX_HP 5
#define BASE_ITEM_CHNC 30
#define HURT_DUR 60
#define shot_lock 50
#define STRT_ITEM_CHNC 10
#define HURT_SLEEPTIME 200000
#define PLR_GRID_SPD 10

// SHOP
#define SHOP_INVENTORY 50
#define SHOP_VIEW_ITEMS 10
#define SHOP_RENEWAL 5000

// BOSS
#define BOSS_SPWNTIME 50000
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
#define MOBS_BUFFER 150
#define MOBS1_BUFFER MOBS_BUFFER / 2
#define MOBS2_BUFFER MOBS_BUFFER / 2

#define MOBS_1_MAXHP 2
#define MOBS_2_MAXHP 8
#define MOBS_1_SPD 5
#define MOBS_2_SPD 10

// ICONS
#define SCR_SMB "★"
#define PROJ_SMB "+"
#define SHLD_SMB "⛨"
#define ATTR_SYMB "⚙"
#define LUCK_SMB "$"
#define HEART_SMB "❤️"
#define STAR_SMB "✧"

#define RED_ICON '\xA0'
#define COIN_IC '\xA1'   // ¤
#define WEAP_IC '\xA2'   // Æ
#define SHIELD_IC '\xA3' // ×
#define HEART_IC '\xA4'  // §
#define ATTR_IC '\xA5'   // «
#define LUCK_IC '\xA6'   // ¶
#define STARS_IC '\xA7'  // ¨

#define DWNSTAIR_IC '\xA8' // ¨
#define UPSTAIR_IC '\xA9'  // ¨

#define COL_HOR '\xB0' // Í
#define COL_VER '\xB1' // º
#define SHLD_OFF_MOB_IC '\xB2'
#define SHLD_ON_MOB_IC '\xB3'
#define STAR_IC '\xB4'
#define STAR_IC2 '\xB5'
#define RAND_IC '\xB6'

#define CANV_HOR '\xC0'       // Ä
#define CANV_VER '\xC1'       // ³
#define PLR_SHLD_OUTL '\xC2'  // Û
#define PLR_SHLD_OUTL1 '\xC3' // Ü
#define PLR_SHLD_OUTL2 '\xC4' // Ý

#define MOB_LBORD '\xD0' // É
#define MOB_RBORD '\xD1' // »
#define MOB_HOR '\xD2'   // Í
#define MOB_VER '\xD3'   // º

#define SHP_LBORD '\xE0' // Ù
#define SHP_RBORD '\xE1' // À
#define SHP_HOR '\xE2'   // Á
#define SHP_VER '\xE3'   // ´

#define EXPL_START '\xF0'   // à
#define PRJ_COL '\xF1'      // à
#define WIND_VER '\xF2'     // Þ
#define WIND_HOR '\xF3'     // ß
#define WIND_DOT '\xF4'     // ú
#define WIND_ALL_DOT '\xF5' // ú
#define WIND_ALL_VER '\xF6' // ú
#define WIND_ALL_HOR '\xF7' // ú

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

// MINIMAP
#define MOB_MM_IC '\xEA'   // ÷
#define PLR_MM_IC '\xEB'   // ÷
#define SHOP_MM_IC '\xEC'  // ÷
#define HOUSE_MM_IC '\xED' // ÷
#define FORT_MM_IC '\xEE'  // ÷
#define ARM_MM_IC '\xEF'   // ÷

// COLLISIONS
#define VRT_WALL '\xFA'    // ÷
#define HOR_WALL '\xFB'    // ÷
#define UPLFT_WALL '\xFC'  // ÷
#define UPRGT_WALL '\xFD'  // ÷
#define DWNLFT_WALL '\xFE' // ÷
#define DWNRGT_WALL '\xFF' // ÷

// COLORED SYMBOLS (Adjusted Hex Values)
#define red_L_parenth '\x80'
#define red_R_parenth '\x81'
#define red_ver_bar '\x82'
#define red_hor_bar '\x83'
#define red_dot '\x84'
#define red_dwn_arw '\x85'
#define red_up_arw '\x86'
#define red_lft_arw '\x87'
#define red_rgt_arw '\x88'

#define grn_L_parenth '\x89'
#define grn_R_parenth '\x8A'
#define grn_ver_bar '\x8B'
#define grn_hor_bar '\x8C'
#define grn_dot '\x8D'
#define grn_dwn_arw '\x8E'
#define grn_up_arw '\x8F'
#define grn_lft_arw '\x90'
#define grn_rgt_arw '\x91'

#define blu_L_parenth '\x92'
#define blu_R_parenth '\x93'
#define blu_ver_bar '\x94'
#define blu_hor_bar '\x95'
#define blu_dot '\x96'
#define blu_dwn_arw '\x98'
#define blu_up_arw '\x99'
#define blu_lft_arw '\x9A'
#define blu_rgt_arw '\x9B'

#define yel_L_parenth '\x9C'
#define yel_R_parenth '\x9D'
#define yel_ver_bar '\x9E'
#define yel_hor_bar '\x9F'
#define yel_dot '\x00'
#define yel_dwn_arw '\x01'
#define yel_up_arw '\x02'
#define yel_lft_arw '\x03'
#define yel_rgt_arw '\x04'

#define red_L_diag_l '\x05'
#define red_R_diag_r '\x06'
#define grn_L_diag_l '\x07'
#define grn_R_diag_r '\x08'
#endif