#include "../../headers/1_includes.h"

QUEST parse_quest(char *line, int cur_line, QUEST *qst) {
  if (cur_line == 0) {
    // Parsing quest name
    if (line[0] == '\0') {
      strcpy(qst->name, "undefined");
    } else {
      sscanf(line, "%[^\n]", qst->name);
    }
  } else if (cur_line == 1) {
    // Parsing quest description
    if (line[0] == '\0') {
      strcpy(qst->desc, "undefined");
    } else {
      sscanf(line, "%[^\n]", qst->desc);
      if (get_width(qst->desc) > CANV_W - 50) fit_text_to_width(qst->desc, CANV_W - 50);
    }
  } else if (cur_line == 2) {
    // Parsing level requirement, rarity, and quest type
    sscanf(line, "%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d",
           &qst->level_req, // level requirement
           &qst->rar,           // Rarity value
           &qst->q_type);       // Quest type
  } else if (cur_line == 3) {
    // Parsing required amount, currency reward, item reward amount, and XP reward amount
    sscanf(line, "req_am %d currency_reward %d item_reward_amount %d xp_reward %d",
           &qst->req_am,             // Required amount
           &qst->currency_am_reward, // Currency reward
           &qst->reward_am,          // Item reward amount
           &qst->xp_reward_am);      // XP reward amount
  }

  return *qst;
}

QUEST *read_quest_from_file(const char *filename, int *num_quests) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open quest file");
    *num_quests = 0;
    return NULL;
  }
  size_t len = 0;
  ssize_t read;
  char *line = NULL;
  int capacity = 10;
  int cur_line = 0;       // Tracks the current line of the quest
  int quest_complete = 0; // Flag to indicate when a quest is fully parsed
  *num_quests = 0;

  QUEST *quests = malloc(capacity * sizeof(QUEST));
  if (!quests) {
    perror("Failed to allocate memory for quests");
    fclose(file);
    return NULL;
  }

  // Loop through the lines in the file
  while ((read = getline(&line, &len, file)) != -1) {
    line[strcspn(line, "\n")] = '\0'; // Remove newline character

    // Skip empty lines, marking the end of a quest
    if (line[0] == '\0') {
      if (cur_line > 0) {
        (*num_quests)++;
        cur_line = 0;
        quest_complete = 1;
      }
      continue;
    }

    // Allocate more space if necessary
    if (*num_quests >= capacity) {
      capacity *= 2;
      quests = realloc(quests, capacity * sizeof(QUEST));
      if (!quests) {
        perror("Failed to reallocate memory for quests");
        free(line);
        fclose(file);
        return NULL;
      }
    }

    // Only parse a new quest if the previous one is complete
    if (quest_complete) {
      memset(&quests[*num_quests], 0, sizeof(QUEST)); // Reset the current quest
      quest_complete = 0;
    }

    // Parse the quest lines based on the current line in the sequence
    parse_quest(line, cur_line, &quests[*num_quests]);
    cur_line++;

    // If we've parsed 4 lines (a complete quest), mark quest as complete
    if (cur_line == 4) {
      (*num_quests)++;
      cur_line = 0;
      quest_complete = 1;
    }
  }

  // Ensure we add the last quest if file ends without a blank line
  if (cur_line > 0) { (*num_quests)++; }

  free(line);
  fclose(file);

  return quests;
}