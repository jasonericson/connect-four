#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BoardState
{
    uint64_t player1;
    uint64_t player2;
};

inline bool bit_check(uint64_t num, uint8_t pos)
{
    return (num & ((uint64_t)1 << pos)) != 0;
}

inline uint64_t bit_set(uint64_t num, uint8_t pos)
{
    return num | ((uint64_t)1 << pos);
}

#ifndef NDEBUG
uint8_t get_board_value(BoardState* state, uint8_t row, uint8_t col)
{
    if (row >= 6 || col >= 7)
    {
        printf("Invalid row (%d) or col (%d)\n", row, col);
        return 0;
    }

    uint8_t bit_pos = (row * 7) + col;
    bool player1_value = bit_check(state->player1, bit_pos);
    bool player2_value = bit_check(state->player2, bit_pos);
    if (player1_value && player2_value)
    {
        printf("Player 1 and Player 2 cannot occupy the same space. (row: %d, col: %d)\n", row, col);
        return 0;
    }
    else
    {
        return player1_value ? 1 : (player2_value ? 2 : 0);
    }
}
#endif

#ifndef NDEBUG
void set_board_value(BoardState* state, uint8_t row, uint8_t col, uint8_t value)
{
    if (row >= 6 || col >= 7)
    {
        printf("Invalid row (%d) or col (%d)\n", row, col);
        return;
    }

    uint8_t bit_pos = (row * 7) + col;
    switch (value)
    {
        case 1:
        {
            uint64_t player1 = bit_set(state->player1, bit_pos);
            *state = { .player1 = player1, .player2 = state->player2 };
            return;
        }
        case 2:
        {
            uint64_t player2 = bit_set(state->player2, bit_pos);
            *state = { .player1 = state->player1, .player2 = player2 };
            return;
        }
        default:
            printf("Invalid board value to set (value: %d, row: %d, col: %d)\n", value, row, col);
            return;
    }
}
#endif

bool check_for_win(BoardState* state)
{
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

    // Vertical
    for (uint8_t col = 0; col < 7; ++col)
    {
        uint8_t center = get_board_value(state, 2, col);
        if (center == 0 || center != get_board_value(state, 3, col))
            continue;

        uint8_t count = 2;
        for (int8_t row = 1; row >= 0; --row)
        {
            if (get_board_value(state, row, col) != center)
                break;

            ++count;
        }

        if (count == 4)
        {
            return true;
        }

        for (uint8_t row = 4; row < 6; ++row)
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

bool make_move(uint8_t player, uint8_t column, BoardState* state)
{
    // memcpy(state_new, state_old, sizeof(uint8_t) * 6 * 7);

    if (column < 0 || column >= 7)
        return false;

    if (get_board_value(state, 5, column) != 0)
        return false;

    for (uint8_t row = 0; row < 6; ++row)
    {
        if (get_board_value(state, row, column) == 0)
        {
            set_board_value(state, row, column, player);
            return true;
        }
    }

    return false;
}

// uint64_t wins_found = 0;
// uint64_t dead_ends_found = 0;

// int32_t get_move_score(uint8_t player, uint8_t player_this_turn, uint8_t col, uint8_t (*state)[6][7], bool *move_possible, uint32_t depth)
// {
//     uint8_t state_new[6][7];
//     memcpy(&state_new, state, sizeof(uint8_t) * 6 * 7);
//     *move_possible = make_move(player_this_turn, col, &state_new);
//     if (*move_possible == false)
//     {
//         ++dead_ends_found;
//         printf("Wins found: %d, dead-ends found: %d\r", wins_found, dead_ends_found);
//         return 0;
//     }

//     if (check_for_win(&state_new))
//     {
//         ++wins_found;
//         printf("Wins found: %d, dead-ends found: %d\r", wins_found, dead_ends_found);
//         return player == player_this_turn ? 1 : -1;
//     }

//     int32_t total_score = 0;
//     player_this_turn = player_this_turn % 2 + 1;
//     for (uint8_t col = 0; col < 7; ++col)
//     {
//         bool temp_move_possible;
//         total_score += get_move_score(player, player_this_turn, col, &state_new, &temp_move_possible, depth + 1);
//     }

//     return total_score;
// }

int main()
{
    BoardState state = { .player1 = 0, .player2 = 0 };
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
        // Print board
        printf("_1_2_3_4_5_6_7_\n");
        for (int8_t row = 6 - 1; row >= 0; --row)
        {
            printf("|");
            for (uint8_t col = 0; col < 7; ++col)
            {
                switch (get_board_value(&state, row, col))
                {
                    case 1:
                        printf("X");
                        break;
                    case 2:
                        printf("O");
                        break;
                    default:
                        printf(" ");
                        break;
                }

                printf("|");
            }
            printf("\n");
        }
        printf("‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");

        // Check board for wins
        bool win = check_for_win(&state);

        // Game Over
        if (win)
        {
            printf("Player %d wins!\n", player % 2 + 1);
            break;
        }

        // // Evaluate moves
        // bool move_possible;
        // int32_t move_score = get_move_score(player, player, 0, &state, &move_possible, 1);
        // printf("Column 0 move score: %d\n", move_score);

        // Wait for move
        printf("Player %d, what's your move?\n", player);

        int input;
        scanf("%d", &input);

        bool success = make_move(player, input - 1, &state);
        if (success)
        {
            player = player % 2 + 1;
        }

        printf("\033[2J");
    }

    return 0;
}