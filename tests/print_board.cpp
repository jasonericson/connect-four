#include "../core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    init_core();

    BoardState state;
    init_board(state);

    make_move_lookup_full(0, 5, state);
    make_move_lookup_full(1, 0, state);
    make_move_lookup_full(0, 6, state);
    make_move_lookup_full(1, 6, state);
    make_move_lookup_full(0, 0, state);
    make_move_lookup_full(1, 3, state);
    make_move_lookup_full(0, 4, state);
    make_move_lookup_full(1, 1, state);
    make_move_lookup_full(0, 3, state);
    make_move_lookup_full(1, 4, state);
    make_move_lookup_full(0, 5, state);
    make_move_lookup_full(1, 5, state);
    make_move_lookup_full(0, 4, state);
    make_move_lookup_full(1, 5, state);
    make_move_lookup_full(0, 3, state);
    make_move_lookup_full(1, 0, state);
    make_move_lookup_full(0, 1, state);
    make_move_lookup_full(1, 1, state);
    
    const char* correct_board =
        "_1_2_3_4_5_6_7_\n"
        "| | | | | | | |\n"
        "| | | | | | | |\n"
        "| | | | | |X| |\n"
        "|X|X| |O|O|X| |\n"
        "|O|O| |O|X|O|X|\n"
        "|X|X| |X|O|O|O|\n"
        "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n";

    char test_board[256];
    sprint_board(state, test_board);

    int result = strcmp(correct_board, test_board);
    if (result != 0)
    {
        printf("Print board failed!\n");
        printf("Expected:\n");
        printf("%s", correct_board);
        printf("Actual:\n");
        printf("%s", test_board);
    }

    return result;
}
