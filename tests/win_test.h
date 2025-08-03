#include <inttypes.h>

struct Game
{
    uint8_t num_moves;
    uint8_t moves[30];
};

bool win_test(Game games[10], bool fail_on_win = false);
