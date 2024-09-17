#include "1_includes.h"

#ifndef DEFINE_H
#define DEFINE_H

#define SAVEFILE "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/stuff/savefile.txt"
#define ITEMFILE "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/stuff/items_data.txt"
#define QSTFILE "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/stuff/quests_data.txt"
#define BOSSFILE "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/stuff/boss_data.txt"

// AUDIO
#define AU_bg_world "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/music/bg_world.mp3"
#define AU_bg_house "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/music/bg_house.mp3"
#define AU_bg_fight "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/music/bg_fight.mp3"
#define AU_ship_mv "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/ship_drive.mp3"

#define AU_click1 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/UI/click1.mp3"
#define AU_click2 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/UI/click2.mp3"
#define AU_change_sel "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/UI/change_sel.mp3"
#define AU_proj1 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/UI/proj1.mp3"
#define AU_high_pitch "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/UI/high_pitch.mp3"
#define AU_high_pitch2 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/UI/high_pitch2.mp3"
#define AU_expl1 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/expl1.mp3"
#define AU_expl2 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/expl2.mp3"
#define AU_expl3 "audio_files/fx/expl3.mp3"
#define AU_expl4 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/expl4.mp3"
#define AU_hurt1 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/hurt1.mp3"
#define AU_hurt2 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/hurt2.mp3"
#define AU_hurt3 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/hurt3.mp3"
#define AU_hurt4 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/hurt4.mp3"
#define AU_hurt5 "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/hurt5.mp3"
#define AU_thump "/Users/giuliovalente/Desktop/DEV/Cs/ASCII_SPACE/audio_files/fx/thump.mp3"

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
#define PROJ_BUFF_SIZE 1000
#define PROJ_MOB_BUFF_SIZE 3000
#define ITEM_DROPS_BUFF_SIZE 200
#define INVENTORY_BUFF_SIZE 200
#define STAR_BUFF_SIZE 20
#define BLDING_BUFF_SIZE 100
#define WIND_BUFF_SIZE 60
#define PLR_WIND_BUFF_SIZE (WIND_BUFF_SIZE / 2)
#define CELL_BUFF_SIZE 8
#define QUESTS_BUFF_SIZE 60
#define SHOP_BUFF_SIZE 50

//  SIZES
#define CANV_W 120
#define UPG_CANV_W (CANV_W - (CANV_W / 6))
#define UPG_CANV_H (CANV_H - (CANV_H / 6))
#define CANV_H (CANV_X_CENTER)
#define CANV_SIZE (CANV_W + (CANV_H * CANV_W))
#define CELL_SIZE (CANV_W / CELL_BUFF_SIZE)
#define C_WIDTH (CANV_W / CELL_SIZE)
#define C_HEIGHT (CANV_H / CELL_SIZE)
#define PLR_SHIP_W (C_HEIGHT - 2)
#define PLR_SHIP_H (C_WIDTH - 2)

//  POSITIONS
#define CANV_X_CENTER CANV_W / 2
#define CANV_Y_CENTER CANV_H / 2
#define PLR_CENTER_X ((CANV_W + 1) / 2) - (PLR_SHIP_W / 2)
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
#define MINMAX(value_A, min, max) (value_A < min ? min : value_A > max ? max : value_A)

// PLAYER
#define circ_max_am 50
#define circ_add_interval 50
#define PLR_MAX_HP 5
#define HURT_DUR 60
#define shot_lock 50
#define HURT_SLEEPTIME 100000
#define PLR_GRID_SPD 10

// MOBS
#define MOB_HURT_DUR 5
#define MOB_DEATH_DUR 10
#define MOB_OUT_LIM 500
#define MOBS_BUFFER 150
#define MOBS1_BUFFER MOBS_BUFFER / 2
#define MOBS2_BUFFER MOBS_BUFFER / 2
#define MOBS_1_MAXHP 2
#define MOBS_2_MAXHP 8
#define MOBS_1_SPD 3
#define MOBS_2_SPD 5

// BOSS
#define BOSS_SPWNTIME 50000

// ICONS
#define SCR_SMB "★"
#define PROJ_SMB "+"
#define SHLD_SMB "⛨"
#define attr_SYMB "⚙"
#define luck_SMB "$"
#define HEART_SMB "❤️"
#define STAR_SMB "✧"

#define red_st '\xA0'
#define grn_st '\xA1'   // ¤
#define blu_st '\xA2'   // Æ
#define yel_st '\xA3'   // ×
#define orn_st '\xA4'   // §
#define prpl_st '\xA5'  // «
#define HEART_IC '\xA6' // ¶
#define RED_ICON '\xA7'
#define ERASE_IC '\xA8'

#define DWNSTAIR_IC '\xA8' // ¨
#define UPSTAIR_IC '\xA9'  // ¨

#define COL_HOR '\xB0' // Í
#define COL_VER '\xB1' // º
#define SHLD_OFF_MOB_IC ';'
#define SHLD_ON_MOB_IC '\xB3'
#define STAR_IC '\xB4'
#define STAR_IC2 '\xB5'
#define RAND_IC '\xB6'
#define BLD_BACKGROUND1 '\xB7' // Í
#define BLD_BACKGROUND2 '\xB8' // Í

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

// FX
#define EXPL_START '\xF0'   // à
#define PRJ_COL '\xF1'      // à
#define WIND_VER '\xF2'     // Þ
#define WIND_HOR '\xF3'     // ß
#define WIND_DOT '\xF4'     // ú
#define WIND_ALL_DOT '\xF5' // ú
#define WIND_ALL_VER '\xF6' // ú
#define WIND_ALL_HOR '\xF7' // ú

// PROJ
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

#define PRJ_RIC_VER '\xCE'    // ±
#define PRJ_RIC_HOR '\xCF'    // ÷
#define PRJ_RIC_DIAG_L '\xDA' // ¼
#define PRJ_RIC_DIAG_R '\xDB' // ½

#define MPRJ_BS_VER '\xDC'    // ±
#define MPRJ_BS_HOR '\xDD'    // ÷
#define MPRJ_BS_DIAG_L '\xDE' // ¼
#define MPRJ_BS_DIAG_R '\xDF' // ½

// minimap
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