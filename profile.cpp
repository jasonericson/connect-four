#include "core.h"

#include <math.h>
#include "SDL.h"
#include "SDL_timer.h"
#include <stdio.h>
#include <stdlib.h>

struct Game
{
    uint8_t num_moves;
    uint8_t* moves;
};

int main()
{
    const uint64_t NUM_GAMES = 10000000;
    const uint8_t MAX_NUM_MOVES = 30;

    uint64_t total_num_moves = 0;
    Game* games = (Game*)malloc(sizeof(Game) * NUM_GAMES);
    for (uint64_t i = 0; i < NUM_GAMES; ++i)
    {
        games[i].num_moves = (uint8_t)SDL_rand(MAX_NUM_MOVES) + 1;
        games[i].moves = (uint8_t*)malloc(sizeof(uint8_t) * games[i].num_moves);
        for (uint8_t j = 0; j < games[i].num_moves; ++j)
        {
            games[i].moves[j] = (uint8_t)SDL_rand(7);
        }
        total_num_moves += games[i].num_moves;
    }

    BoardState* states = (BoardState*)malloc(sizeof(BoardState) * NUM_GAMES);
    for (uint64_t i = 0; i < NUM_GAMES; ++i)
    {
        for (uint8_t j = 0; j < 7; ++j)
        {
            states[i][j] = 1;
        }
    }

    uint64_t make_move_start_time = SDL_GetPerformanceCounter();
    for (uint64_t i = 0; i < NUM_GAMES; ++i)
    {
        uint8_t player = 1;
        for (uint8_t j = 0; j < games[i].num_moves; ++j)
        {
            make_move(player, games[i].moves[j], states[i]);
            player = 3 - player;
        }
    }
    uint64_t make_move_end_time = SDL_GetPerformanceCounter();

    double_t make_move_total_time_ns = (double_t)(make_move_end_time - make_move_start_time) / (SDL_GetPerformanceFrequency() / 1000000000);
    double_t make_move_avg_time_ns = make_move_total_time_ns / total_num_moves;
    double_t make_move_num_per_us = 1000 / make_move_avg_time_ns;
    printf("make_move(): %f times / μs. (total time to make %lu moves: %f ms\n", make_move_num_per_us, total_num_moves, make_move_total_time_ns / 1000000);

    bool* wins = (bool*)malloc(sizeof(bool) * NUM_GAMES); // basically just to keep the compiler from optimizing out check_for_win
    uint64_t check_for_win_start_time = SDL_GetPerformanceCounter();
    for (uint64_t i = 0; i < NUM_GAMES; ++i)
    {
        wins[i] = check_for_win(states[i]);
    }
    uint64_t check_for_win_end_time = SDL_GetPerformanceCounter();

    double_t check_for_win_total_time_ns = (double_t)(check_for_win_end_time - check_for_win_start_time) / (SDL_GetPerformanceFrequency() / 1000000000);
    double_t check_for_win_avg_time_ns = check_for_win_total_time_ns / NUM_GAMES;
    double_t check_for_win_num_per_us = 1000 / check_for_win_avg_time_ns;
    printf("check_for_win(): %f times / μs. (total time to check %lu games: %f ms\n", check_for_win_num_per_us, NUM_GAMES, check_for_win_total_time_ns / 1000000);
}
