#ifndef STRUCTS_ENUMS_DEFINES_H
#define STRUCTS_ENUMS_DEFINES_H

#define ROWS 22
#define COLS 12
#define TOP_BOTTEM_EDGE '-'
#define LEFT_RIGHT_EDGE '|'
#define EMPTY_SPACE ' '

typedef struct RGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;

typedef struct Board {
    char character_board[ROWS][COLS];
    RGB color_board[ROWS][COLS];
} Board;

#define PIECE_COMPONENT '#'
//when a line gets filled in, the line will flash with DELETING_COMPONENT once then it gets deleted
#define FLASHING_COMPONENT '@'

#define START_ROW 1
#define START_COL 5

enum Piece_Type {
    LINE,
    SQUARE,
    L,
    REVERS_L,
    Z,
    REVERS_Z,
    T
};

enum Button_Press {
    DROP = 's',
    LEFT = 'a',
    RIGHT = 'd',
    ROTATE_LEFT = 'q',
    ROTATE_RIGHT = 'e',
    //ESC = 27
    QUIT = 27
};

typedef struct Piece_Component {
    int row;
    int col;
} Piece_Component;

typedef struct Piece {
    Piece_Component components[4];
    enum Piece_Type type;
} Piece;

typedef struct Thread_Args {
    Board* board;
    Piece* piece;
    Piece* next_piece;
    int* score;
} Thread_Args;

#endif
