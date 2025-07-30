#include "../core.h"

#include <stdio.h>

struct Game
{
    uint8_t num_moves;
    uint8_t moves[30];
};
Game games[10] = {
    { 17, { 6, 3, 2, 0, 4, 0, 6, 5, 5, 5, 1, 6, 5, 6, 3, 6, 4, }},
    { 25, { 6, 6, 0, 0, 2, 2, 3, 2, 3, 5, 3, 2, 4, 3, 5, 4, 3, 0, 5, 0, 6, 4, 0, 4, 1, }},
    { 15, { 5, 0, 1, 0, 2, 0, 3, 6, 5, 5, 2, 3, 3, 2, 4, }},
    { 9, { 2, 1, 4, 2, 3, 6, 2, 3, 5, }},
    { 9, { 3, 1, 4, 1, 5, 1, 0, 4, 2, }},
    { 16, { 5, 1, 6, 4, 6, 1, 1, 0, 4, 2, 0, 2, 0, 4, 0, 3, }},
    { 13, { 6, 5, 0, 4, 4, 6, 2, 3, 3, 0, 5, 1, 2, }},
    { 15, { 4, 1, 2, 1, 4, 2, 0, 2, 1, 4, 2, 2, 3, 4, 5, }},
    { 11, { 2, 0, 5, 1, 0, 3, 3, 0, 2, 6, 1, }},
    { 16, { 0, 6, 5, 2, 3, 2, 6, 3, 1, 6, 4, 0, 5, 3, 4, 1, }},
};

int main()
{
    for (uint8_t i = 0; i < 10; ++i)
    {
        BoardState state;
        init_board(&state);
        uint8_t player = 1;

        int8_t row;
        for (uint8_t j = 0; j < games[i].num_moves; ++j)
        {
            row = make_move(player, games[i].moves[j], &state);
            player = 3 - player;
        }

        if (!check_for_win(&state, row, games[i].moves[games[i].num_moves - 1]))
        {
            printf("Failed on game %d\n", i);
            print_board(&state);
            return 1;
        }
    }

    return 0;
}
