#include "core.h"

#include <cassert>
#include <math.h>
#include "SDL.h"
#include <stdio.h>

MakeMoveResult make_move_table_partial[2][0b01000000];
MakeMoveResult make_move_table_full[2][0b01111111];

void init_board(BoardState state)
{
    for (uint8_t col = 0; col < 7; ++col)
    {
        state[col] = 1;
    }
}

inline uint8_t get_board_value(BoardState state, uint8_t row, uint8_t col)
{
    assert(row < 6 && col < 7);

    return state[col] < (1 << (row + 1)) ? 0 : ((state[col] & (1 << row)) >> row) + 1;
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

void init_core()
{
    SDL_Init(0);

    // Building partial lookup table (skips full columns)
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

    // Building full lookup table (includes full columns)
    for (uint8_t player = 0; player < 2; ++player)
    {
        make_move_table_full[player][0] = MakeMoveResult { -1, 0 };
        for (uint8_t column_state = 1; column_state > 0; ++column_state)
        {
            MakeMoveResult result;
            result.column_state = column_state;
            result.row = make_move_column(player, &result.column_state);
            make_move_table_full[player][column_state] = result;
        }
    }
}

int8_t make_move(uint8_t player, uint8_t column, BoardState state)
{
    assert(column >= 0 && column < 7);

    return make_move_column(player, &state[column]);
}

bool check_for_win(BoardState state, uint8_t last_move_row, uint8_t last_move_col)
{
    assert(last_move_row < 6 && last_move_col < 7);

    uint8_t last_move_player = get_board_value(state, last_move_row, last_move_col);

    // Vertical
    switch (state[last_move_col])
    {
        case 0b00010000:
        case 0b00011111:
        case 0b00100001:
        case 0b00111110:
        case 0b01000010:
        case 0b01000011:
        case 0b01111100:
        case 0b01111101:
            return true;
        default:
            break;
    }

    // Horizontal
    uint8_t center = get_board_value(state, last_move_row, 3);
    if (center == last_move_player)
    {
        uint8_t count = 1;
        for (int8_t col = 2; col >= 0; --col)
        {
            if (get_board_value(state, last_move_row, col) != center)
                break;

            ++count;
        }

        if (count == 4)
        {
            return true;
        }

        for (uint8_t col = 4; col < 7; ++col)
        {
            if (get_board_value(state, last_move_row, col) != center)
                break;

            ++count;

            if (count == 4)
            {
                return true;
            }
        }
    }

    // Diagonal (bottom-left to top-right)
    uint8_t count = 1;
    int8_t diag_row = last_move_row - 1;
    int8_t diag_col = last_move_col - 1;
    for (; diag_row >= 0 && diag_col >= 0; --diag_row, --diag_col)
    {
        if (get_board_value(state, diag_row, diag_col) != last_move_player)
            break;

        ++count;

        if (count >= 4)
        {
            return true;
        }
    }

    diag_row = last_move_row + 1;
    diag_col = last_move_col + 1;
    for (; diag_row < 6 && diag_col < 7; ++diag_row, ++diag_col)
    {
        if (get_board_value(state, diag_row, diag_col) != last_move_player)
            break;

        ++count;

        if (count >= 4)
        {
            return true;
        }
    }

    // Diagonal (top-left to bottom-right)
    count = 1;
    diag_row = last_move_row + 1;
    diag_col = last_move_col - 1;
    for (; diag_row < 6 && diag_col >= 0; ++diag_row, --diag_col)
    {
        if (get_board_value(state, diag_row, diag_col) != last_move_player)
            break;

        ++count;

        if (count >= 4)
        {
            return true;
        }
    }

    diag_row = last_move_row - 1;
    diag_col = last_move_col + 1;
    for (; diag_row >= 0 && diag_col < 7; --diag_row, ++diag_col)
    {
        if (get_board_value(state, diag_row, diag_col) != last_move_player)
            break;

        ++count;

        if (count >= 4)
        {
            return true;
        }
    }

    return false;
}

uint64_t total_moves_evaluated = 0;
uint64_t wins_found = 0;
uint64_t dead_ends_found = 0;
uint8_t min_depth = 50;
uint64_t time_of_last_print;
uint64_t get_move_score_start_time;

void sprint_friendly_time(double_t total_seconds, char* buffer)
{
    uint32_t days = (uint32_t)(total_seconds / (60 * 60 * 24));
    total_seconds -= days * 60.0 * 60.0 * 24.0;
    uint32_t hours = (uint32_t)(total_seconds / (60 * 60));
    total_seconds -= hours * 60.0 * 60.0;
    uint32_t minutes = (uint32_t)(total_seconds / 60);
    total_seconds -= minutes * 60.0;
    uint32_t seconds = (uint32_t)total_seconds;

    if (days > 0)
    {
        snprintf(buffer, 256, "%d day%s, %d hour%s, %d minute%s, %d second%s",
            days, days > 1 ? "s" : "",
            hours, hours > 1 ? "s" : "",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else if (hours > 0)
    {
        snprintf(buffer, 256, "%d hour%s, %d minute%s, %d second%s",
            hours, hours > 1 ? "s" : "",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else if (minutes > 0)
    {
        snprintf(buffer, 256, "%d minute%s, %d second%s",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else
    {
        snprintf(buffer, 256, "%d second%s",
            seconds, seconds > 1 ? "s": "");
    }
}

int32_t get_move_score(uint8_t player, uint8_t player_this_turn, uint8_t col, BoardState state, bool *move_possible, uint8_t depth)
{
    uint64_t time_current = SDL_GetTicks();
    if (time_current - time_of_last_print >= 1000)
    {
        time_of_last_print = time_current;

        double_t percent_completed = 100.0 * total_moves_evaluated / 4531985219092;
        uint64_t time_elapsed = time_current - get_move_score_start_time;
        double_t time_to_complete = (time_elapsed / 1000.0) * (100.0 / percent_completed);

        printf("Total moves: %llu (%f%%), wins found: %llu, dead-ends found: %llu, depth: %d\n", total_moves_evaluated, percent_completed, wins_found, dead_ends_found, min_depth);

        char* friendly_time_string = (char*)malloc(sizeof(char) * 512);
        sprint_friendly_time(time_to_complete, friendly_time_string);
        printf("- Time to complete: %s\n", friendly_time_string);
        free(friendly_time_string);
    }

    BoardState state_new;
    memcpy(state_new, state, sizeof(BoardState));
    int8_t row = make_move(player_this_turn, col, state_new);
    *move_possible = row >= 0;
    if (*move_possible == false)
    {
        ++dead_ends_found;

        return 0;
    }

    ++total_moves_evaluated;

    bool result = check_for_win(state_new, row, col);
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
                case 1:
                    snprintf(buffer, 256, "X");
                    buffer += 1;
                    break;
                case 2:
                    snprintf(buffer, 256, "O");
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