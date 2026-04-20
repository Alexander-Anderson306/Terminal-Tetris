#ifndef GAME_H
#define GAME_H

#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <string.h>
#include "board.h"
#include "piece.h"
#include "terminal.h"

#define USER_TICK_RATE 166667

//its looking like we will need two threads
//one for gravity
//one for user input

void game_loop(Board* board, Piece* piece, Piece* next_piece, int* score);
void* input_thread(void* args);
int check_for_clears_and_score(Board* board, Piece* next_piece, int fall_rate_index);
int update_fall_rate(int score);
double get_time();
#endif
