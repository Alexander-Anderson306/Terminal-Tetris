#ifndef PIECE_H
#define PIECE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structs_enums_defines.h"

void init_piece(Piece* piece);
void copy_piece(Piece* source, Piece* destination);
void rotate_piece(Piece* piece, char direction);
void move_piece(Piece* piece, Board* board, char direction);
void gravity_tick(Piece* piece);

#endif
