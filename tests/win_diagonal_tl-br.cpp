#include "../core.h"

#include <stdio.h>

struct Game
{
    uint8_t num_moves;
    uint8_t moves[30];
};
Game games[10] = {
    { 24, { 5, 4, 6, 3, 1, 3, 1, 1, 5, 6, 2, 1, 6, 2, 2, 6, 0, 2, 0, 4, 0, 3, 6, 0, }},
    { 20, { 3, 3, 1, 2, 3, 3, 0, 4, 4, 0, 1, 6, 3, 4, 2, 2, 0, 4, 5, 5, }},
    { 26, { 2, 4, 1, 2, 4, 0, 2, 0, 2, 2, 6, 2, 2, 3, 0, 0, 2, 2, 1, 6, 6, 2, 5, 5, 3, 1, }},
    { 22, { 2, 4, 2, 2, 6, 1, 1, 6, 4, 5, 1, 2, 3, 3, 0, 0, 6, 4, 6, 6, 0, 1, }},
    { 25, { 6, 0, 3, 5, 4, 2, 0, 5, 4, 6, 4, 4, 5, 4, 1, 4, 3, 4, 2, 1, 2, 3, 1, 5, 1, }},
    { 21, { 6, 3, 1, 0, 4, 6, 5, 1, 6, 4, 5, 6, 1, 0, 1, 3, 0, 3, 3, 2, 4, }},
    { 21, { 5, 3, 0, 2, 1, 1, 0, 4, 1, 0, 1, 6, 3, 1, 6, 0, 6, 2, 2, 2, 0, }},
    { 19, { 4, 2, 2, 5, 0, 1, 0, 2, 5, 0, 3, 1, 0, 0, 6, 3, 6, 5, 1, }},
    { 22, { 6, 4, 0, 0, 1, 5, 2, 4, 1, 1, 0, 2, 5, 2, 6, 3, 5, 6, 1, 6, 3, 0, }},
    { 28, { 0, 0, 0, 4, 1, 2, 2, 5, 0, 5, 4, 4, 3, 1, 0, 4, 4, 3, 4, 6, 3, 6, 3, 5, 6, 4, 5, 3, }},
};

int main()
{
    init_core();

    for (uint8_t i = 0; i < 10; ++i)
    {
        BoardState state;
        init_board(state);
        uint8_t player = 1;

        int8_t row;
        for (uint8_t j = 0; j < games[i].num_moves; ++j)
        {
            row = make_move(player, games[i].moves[j], state);
            player = 3 - player;
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
