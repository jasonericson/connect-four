#include <cassert>
#include <math.h>
#include "SDL/include/SDL3/SDL.h"
#include "SDL/include/SDL3/SDL_timer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REGISTER_PROFILE(label) \
    uint64_t label##_time_spent = 0; \
    uint64_t label##_count = 0;
#define START_PROFILE(label) \
    ++label##_count; \
    uint64_t label##_time_start = SDL_GetPerformanceCounter();
#define END_PROFILE(label) \
    label##_time_spent += (SDL_GetPerformanceCounter() - label##_time_start);
#define PRINT_PROFILE_INFO(label) print_profile_info(#label, label##_count, label##_time_spent)

typedef uint8_t BoardState[7];

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
    // assert(result >= 0 && result < 3);

    // // return bit_check(state->player1, bit_pos) ? 1 : (bit_check(state->player2, bit_pos) ? 2 : 0);
    // return bit_check(state->player1, bit_pos) + bit_check(state->player2, bit_pos) * 2;
}

bool make_move(uint8_t player, uint8_t column, BoardState state)
{
    assert(column >= 0 && column < 7);

    if (state[column] >= 0b01000000)
        return false;

    uint8_t probe = 0b01000000;
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

REGISTER_PROFILE(get_move_score)
REGISTER_PROFILE(make_move)
REGISTER_PROFILE(check_for_win)

void print_profile_info(const char* label, uint64_t count, uint64_t time_spent)
{
    double_t total_time_seconds = (double)time_spent / SDL_GetPerformanceFrequency();
    double_t total_time_percentage = 100.0 * total_time_seconds / ((double)get_move_score_time_spent / SDL_GetPerformanceFrequency());
    double_t avg_time_nanoseconds = ((double)time_spent / count) / (SDL_GetPerformanceFrequency() / 1000000000);

    printf("-- %s: count = %lu, total time = %f seconds (%f%%), avg time = %f nanoseconds\n",
        label, count, total_time_seconds, total_time_percentage, avg_time_nanoseconds);
}

int32_t get_move_score(uint8_t player, uint8_t player_this_turn, uint8_t col, BoardState state, bool *move_possible, uint8_t depth)
{
    uint64_t time_current = SDL_GetPerformanceCounter();
    double_t seconds_since_last_print = (double_t)(time_current - time_of_last_print) / (double_t)SDL_GetPerformanceFrequency();
    if (seconds_since_last_print >= 1.0)
    {
        time_of_last_print = time_current;
        printf("Total moves: %lu (%%%f), wins found: %lu, dead-ends found: %lu, depth: %d\n", total_moves_evaluated, 100.0f * (float)total_moves_evaluated / (float)4531985219092, wins_found, dead_ends_found, min_depth);

        double_t get_move_score_total_time = (double)get_move_score_time_spent / SDL_GetPerformanceFrequency();
        double_t get_move_score_avg_time = ((double)get_move_score_time_spent / get_move_score_count) / (SDL_GetPerformanceFrequency() / 1000000000);

        printf("- get_move_score(): count = %lu, total time = %f seconds, avg time = %f nanoseconds\n", get_move_score_count, get_move_score_total_time, get_move_score_avg_time);

        PRINT_PROFILE_INFO(check_for_win);
        PRINT_PROFILE_INFO(make_move);
    }

    START_PROFILE(get_move_score)

    BoardState state_new;
    memcpy(state_new, state, sizeof(BoardState));
    START_PROFILE(make_move)
    *move_possible = make_move(player_this_turn, col, state_new);
    END_PROFILE(make_move)
    if (*move_possible == false)
    {
        ++dead_ends_found;

        END_PROFILE(get_move_score)

        return 0;
    }

    ++total_moves_evaluated;

    START_PROFILE(check_for_win)
    bool result = check_for_win(state_new);
    END_PROFILE(check_for_win)
    if (result)
    {
        ++wins_found;

        END_PROFILE(get_move_score)

        return 0;
    }

    END_PROFILE(get_move_score)

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

    return strcmp(correct_board, test_board) == 0;
}

int main()
{
    SDL_Init(0);

    assert(test_print_board());

    BoardState state = { 1, 1, 1, 1, 1, 1, 1 };
    uint8_t player = 1;

    // for (uint8_t row = 0; row < 6; ++row)
    // {
    //     for (uint8_t col = 0; col < 7; ++col)
    //     {
    //         state[row][col] = rand() % 3;
    //     }
    // }

    while (true)
    {
        // // Print board
        // printf("_1_2_3_4_5_6_7_\n");
        // for (int8_t row = 6 - 1; row >= 0; --row)
        // {
        //     printf("|");
        //     for (uint8_t col = 0; col < 7; ++col)
        //     {
        //         switch (get_board_value(state, row, col))
        //         {
        //             case 1:
        //                 printf("X");
        //                 break;
        //             case 2:
        //                 printf("O");
        //                 break;
        //             default:
        //                 printf(" ");
        //                 break;
        //         }

        //         printf("|");
        //     }
        //     printf("\n");
        // }
        // printf("‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");

        // // Check board for wins
        // bool win = check_for_win(&state);

        // // Game Over
        // if (win)
        // {
        //     printf("Player %d wins!\n", player % 2 + 1);
        //     break;
        // }

        // Evaluate moves
        bool move_possible;
        uint64_t eval_start_time = SDL_GetPerformanceCounter();
        time_of_last_print = eval_start_time;
        int64_t move_score = get_move_score(player, player, 0, state, &move_possible, 1);
        // uint64_t eval_end_time = SDL_GetPerformanceCounter();
        // printf("Time to finish evaluation: %ld\n", (eval_end_time - eval_start_time) / 1000);
        printf("Column 0 move score: %ld\n", move_score);

        // // Wait for move
        // printf("Player %d, what's your move?\n", player);

        // int input;
        // scanf("%d", &input);

        // bool success = make_move(player, input - 1, &state);
        // if (success)
        // {
        //     player = player % 2 + 1;
        // }

        // printf("\033[2J");
    }

    return 0;
}