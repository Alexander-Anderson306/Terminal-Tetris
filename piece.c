#include "piece.h"
#include "board.h"

int left_rotation[]  = { 0, -1,  1,  0 };
int right_rotation[] = { 0,  1, -1,  0 };

/**
 * Initializes a given piece with a random piece type and corresponding components.
 *
 * The generated piece will have a random piece type, and the corresponding components
 * will be set according to the piece type.
 *
 * @param piece The piece to be initialized.
 */
void init_piece(Piece* piece) {
    //generate random piece type
    enum Piece_Type type = (enum Piece_Type) rand() % 7;
    piece->type = type;

    //may need to fix stuff later here
    switch(type) {
        case LINE:
            //first component
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL - 1;
            //second component pivot
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;
            //third component
            piece->components[2].row = START_ROW;
            piece->components[2].col = START_COL + 1;
            //fourth component
            piece->components[3].row = START_ROW;
            piece->components[3].col = START_COL + 2;
            break;
        case SQUARE:
            //first component
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL + 1;
            //second component pivot
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;
            //third component
            piece->components[2].row = START_ROW + 1;
            piece->components[2].col = START_COL;
            //fourth component
            piece->components[3].row = START_ROW + 1;
            piece->components[3].col = START_COL + 1;
            break;
        case L:
            // left block
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL - 1;

            // pivot center
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;

            // right block
            piece->components[2].row = START_ROW;
            piece->components[2].col = START_COL + 1;

            // lower-right foot
            piece->components[3].row = START_ROW + 1;
            piece->components[3].col = START_COL + 1;
            break;

        case REVERS_L:
            // left block
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL - 1;

            // pivot center
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;

            // right block
            piece->components[2].row = START_ROW;
            piece->components[2].col = START_COL + 1;

            // lower-left foot
            piece->components[3].row = START_ROW + 1;
            piece->components[3].col = START_COL - 1;
            break;
        case Z:
            //first component
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL - 1;
            //second component pivot
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;
            //third component
            piece->components[2].row = START_ROW + 1;
            piece->components[2].col = START_COL;
            //fourth component
            piece->components[3].row = START_ROW + 1;
            piece->components[3].col = START_COL + 1;
            break;
        case REVERS_Z:
            //first component
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL + 1;
            //second component pivot
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;
            //third component
            piece->components[2].row = START_ROW + 1;
            piece->components[2].col = START_COL;
            //fourth component
            piece->components[3].row = START_ROW + 1;
            piece->components[3].col = START_COL - 1;
            break;
        case T:
            //first component
            piece->components[0].row = START_ROW;
            piece->components[0].col = START_COL - 1;
            //second component rotate around this one
            piece->components[1].row = START_ROW;
            piece->components[1].col = START_COL;
            //third component
            piece->components[2].row = START_ROW + 1;
            piece->components[2].col = START_COL;
            //fourth component
            piece->components[3].row = START_ROW;
            piece->components[3].col = START_COL + 1;
            break;
        default:
            fprintf(stderr, "Invalid piece type generated\n");
            exit(1);
            break;
    }
}

/**
 * Copies the components of a given piece from a source piece to a destination piece.
 * 
 * @param source The piece to copy from.
 * @param destination The piece to copy to.
 */
void copy_piece(Piece* source, Piece* destination) {
    for (int i = 0; i < 4; i++) {
        destination->components[i].row = source->components[i].row;
        destination->components[i].col = source->components[i].col;
    }
    destination->type = source->type;
}

/**
 * Rotate a given piece by a given direction.
 *
 *
 * @param piece The piece to be rotated.
 * @param direction The direction to rotate the piece, either 'a' or 'd'.
 */
void rotate_piece(Piece* piece, char direction) {
    // we use the second component as the pivot component
    int pivot_row = piece->components[1].row;
    int pivot_col = piece->components[1].col;

    //select the rotation matrix
    int *R = NULL;
    if (direction == ROTATE_LEFT) {
        R = left_rotation;
    } else if (direction == ROTATE_RIGHT) {
        R = right_rotation;
    } else {
        return;
    }

    //ai helped me figure this out ngl
    for (int i = 0; i < 4; i++) {
        int row = piece->components[i].row;
        int col = piece->components[i].col;

        //local coords relative to pivot
        int dx = col - pivot_col;
        int dy = row - pivot_row;

        //rotate (dx, dy) using matrix R
        int new_dx = dx * R[0] + dy * R[1];
        int new_dy = dx * R[2] + dy * R[3];

        //convert back to board coords
        piece->components[i].col = pivot_col + new_dx;
        piece->components[i].row = pivot_row + new_dy;
    }
}

/**
 * Move a given piece in a given direction.
 *
 * @param piece The piece to be moved.
 * @param direction The direction to move the piece, either 'a' for left or 'd' for right.
 */
void move_piece(Piece* piece, Board* board, char direction) {
    char stop_flag = 0;
    switch(direction) {
        case LEFT:
            piece->components[0].col--;
            piece->components[1].col--;
            piece->components[2].col--;
            piece->components[3].col--;
            break;
        case RIGHT:
            piece->components[0].col++;
            piece->components[1].col++;
            piece->components[2].col++;
            piece->components[3].col++;
            break;
        case DROP:
            while(!stop_flag) {
                //check if the piece can move down again
                for (int i = 0; i < 4; i++) {

                    int row = piece->components[i].row;
                    int col = piece->components[i].col;

                    //look at the cell below
                    int below_row = row + 1;
                    int below_col = col;

                    char below = board->character_board[below_row][below_col];

                    //if the cell below is not empty make sure its not part of the same piece
                    if (below != EMPTY_SPACE) {
                        char same_piece = 0;

                        for (int j = 0; j < 4; j++) {
                            if (piece->components[j].row == below_row &&
                                piece->components[j].col == below_col) {
                                same_piece = 1;
                                break;
                            }
                        }

                        if (!same_piece) {
                            stop_flag = 1;
                        }
                    }
                }

                //move the piece down
                if(!stop_flag) {
                    piece->components[0].row++;
                    piece->components[1].row++;
                    piece->components[2].row++;
                    piece->components[3].row++;
                }
            }

            //push the piece back up one sloppy fix for bug
            piece->components[0].row--;
            piece->components[1].row--;
            piece->components[2].row--;
            piece->components[3].row--;
            break;
        default:
            fprintf(stderr, "Invalid direction generated\nDirection: %c\n", direction);
            exit(1);
            break;
    }
}



/**
 * Move a given piece down one row.
 * This is used to simulate gravity in the game.
 *
 * @param piece The piece to be moved down.
 */
void gravity_tick(Piece* piece) {
    //move the piece down
    piece->components[0].row++;
    piece->components[1].row++;
    piece->components[2].row++;
    piece->components[3].row++;
}
