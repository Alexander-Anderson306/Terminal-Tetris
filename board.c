#include "board.h"

/**
 * Initializes a given board with the appropriate characters.
 *
 * @param board The board to be initialized.
 */
void init_board(Board* board) {
    //initialize top and bottom row
    RGB color = get_color((enum Piece_Type) -1);
    for(int i = 0; i < COLS; i++) {
        board->character_board[0][i] = TOP_BOTTEM_EDGE;
        board->character_board[ROWS - 1][i] = TOP_BOTTEM_EDGE;
        board->color_board[0][i].r = color.r;
        board->color_board[0][i].g = color.g;
        board->color_board[0][i].b = color.b;
        board->color_board[ROWS - 1][i].r = color.r;
        board->color_board[ROWS - 1][i].g = color.g;
        board->color_board[ROWS - 1][i].b = color.b;
    }

    //initialize the rest of the board
    for(int i = 1; i < ROWS-1; i++) {
        for(int j = 0; j < COLS; j++) {
            if(j == 0 || j == COLS - 1) {
                board->character_board[i][j] = LEFT_RIGHT_EDGE;
            } else {
                board->character_board[i][j] = EMPTY_SPACE;
            }
            board->color_board[i][j].r = color.r;
            board->color_board[i][j].g = color.g;
            board->color_board[i][j].b = color.b;
        }
    }
}

/**
 * Prints a given board to the console.
 *
 * This function prints the character_board to the console, using ANSI escape
 * codes to set the color of each character to the corresponding color in the
 * color_board. The board is printed row by row, with each row on a new line.
 *
 * @param board The board to be printed.
 */
void print_board(Board* board, Piece* next_piece) {
    //rgb of the next peice
    RGB next_rgb = get_color(next_piece->type);
    RGB grey = get_color(-1);
    int right_offset = COLS * 2 + 5;

    //starting position saved
    printf("\x1b[s");

    //moving to the right of where the board will be
    printf("\x1b[%dC", right_offset);
    printf("Next:\n");
    //print the top part of the box
    printf("\x1b[%dC", right_offset);
    printf("- - - -\n");
    for (int i = START_ROW; i < START_ROW + 2; i++) {
        printf("\x1b[%dC", right_offset);
        //print the side of the box
        printf("\x1b[38;2;%d;%d;%dm%c",
        grey.r,
        grey.g,
        grey.b,
        LEFT_RIGHT_EDGE);
        for (int j = START_COL-2; j < START_COL + 3; j++) {
            if(next_piece->components[0].col == j && next_piece->components[0].row == i) {
                printf("\x1b[38;2;%d;%d;%dm%c",
                next_rgb.r,
                next_rgb.g,
                next_rgb.b,
                PIECE_COMPONENT);
            } else if(next_piece->components[1].col == j && next_piece->components[1].row == i) {
                printf("\x1b[38;2;%d;%d;%dm%c",
                next_rgb.r,
                next_rgb.g,
                next_rgb.b,
                PIECE_COMPONENT);
            } else if(next_piece->components[2].col == j && next_piece->components[2].row == i) {
                printf("\x1b[38;2;%d;%d;%dm%c",
                next_rgb.r,
                next_rgb.g,
                next_rgb.b,
                PIECE_COMPONENT);
            } else if(next_piece->components[3].col == j && next_piece->components[3].row == i) {
                printf("\x1b[38;2;%d;%d;%dm%c",
                next_rgb.r,
                next_rgb.g,
                next_rgb.b,
                PIECE_COMPONENT);
            } else {
                printf(" ");
            }

        }
        //print the end of the box
        printf(" ");
        printf("\x1b[38;2;%d;%d;%dm%c",
        grey.r,
        grey.g,
        grey.b,
        LEFT_RIGHT_EDGE);
        printf("\r\n");
    }
    //print the bottom of the board
    
    printf("\x1b[%dC", right_offset);
    printf("- - - -");
    //Restoring origional cursor position
    printf("\x1b[u");              

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("\x1b[38;2;%d;%d;%dm%c ",
                board->color_board[i][j].r,
                board->color_board[i][j].g,
                board->color_board[i][j].b,
                board->character_board[i][j]);
        }
        printf("\x1b[0m\n");
    }
}

