#include "core.h"
#include "helpers.h"

#include <cassert>
#include <math.h>
#include "SDL.h"
#include <stdio.h>

struct WinningBoardCollection
{
    uint8_t num_boards;
    uint64_t* boards;
};

MakeMoveResult make_move_table_partial[2][MAKE_MOVE_TABLE_PARTIAL_SIZE];
MakeMoveResult make_move_table_full[2][MAKE_MOVE_TABLE_FULL_SIZE];

const uint8_t CHECK_FOR_WIN_PROCESSED_COLUMN_TABLE_SIZE = 0b01111111;
uint8_t check_for_win_processed_column_table[2][CHECK_FOR_WIN_PROCESSED_COLUMN_TABLE_SIZE];
WinningBoardCollection check_for_win_processed_winning_boards_table[6][7];

void init_board(BoardState state)
{
    for (uint8_t col = 0; col < 7; ++col)
    {
        state[col] = 1;
    }
}

inline int8_t get_board_value(BoardState state, uint8_t row, uint8_t col)
{
    assert(row < 6 && col < 7);

    return state[col] < (1 << (row + 1)) ? -1 : ((state[col] & (1 << row)) >> row);
}

int8_t make_move_column(uint8_t player, uint8_t* column_state)
{
    if (*column_state >= 0b01000000)
        return -1;

    uint8_t mask = *column_state;      // 0011 0000 (example)
    mask |= mask >> 1;                 // 0011 1000
    mask |= mask >> 2;                 // 0011 1110
    mask |= mask >> 4;                 // 0011 1111
    ++mask;                            // 0100 0000

    // add 1 to top of stack
    *column_state |= mask;             // 0111 0000
    if (player == 0)
    {
        // clear top piece of the stack for player 0
        *column_state &= ~(mask >> 1); // 0101 0000
    }

    switch (mask)
    {
        case 0b00000010:
            return 0;
        case 0b00000100:
            return 1;
        case 0b00001000:
            return 2;
        case 0b00010000:
            return 3;
        case 0b00100000:
            return 4;
        case 0b01000000:
            return 5;
        default:
            assert(false);
            return -1;
    }
}

inline uint64_t bit_set(uint64_t num, uint8_t row, uint8_t col)
{
    return num | ((uint64_t)1 << (col * 8 + (row)));
}

