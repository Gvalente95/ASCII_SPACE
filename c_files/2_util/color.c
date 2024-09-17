#include "../../headers/1_includes.h"
void Color_from_index(COLOR col, char **text) {
  if (!*text) return;
  char color_char = col == red ? red_st : col == blue ? blu_st : col == green ? grn_st : col == yellow ? yel_st : col == orange ? orn_st : col == purple ? prpl_st : red_st;
  int original_size = strlen(*text);
  int new_size = original_size * 2 + 1;
  char *new_text = malloc(new_size);
  if (!new_text) return;

  int new_i = 0, prv_i = 0;

  while (prv_i < original_size) {
    char c = (*text)[prv_i];
    if (c == '\n' || c == ' ') {
      new_text[new_i++] = c;
      prv_i++;
    } else
      new_text[new_i++] = color_char, new_text[new_i++] = (*text)[prv_i++];
  }

  new_text[new_i] = '\0';
  free(*text);
  *text = new_text;
}

int color_text(COLOR col, char *text, int fill_with_dots) {
  int i = 0;
  while (text[i]) {
    if (text[i] == ' ' && fill_with_dots)
      text[i] = col == red ? red_dot : col == blue ? blu_dot : col == green ? grn_dot : yel_dot;
    else {
      switch (text[i]) {
        case '(':
          text[i] = col == red ? red_L_parenth : col == blue ? blu_L_parenth : col == green ? grn_L_parenth : yel_L_parenth;
          break;
        case ')':
          text[i] = col == red ? red_R_parenth : col == blue ? blu_R_parenth : col == green ? grn_R_parenth : yel_R_parenth;
          break;
        case '|':
          text[i] = col == red ? red_ver_bar : col == blue ? blu_ver_bar : col == green ? grn_ver_bar : yel_ver_bar;
          break;
        case '_':
        case '-':
          text[i] = col == red ? red_hor_bar : col == blue ? blu_hor_bar : col == green ? grn_hor_bar : yel_hor_bar;
          break;
        case '.':
        case ',':
        case '\'':
          text[i] = col == red ? red_dot : col == blue ? blu_dot : col == green ? grn_dot : yel_dot;
          break;
        case 'v':
          text[i] = col == red ? red_dwn_arw : col == blue ? blu_dwn_arw : col == green ? grn_dwn_arw : yel_dwn_arw;
          break;
        case '^':
          text[i] = col == red ? red_up_arw : col == blue ? blu_up_arw : col == green ? grn_up_arw : yel_up_arw;
          break;
        case '<':
          text[i] = col == red ? red_lft_arw : col == blue ? blu_lft_arw : col == green ? grn_lft_arw : yel_lft_arw;
          break;
        case '>':
          text[i] = col == red ? red_rgt_arw : col == blue ? blu_rgt_arw : col == green ? grn_rgt_arw : yel_rgt_arw;
          break;
        case '/':
          text[i] = col == red ? red_R_diag_r : grn_R_diag_r;
          break;
        case '\\':
          text[i] = col == red ? red_L_diag_l : grn_L_diag_l;
          break;
        default:
          break;
      }
    }

    i++;
  }
  return 1;
}