/**
 * Copies the character_board and color_board of a given source board to a given destination board.
 * 
 *
 * @param source The board to copy from.
 * @param destination The board to copy to.
 */
void copy_board(Board* source, Board* destination) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            destination->character_board[i][j] = source->character_board[i][j];
            destination->color_board[i][j].r = source->color_board[i][j].r;
            destination->color_board[i][j].g = source->color_board[i][j].g;
            destination->color_board[i][j].b = source->color_board[i][j].b;
        }
    }
}

/**
 * Returns an RGB struct containing the color corresponding to the given
 * PieceType. If the PieceType
 *
 *
 * @param type The PieceType to get the color for.
 * @return An RGB struct containing the color corresponding to the given PieceType.
 */
RGB get_color(enum Piece_Type type) {
    switch (type) {
        case LINE:
            return (RGB){3, 248, 252};
        case SQUARE:
            return (RGB){244, 252, 3};
        case L:
            return (RGB){252, 190, 3};
        case REVERS_L:
            return (RGB){232, 90, 237};
        case Z:
            return (RGB){86, 252, 3};
        case REVERS_Z:
            return (RGB){252, 19, 3};
        case T:
            return (RGB){128, 0, 255};
        default:
            return (RGB){193, 199, 201};
    }
}

/**
 * Copies the RGB values from a given source to a given destination.
 *
 * @param source The RGB values to copy from.
 * @param destination The RGB values to copy to.
 */
void copy_rgb(RGB* source, RGB* destination) {
    destination->r = source->r;
    destination->g = source->g;
    destination->b = source->b;
}

/**
 * Updates a given board by removing an old piece and adding a new piece.
 *
 *
 * @param board The board to be updated.
 * @param new_piece The new piece to be added to the board.
 * @param old_piece The old piece to be removed from the board.
 * @return 1 if the piece is valid but cannot move down any further, 2 if the piece is invalid.
 */
char update_board(Board* board, Piece* new_piece, Piece* old_piece) {
    //get rid of the old piece
    RGB grey = get_color((enum Piece_Type) -1);
    if(old_piece != NULL){
        for(int i = 0; i < 4; i++) {
            board->character_board[old_piece->components[i].row][old_piece->components[i].col] = EMPTY_SPACE;
            board->color_board[old_piece->components[i].row][old_piece->components[i].col].r = grey.r;
            board->color_board[old_piece->components[i].row][old_piece->components[i].col].g = grey.g;
            board->color_board[old_piece->components[i].row][old_piece->components[i].col].b = grey.b;
        }
    }

    //color of the piece
    RGB color = get_color(new_piece->type);
    //add new peice
    //return 1 if piece is valid, but cannot move down any further
    //return 2 if the piece is invalid
    for(int i = 0; i < 4; i++) {
        if(board->character_board[new_piece->components[i].row][new_piece->components[i].col] == EMPTY_SPACE) {
            board->character_board[new_piece->components[i].row][new_piece->components[i].col] = PIECE_COMPONENT;
            board->color_board[new_piece->components[i].row][new_piece->components[i].col].r = color.r;
            board->color_board[new_piece->components[i].row][new_piece->components[i].col].g = color.g;
            board->color_board[new_piece->components[i].row][new_piece->components[i].col].b = color.b;
        } else {
            return 2;
        }
    }

    //check if the piece can move down again
    for (int i = 0; i < 4; i++) {

        int row = new_piece->components[i].row;
        int col = new_piece->components[i].col;

        //look at the cell below
        int below_row = row + 1;
        int below_col = col;

        char below = board->character_board[below_row][below_col];

        //if the cell below is not empty make sure its not part of the same piece
        if (below != EMPTY_SPACE) {
            char same_piece = 0;

            for (int j = 0; j < 4; j++) {
                if (new_piece->components[j].row == below_row &&
                    new_piece->components[j].col == below_col) {
                    same_piece = 1;
                    break;
                }
            }

            if (!same_piece) {
                return 1;
            }
        }
    }

    return 0;
}
