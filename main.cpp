#include "core.h"

#include <cassert>
#include <math.h>
#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    init_core();

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

        int64_t final_scores[7];
        get_move_scores(final_scores);

        // uint64_t eval_end_time = SDL_GetPerformanceCounter();
        // printf("Time to finish evaluation: %ld\n", (eval_end_time - eval_start_time) / 1000);
        // printf("Column 0 move score: %d\n", move_score);

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