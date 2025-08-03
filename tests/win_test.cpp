#include "win_test.h"
#include "../core.h"

#include <stdio.h>

bool win_test(Game games[10], bool fail_on_win /* = false */)
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

        if (fail_on_win == check_for_win(state, row, games[i].moves[games[i].num_moves - 1]))
        {
            printf("Failed on game %d\n", i);
            print_board(state);
            return false;
        }
    }

    return true;
}

/*

          fow
      true | false
      ------------
    t|  t  |  f  |
cfw  |------------
    f|  f  |  t  |
      ------------
*/