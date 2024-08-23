#include "header.h"

void initialize_projectile(PROJ *proj, int i, DIR dir, SHIP *plr) {
  proj[i].x_pos = dir == up || dir == down ? plr->x_pos + SHIP_W / 2 : dir == left || dir == up_left ? plr->x_pos - 3 : plr->x_pos + SHIP_W + 3;
  proj[i].y_pos = dir == up || dir == up_left || dir == up_right         ? plr->y_pos - 1
                  : dir == down || dir == down_left || dir == down_right ? plr->y_pos + SHIP_H / 2
                  : dir == left || dir == right                          ? plr->y_pos + 1
                                                                         : plr->y_pos - 1;
}

DIR determine_direction(int w_prj, int max_weap, SHIP plr) {
  if (w_prj == max_weap) return (plr.dir);
  if (w_prj == max_weap - 1) return (plr.dir == up ? down : plr.dir == down ? up : plr.dir == left ? right : left);
  if (w_prj == max_weap - 2) return (plr.dir == up ? left : plr.dir == down ? right : plr.dir == left ? up : down);
  if (w_prj == max_weap - 3) return (plr.dir == up ? right : plr.dir == down ? left : plr.dir == left ? down : up);
  if (w_prj == max_weap - 4) return (plr.dir == up ? up_right : plr.dir == down ? up_left : plr.dir == left ? down_left : down_right);
  if (w_prj == max_weap - 5) return (plr.dir == up ? up_left : plr.dir == down ? up_right : plr.dir == left ? down_right : down_left);
  if (w_prj == max_weap - 6) return (plr.dir == up ? down_left : plr.dir == down ? down_right : plr.dir == left ? up_left : up_right);
  if (w_prj == max_weap - 7) return (plr.dir == up ? down_right : plr.dir == down ? down_left : plr.dir == left ? up_right : up_left);
  return left; // Default direction
}