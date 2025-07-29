#include "../core.h"

struct Game
{
    uint8_t num_moves;
    uint8_t moves[30];
};
Game games[10] = {
    { 5, { 0, 1, 0, 1, 1, }},
    { 19, { 4, 0, 6, 1, 6, 0, 2, 4, 3, 6, 0, 1, 0, 0, 3, 6, 1, 3, 6, }},
    { 12, { 0, 1, 2, 0, 2, 1, 5, 5, 4, 1, 5, 6, }},
    { 7, { 6, 3, 3, 3, 6, 3, 1, }},
    { 12, { 0, 3, 6, 6, 2, 5, 4, 4, 0, 1, 5, 0, }},
    { 17, { 4, 4, 1, 0, 2, 4, 5, 0, 2, 1, 4, 0, 4, 4, 2, 4, 0, }},
    { 15, { 1, 1, 4, 6, 6, 1, 6, 4, 0, 6, 0, 0, 4, 4, 0, }},
    { 7, { 1, 0, 4, 5, 2, 1, 6, }},
    { 4, { 4, 0, 1, 2, }},
    { 13, { 5, 0, 3, 4, 2, 0, 6, 3, 1, 1, 0, 1, 5, }},
};

int main()
{
    for (uint8_t i = 0; i < 10; ++i)
    {
        BoardState state = { 1, 1, 1, 1, 1, 1, 1 };
        uint8_t player = 1;

        for (uint8_t j = 0; j < games[i].num_moves; ++j)
        {
            make_move(player, games[i].moves[j], state);
            player = 3 - player;
        }

        if (check_for_win(state))
        {
            return 1;
        }
    }

    return 0;
}
