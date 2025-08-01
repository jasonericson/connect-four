#pragma once

#include <cassert>
#include <stdint.h>

typedef uint8_t BoardState[7];

void init_board(BoardState state);
void init_core();
int8_t make_move(uint8_t player, uint8_t column, BoardState state);

struct MakeMoveResult
{
    int8_t row;
    uint8_t column_state;
};

extern MakeMoveResult make_move_table_partial[2][0b01000000];
extern MakeMoveResult make_move_table_full[2][0b01111111];

inline int8_t make_move_lookup_partial(uint8_t player, uint8_t column, BoardState state)
{
    assert(column >= 0 && column < 7);

    if (state[column] >= 0b01000000)
        return -1;

    MakeMoveResult result = make_move_table_partial[player][state[column]];
    state[column] = result.column_state;
    return result.row;
}

inline int8_t make_move_lookup_full(uint8_t player, uint8_t column, BoardState state)
{
    assert(column >= 0 && column < 7);

    MakeMoveResult result = make_move_table_full[player][state[column]];
    state[column] = result.column_state;
    return result.row;
}

bool check_for_win(BoardState state, uint8_t last_move_row, uint8_t last_move_col);
bool check_for_win(BoardState state);
int32_t get_move_score(uint8_t player, uint8_t player_this_turn, uint8_t col, BoardState state, bool *move_possible, uint8_t depth);
int32_t get_move_score_full();
void sprint_board(BoardState state, char* buffer);
void print_board(BoardState state);
