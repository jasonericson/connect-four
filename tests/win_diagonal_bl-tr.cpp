#include "../core.h"

#include <stdio.h>

struct Game
{
    uint8_t num_moves;
    uint8_t moves[30];
};
Game games[10] = {
    { 24, { 1, 0, 1, 1, 4, 5, 5, 4, 4, 5, 0, 4, 4, 2, 6, 0, 2, 6, 6, 3, 0, 0, 5, 6, }},
    { 24, { 5, 2, 4, 5, 0, 3, 6, 3, 2, 0, 6, 5, 2, 4, 3, 2, 0, 4, 2, 2, 0, 1, 2, 5, }},
    { 20, { 2, 3, 0, 6, 5, 4, 5, 3, 5, 2, 6, 4, 1, 4, 2, 5, 6, 6, 4, 6, }},
    { 19, { 4, 2, 4, 6, 3, 5, 1, 0, 6, 5, 5, 1, 6, 5, 4, 4, 5, 3, 6, }},
    { 21, { 2, 3, 3, 3, 5, 3, 3, 0, 3, 5, 5, 4, 4, 6, 3, 0, 4, 4, 2, 1, 5, }},
    { 14, { 4, 5, 5, 6, 6, 0, 0, 4, 6, 5, 2, 3, 1, 6, }},
    { 21, { 6, 0, 4, 3, 3, 2, 6, 5, 1, 6, 1, 4, 1, 6, 5, 1, 3, 6, 2, 4, 4, }},
    { 16, { 4, 3, 2, 2, 6, 1, 4, 6, 4, 1, 3, 3, 5, 2, 3, 4, }},
    { 15, { 5, 4, 3, 5, 2, 5, 4, 1, 5, 2, 4, 0, 6, 6, 3, }},
    { 23, { 2, 5, 6, 6, 1, 5, 6, 1, 3, 4, 5, 5, 5, 5, 6, 6, 2, 3, 2, 0, 0, 5, 4, }},
};

int main()
{
    init_core();

    for (uint8_t i = 0; i < 10; ++i)
    {
        BoardState state;
        init_board(state);
        uint8_t player = 0;

        int8_t row;
        for (uint8_t j = 0; j < games[i].num_moves; ++j)
        {
            row = make_move(player, games[i].moves[j], state);
            player = 1 - player;
        }

        if (!check_for_win(state, row, games[i].moves[games[i].num_moves - 1]))
        {
            printf("Failed on game %d\n", i);
            print_board(state);
            return 1;
        }
    }

    return 0;
}
