#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <string.h>
#include "structs_enums_defines.h"

void init_board(Board* board);
void print_board(Board* board, Piece* next_piece);
void copy_board(Board* source, Board* destination);
RGB get_color(enum Piece_Type type);
void copy_rgb(RGB* source, RGB* destination);
char update_board(Board* board, Piece* new_piece, Piece* old_piece);

#endif
