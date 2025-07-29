#include <stdint.h>

typedef uint8_t BoardState[7];

bool make_move(uint8_t player, uint8_t column, BoardState state);
bool check_for_win(BoardState state);
int32_t get_move_score(uint8_t player, uint8_t player_this_turn, uint8_t col, BoardState state, bool *move_possible, uint8_t depth);
int32_t get_move_score_full();
