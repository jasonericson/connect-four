#include "core.h"

#include <cassert>
#include <math.h>
#include "SDL.h"
#include <stdio.h>

inline uint8_t bit_check(uint64_t num, uint8_t pos)
{
    return !!(num & ((uint64_t)1 << pos));
}

inline uint64_t bit_set(uint64_t num, uint8_t pos)
{
    return num | ((uint64_t)1 << pos);
}

uint8_t get_board_value(BoardState state, uint8_t row, uint8_t col)
{
    assert(row < 6 && col < 7);

    return state[col] < (1 << (row + 1)) ? 0 : ((state[col] & (1 << row)) >> row) + 1;

    // uint8_t bit_pos = (row * 7) + col;

    // uint8_t result = bit_check(state->player1, bit_pos) + bit_check(state->player2, bit_pos) * 2;
    // assert(result >= 0 && result < 3);s

    // // return bit_check(state->player1, bit_pos) ? 1 : (bit_check(state->player2, bit_pos) ? 2 : 0);
    // return bit_check(state->player1, bit_pos) + bit_check(state->player2, bit_pos) * 2;
}

bool make_move(uint8_t player, uint8_t column, BoardState state)
{
    assert(column >= 0 && column < 7);

    uint8_t probe = 0b01000000;

    if (state[column] >= probe)
        return false;

    while (state[column] < probe)
    {
        probe >>= 1;
    }

    if (player == 1)
    {
        state[column] &= ~(probe);
    }

    state[column] |= (probe << 1);

    return true;
}

