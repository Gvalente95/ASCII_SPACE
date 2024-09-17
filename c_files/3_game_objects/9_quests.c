#include "../../headers/1_includes.h"

int is_same_quest(QUEST a, QUEST b) { return (a.desc == b.desc && a.name == b.name ? 1 : 0); }

char *get_quest_debug_info(QUEST *q) {
  if (!q || q->name[0] == '\0') return NULL;
  char *txt = malloc(100);
  snprintf(txt, 99, " %-20s  TYP %-15s ST %-10s X%-4dY%-4d  REQ_AM %-3d  RAR %d  XPR %-5d  $R %-5d", q->name, quest_types_lb[q->q_type], quest_status_lb[q->q_status], q->x_pos,
           q->y_pos, q->req_am, q->rar, q->xp_reward_am, q->currency_am_reward);
  fit_text_to_width(txt, CANV_W - 10);
  return (txt);
}

QUEST **filter_quests_by_status(QUEST *base, QUEST_STATUS status_sel, int max_quests, int *filtered_count) {
  QUEST **qsts = (QUEST **)malloc(max_quests * sizeof(QUEST *));
  if (!qsts) return NULL;
  status_sel = MINMAX(status_sel, 0, QUEST_STATUS_COUNT - 1);
  int bs_index = 0, qst_i = 0;
  while (base[bs_index].name[0] && bs_index < max_quests) {
    if (base[bs_index].q_status == status_sel) qsts[qst_i++] = &base[bs_index];
    bs_index++;
  }
  *filtered_count = qst_i;
  return qsts;
}

void reset_quest(QUEST *q) {
  q->desc[0] = '\0';
  q->name[0] = '\0';
  q->q_status = QUEST_STATUS_COUNT;
  q->cur_am = q->req_am = -1;
}

int get_empty_quest_index(QUEST *quest_list, int list_size) {
  if (quest_list == NULL || list_size <= 0) return (-1);
  for (int x = 0; x < list_size - 1; x++) {
    if (quest_list[x].name[0] == '\0') return (x);
  }
  return (-1);
}

QUEST *set_in_list(QUEST *list, QUEST quest, int list_size) {
  int index = get_empty_quest_index(list, list_size);
  if (index == -1) return (NULL);
  copy_quest(&list[index], quest);
  return (&list[index]);
}

QUEST Get_random_quest(QUEST *list, int l_size) { return (list[rand_range(0, l_size - 1)]); }

void copy_quest(QUEST *to, QUEST from) {
  strncpy(to->name, from.name, sizeof(to->name) - 1);
  strncpy(to->desc, from.desc, sizeof(to->desc) - 1);
  to->q_status = NEW;
  to->req_am = from.req_am;
  to->req_itm = from.req_itm;
  to->itm_reward = from.itm_reward;
  to->reward_am = from.reward_am;
  to->currency_am_reward = from.currency_am_reward;
  to->q_type = from.q_type;
  to->rar = from.rar;
  to->level_req = from.level_req;
  to->xp_reward_am = from.xp_reward_am;
}

QUEST Get_Quest_of_type(QUESTS_TYPES Type, QUEST *list, int l_size) {
  QUEST to_return;
  to_return.name[0] = '\0';
  if (!list || l_size <= 0) return (to_return);
  QUEST qst_bf[l_size - 1];
  int i = 0, qst_found = 0;
  for (int x = 0; x < l_size; x++) {
    if (Type == all || list[x].q_type == Type) {
      copy_quest(&qst_bf[i++], list[x]);
      qst_found = 1;
    }
  }
  if (!qst_found) return (to_return);
  return (qst_bf[rand_range(0, i - 1)]);
}

int QST_Get_itm_duplicate_index(QUEST a, QUEST *quests_list, int list_size) {
  if (list_size == 0) return (0);
  for (int i = 0; i < list_size; i++) {
    if (is_same_quest(a, quests_list[i])) return (i);
  }
  return (-1);
}

int set_new_quest(QUEST *quests_buffer, int buffer_size, QUEST ref) {
  int empty_index = -1;
  for (int i = 0; i < buffer_size; i++) {
    if (!quests_buffer[i].name[0] || quests_buffer[i].q_status == COMPLETE) {
      empty_index = i;
      break;
    }
  }
  if (empty_index == -1) return (0);
  copy_quest(&quests_buffer[empty_index], ref);
  return (1);
}

void reorder_qst_list(QUEST *list, int max_size) {
  int next_fill = 0;

  for (int i = 0; i < max_size; i++) {
    if (list[i].q_status == COMPLETE) {
      if (i != next_fill) {
        copy_quest(&list[next_fill], list[i]);
        reset_quest(&list[i]);
      }
      next_fill++;
    }
  }
  for (int j = next_fill; j < max_size; j++) {
    reset_quest(&list[j]);
  }
}
