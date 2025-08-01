#include "../core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    init_core();

    BoardState state;
    init_board(state);

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