void init_core()
{
    SDL_Init(0);

    // Building make_move partial lookup table (skips full columns)
    for (uint8_t player = 0; player < 2; ++player)
    {
        make_move_table_partial[player][0] = MakeMoveResult { -1, 0 };
        for (uint8_t column_state = 1; column_state < 0b01000000; ++column_state)
        {
            MakeMoveResult result;
            result.column_state = column_state;
            result.row = make_move_column(player, &result.column_state);
            make_move_table_partial[player][column_state] = result;
        }
    }

    // Building make_move full lookup table (includes full columns)
    for (uint8_t player = 0; player < 2; ++player)
    {
        make_move_table_full[player][0] = MakeMoveResult { -1, 0 };
        for (uint8_t column_state = 1; column_state < MAKE_MOVE_TABLE_FULL_SIZE; ++column_state)
        {
            MakeMoveResult result;
            result.column_state = column_state;
            result.row = make_move_column(player, &result.column_state);
            make_move_table_full[player][column_state] = result;
        }
    }

    // Building check_for_win processed column lookup table
    for (uint8_t player = 0; player < 2; ++player)
    {
        check_for_win_processed_column_table[player][0] = 0;
        for (uint8_t in_column = 1; in_column < CHECK_FOR_WIN_PROCESSED_COLUMN_TABLE_SIZE; ++in_column)
        {
            // Remove top-of-stack bit
            uint8_t mask = in_column;           // 0011 0000 (example)
            mask |= mask >> 1;                  // 0011 1000
            mask |= mask >> 2;                  // 0011 1110
            mask |= mask >> 4;                  // 0011 1111
            mask >>= 1;                         // 0001 1111

            uint8_t out_column = in_column;
            if (player == 0)
            {
                out_column = ~out_column;       // 1100 1111
                out_column &= mask;             // 0000 1111
            }
            else
            {
                out_column &= mask;             // 0001 0000
            }

            check_for_win_processed_column_table[player][in_column] = out_column;
        }
    }

    // Building check_for_win winning board lookup table
    for (uint8_t table_row = 0; table_row < 6; ++table_row)
    {
        for (uint8_t table_col = 0; table_col < 7; ++table_col)
        {
            uint64_t boards[32];
            uint8_t num_boards = 0;

            // Vertical
            for (uint8_t row_start = 0; row_start < 3; ++row_start)
            {
                uint64_t winning_board_state = 0;
                for (uint8_t row_offset = 0; row_offset < 4; ++row_offset)
                {
                    winning_board_state = bit_set(winning_board_state, row_start + row_offset, table_col);
                }

                boards[num_boards] = winning_board_state;
                ++num_boards;
            }

            // Horizontal
            for (uint8_t col_start = 0; col_start < 4; ++col_start)
            {
                uint64_t winning_board_state = 0;
                for (uint8_t col_offset = 0; col_offset < 4; ++col_offset)
                {
                    winning_board_state = bit_set(winning_board_state, table_row, col_start + col_offset);
                }

                boards[num_boards] = winning_board_state;
                ++num_boards;
            }

            // Diagonal (bottom-left to top-right)
            {
                uint8_t diag_row = table_row;
                uint8_t diag_col = table_col;
                while (diag_row > 0 && diag_col > 0)
                {
                    --diag_row;
                    --diag_col;
                }

                while (diag_row + 4 <= 6 && diag_col + 4 <= 7)
                {
                    uint64_t winning_board_state = 0;
                    for (uint8_t i = 0; i < 4; ++i)
                    {
                        winning_board_state = bit_set(winning_board_state, diag_row + i, diag_col + i);
                    }

                    boards[num_boards] = winning_board_state;
                    ++num_boards;

                    ++diag_row;
                    ++diag_col;
                }
            }

            // Diagonal (top-left to bottom-right)
            {
                int8_t diag_row = table_row;
                uint8_t diag_col = table_col;
                while (diag_row + 1 < 6 && diag_col > 0)
                {
                    ++diag_row;
                    --diag_col;
                }

                while ((diag_row + 1) - 4 >= 0 && diag_col + 4 <= 7)
                {
                    uint64_t winning_board_state = 0;
                    for (uint8_t i = 0; i < 4; ++i)
                    {
                        winning_board_state = bit_set(winning_board_state, diag_row - i, diag_col + i);
                    }

                    boards[num_boards] = winning_board_state;
                    ++num_boards;

                    --diag_row;
                    ++diag_col;
                }
            }

            WinningBoardCollection board_collection;
            board_collection.num_boards = num_boards;
            board_collection.boards = (uint64_t*)malloc(num_boards * sizeof(uint64_t));
            memcpy(board_collection.boards, boards, num_boards * sizeof(uint64_t));

            check_for_win_processed_winning_boards_table[table_row][table_col] = board_collection;
        }
    }
}

int8_t make_move(uint8_t player, uint8_t column, BoardState state)
{
    assert(column >= 0 && column < 7);

    return make_move_column(player, &state[column]);
}

bool check_for_win(BoardState state, uint8_t last_move_player, uint8_t last_move_row, uint8_t last_move_col)
{
    assert(last_move_player <= 1);
    assert(last_move_row < 6 && last_move_col < 7);

    uint8_t processed_board_state[8] = { 0 };
    for (uint8_t col = 0; col < 7; ++col)
    {
        assert(state[col] < 0b10000000);
        processed_board_state[col] = check_for_win_processed_column_table[last_move_player][state[col]];
    }

    uint64_t* processed_board_state_binary = (uint64_t*)processed_board_state;

    WinningBoardCollection winning_boards = check_for_win_processed_winning_boards_table[last_move_row][last_move_col];
    for (uint8_t i = 0; i < winning_boards.num_boards; ++i)
    {
        uint64_t winning_board_state = winning_boards.boards[i];
        if ((winning_board_state & *processed_board_state_binary) == winning_board_state)
        {
            return true;
        }
    }

    return false;
}