bool check_for_win(BoardState state)
{
    // Vertical
    for (uint8_t col = 0; col < 7; ++col)
    {
        switch (state[col])
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
    }

    // Horizontal
    for (uint8_t row = 0; row < 6; ++row)
    {
        uint8_t center = get_board_value(state, row, 3);
        if (center == 0)
            continue;

        uint8_t count = 1;
        for (int8_t col = 2; col >= 0; --col)
        {
            if (get_board_value(state, row, col) != center)
                break;

            ++count;
        }

        if (count == 4)
        {
            return true;
        }

        for (uint8_t col = 4; col < 7; ++col)
        {
            if (get_board_value(state, row, col) != center)
                break;

            ++count;

            if (count == 4)
            {
                return true;
            }
        }
    }

    // Diagonal (bottom-left to top-right)
    for (int8_t row = 0; row < 6; ++row)
    {
        uint8_t center = get_board_value(state, row, 3);
        if (center == 0)
            continue;

        uint8_t count = 1;
        int8_t diag_row = row - 1;
        int8_t diag_col = 3 - 1;
        for (diag_row, diag_col; diag_row >= 0 && diag_col >= 0; --diag_row, --diag_col)
        {
            if (get_board_value(state, diag_row, diag_col) != center)
                break;

            ++count;
        }

        if (count == 4)
        {
            return true;
        }

        diag_row = row + 1;
        diag_col = 3 + 1;
        for (diag_row, diag_col; diag_row < 6 && diag_col < 7; ++diag_row, ++diag_col)
        {
            if (get_board_value(state, diag_row, diag_col) != center)
                break;

            ++count;

            if (count == 4)
            {
                return true;
            }
        }
    }

    // Diagonal (top-left to bottom-right)
    for (int8_t row = 0; row < 6; ++row)
    {
        uint8_t center = get_board_value(state, row, 3);
        if (center == 0)
            continue;

        uint8_t count = 1;
        int8_t diag_row = row + 1;
        int8_t diag_col = 3 - 1;
        for (diag_row, diag_col; diag_row < 6 && diag_col >= 0; ++diag_row, --diag_col)
        {
            if (get_board_value(state, diag_row, diag_col) != center)
                break;

            ++count;
        }

        if (count == 4)
        {
            return true;
        }

        diag_row = row - 1;
        diag_col = 3 + 1;
        for (diag_row, diag_col; diag_row >= 0 && diag_col < 7; --diag_row, ++diag_col)
        {
            if (get_board_value(state, diag_row, diag_col) != center)
                break;

            ++count;

            if (count == 4)
            {
                return true;
            }
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
        sprintf(buffer, "%d day%s, %d hour%s, %d minute%s, %d second%s",
            days, days > 1 ? "s" : "",
            hours, hours > 1 ? "s" : "",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else if (hours > 0)
    {
        sprintf(buffer, "%d hour%s, %d minute%s, %d second%s",
            hours, hours > 1 ? "s" : "",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else if (minutes > 0)
    {
        sprintf(buffer, "%d minute%s, %d second%s",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else
    {
        sprintf(buffer, "%d second%s",
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

        printf("Total moves: %lu (%f%%), wins found: %lu, dead-ends found: %lu, depth: %d\n", total_moves_evaluated, percent_completed, wins_found, dead_ends_found, min_depth);

        char* friendly_time_string = (char*)malloc(sizeof(char) * 512);
        sprint_friendly_time(time_to_complete, friendly_time_string);
        printf("- Time to complete: %s\n", friendly_time_string);
        free(friendly_time_string);
    }

    BoardState state_new;
    memcpy(state_new, state, sizeof(BoardState));
    *move_possible = make_move(player_this_turn, col, state_new);
    if (*move_possible == false)
    {
        ++dead_ends_found;

        return 0;
    }

    ++total_moves_evaluated;

    bool result = check_for_win(state_new);
    if (result)
    {
        ++wins_found;

        return 0;
    }

    int32_t total_score = 0;
    player_this_turn = player_this_turn % 2 + 1;
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

    return get_move_score(1, 1, 0, state, &move_possible, 1);
}

void sprint_board(BoardState state, char* buffer)
{
    sprintf(buffer, "_1_2_3_4_5_6_7_\n");
    buffer += 16;
    for (int8_t row = 6 - 1; row >= 0; --row)
    {
        sprintf(buffer, "|");
        buffer += 1;
        for (uint8_t col = 0; col < 7; ++col)
        {
            switch (get_board_value(state, row, col))
            {
                case 1:
                    sprintf(buffer, "X");
                    buffer += 1;
                    break;
                case 2:
                    sprintf(buffer, "O");
                    buffer += 1;
                    break;
                default:
                    sprintf(buffer, " ");
                    buffer += 1;
                    break;
            }

            sprintf(buffer, "|");
            buffer += 1;
        }
        sprintf(buffer, "\n");
        buffer += 1;
    }
    sprintf(buffer, "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");
}

void print_board(BoardState state)
{
    char* board = (char*)malloc(sizeof(char) * 256);
    sprint_board(state, board);
    printf("%s", board);
    free(board);
}

bool test_print_board()
{
    BoardState state = { 1, 1, 1, 1, 1, 1, 1 };

    make_move(1, 5, state);
    make_move(2, 0, state);
    make_move(1, 6, state);
    make_move(2, 6, state);
    make_move(1, 0, state);
    make_move(2, 3, state);
    make_move(1, 4, state);
    make_move(2, 1, state);
    make_move(1, 3, state);
    make_move(2, 4, state);
    make_move(1, 5, state);
    make_move(2, 5, state);
    make_move(1, 4, state);
    make_move(2, 5, state);
    make_move(1, 3, state);
    make_move(2, 0, state);
    make_move(1, 1, state);
    make_move(2, 1, state);
    
    const char* correct_board =
        "_1_2_3_4_5_6_7_\n"
        "| | | | | | | |\n"
        "| | | | | | | |\n"
        "| | | | | |O| |\n"
        "|O|O| |X|X|O| |\n"
        "|X|X| |X|O|X|O|\n"
        "|O|O| |O|X|X|X|\n"
        "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n";

    char* test_board = (char*)malloc(sizeof(char) * 256);
    sprint_board(state, test_board);

    bool result = strcmp(correct_board, test_board) == 0;
    free(test_board);
    return result;
}

bool test_print_friendly_time()
{
    char* test_string = (char*)malloc(sizeof(char) * 1024);
    sprint_friendly_time(3412441, test_string);

    bool result = strcmp("39 days, 11 hours, 54 minutes, 1 second", test_string) == 0;
    free(test_string);
    return result;
}