uint64_t total_moves_evaluated;
uint64_t wins_found;
uint64_t dead_ends_found;
uint8_t min_depth;
uint64_t time_of_last_print;
uint64_t get_move_score_start_time;

int32_t get_move_score(uint8_t player, uint8_t player_this_turn, uint8_t col, BoardState state, bool *move_possible, uint8_t depth)
{
    uint64_t time_current = SDL_GetTicks();
    if (time_current - time_of_last_print >= 1000)
    {
        time_of_last_print = time_current;

        double_t percent_completed = 100.0 * total_moves_evaluated / 4531985219092;
        uint64_t time_elapsed = time_current - get_move_score_start_time;
        double_t time_to_complete = (time_elapsed / 1000.0) * (100.0 / percent_completed);

        printf("Total moves: %lu (%f%%), wins found: %lu, dead-ends found: %lu, depth: %d\n", total_moves_evaluated, percent_completed, wins_found, dead_ends_found, min_depth);

        char friendly_time_string[256];
        sprint_friendly_time(time_to_complete, friendly_time_string);
        printf("- Time to complete: %s\n", friendly_time_string);
    }

    BoardState state_new;
    memcpy(state_new, state, sizeof(BoardState));
    int8_t row = make_move_lookup_full(player_this_turn, col, state_new);
    *move_possible = row >= 0;
    if (*move_possible == false)
    {
        ++dead_ends_found;

        return 0;
    }

    ++total_moves_evaluated;

    bool result = check_for_win(state_new, player_this_turn, row, col);
    if (result)
    {
        ++wins_found;

        return 0;
    }

    int32_t total_score = 0;
    player_this_turn = 1 - player_this_turn;
    for (uint8_t col = 0; col < 7; ++col)
    {
        bool temp_move_possible;
        total_score += get_move_score(player, player_this_turn, col, state_new, &temp_move_possible, depth + 1);
    }

    min_depth = depth < min_depth ? depth : min_depth;

    return total_score;
}

// @TODO: lol this function still only gets the move score for one move, gotta get back to that
int32_t get_move_score_full()
{
    BoardState state = { 1, 1, 1, 1, 1, 1, 1 };
    bool move_possible;
    get_move_score_start_time = SDL_GetTicks();
    time_of_last_print = get_move_score_start_time;
    total_moves_evaluated = 0;
    wins_found = 0;
    dead_ends_found = 0;
    min_depth = 50;

    return get_move_score(0, 0, 0, state, &move_possible, 1);
}

void sprint_board(BoardState state, char* buffer)
{
    snprintf(buffer, 256, "_1_2_3_4_5_6_7_\n");
    buffer += 16;
    for (int8_t row = 6 - 1; row >= 0; --row)
    {
        snprintf(buffer, 256, "|");
        buffer += 1;
        for (uint8_t col = 0; col < 7; ++col)
        {
            switch (get_board_value(state, row, col))
            {
                case 0:
                    snprintf(buffer, 256, "O");
                    buffer += 1;
                    break;
                case 1:
                    snprintf(buffer, 256, "X");
                    buffer += 1;
                    break;
                default:
                    snprintf(buffer, 256, " ");
                    buffer += 1;
                    break;
            }

            snprintf(buffer, 256, "|");
            buffer += 1;
        }
        snprintf(buffer, 256, "\n");
        buffer += 1;
    }
    snprintf(buffer, 256, "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");
}

void print_board(BoardState state)
{
    char* board = (char*)malloc(sizeof(char) * 256);
    sprint_board(state, board);
    printf("%s", board);
    free(board);
}