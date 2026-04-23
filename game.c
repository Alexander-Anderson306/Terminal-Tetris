#include "game.h"
#include "piece.h"
//in terms of how many frames pass before scene update
#ifdef HARD_MODE
int fall_rate[] = {60 * 0.8, 60 * 0.6, 60 * 0.5, 60 * 0.4, 60 * 0.3, 60 * 0.2};
#else
int fall_rate[] = {60 * 1.5, 60, 60 * 0.8, 60 * 0.6, 60 * 0.5, 60 * 0.4};
#endif
double frame_rate = 1.0/60.0;
_Atomic char game_over = 0;
//this mutex will be used to lock access to the board
static pthread_mutex_t board_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    //seed random
    usleep(1000);
    srand(time(NULL));
    //set raw mode
    set_raw_mode();

    Board board;
    init_board(&board);
    Piece piece;
    //starting with a z or reverse z sucks
    do {
        init_piece(&piece);   
    } while(piece.type == Z || piece.type == REVERS_Z);
    Piece next_piece;
    init_piece(&next_piece);
    int score = 0;
    //put the piece on the board and start the game loop
    update_board(&board, &piece, NULL);

    //initial frame
    print_board(&board, &next_piece);
    printf("Score: %d\n", 0);
    printf("\033[%dA", ROWS + 1);

    //user input thread
    pthread_t input_tid;
    Thread_Args args = (Thread_Args) {&board, &piece, &next_piece, &score};
    pthread_create(&input_tid, NULL, input_thread, &args);

    game_loop(&board, &piece, &next_piece, &score);

    //past here the game is over 

    //rejoin the input thread and kill mutexs
    pthread_join(input_tid, NULL);
    pthread_mutex_destroy(&board_mutex);
    pthread_mutex_destroy(&print_mutex);


    reset_terminal();

    //print the final score
    printf("\033[%dB", ROWS);
    printf("Final Score: %d\n", score);
    return 0;
}

/**
 * This function controles the tetris game logic. Delta time is used to set the fall rate.
**/
void game_loop(Board* board, Piece* piece, Piece* next_piece, int* score) {
    double last_time = get_time();
    double now = 0;
    double delta_time = 0;
    int current_frame = 0;
    int fall_rate_index = 0;
    char result = 0;
    Board copy;
    while(!game_over) {
        now = get_time();
        delta_time = now - last_time;

        //update frame
        if(delta_time >= frame_rate) {
            ++current_frame;
            last_time = now;
        } else {
            continue;
        }

        if(current_frame >= fall_rate[fall_rate_index]) {
            pthread_mutex_lock(&board_mutex);
            current_frame = 0;
            //save a copy of the last piece for proper update and board
            Piece temp_piece;
            copy_piece(piece, &temp_piece);
            Board backup_board;
            copy_board(board, &backup_board);

            //move the piece down
            gravity_tick(&temp_piece);

            //update the board
            result = update_board(board, &temp_piece, piece);

            if(result == 0) {
                //gravity move succeeded
                copy_piece(&temp_piece, piece);
            } else if(result == 1) {
                //piece landed
                copy_piece(next_piece, piece);
                init_piece(next_piece);
                //check for clears
                char score_updated = 1;
                int old_score = *score;
                //handle clears update the score (and handle when pieces fall in place and need to be cleared again)
                while(score_updated) {
                    *score += check_for_clears_and_score(board, next_piece, fall_rate_index);
                    score_updated = *score != old_score;
                    old_score = *score;
                }
                result = update_board(board, piece, NULL);
            } else {
                //invalid move load the old board
                copy_board(&backup_board, board);
            }

            //check for a game end
            if(result == 2 && (piece->components[0].row == 1 || piece->components[1].row == 1 || 
                piece->components[2].row == 1 || piece->components[3].row == 1)) {
                game_over = 1;
            }

            //take a copy of the board for printing
            copy_board(board, &copy);
            //unlock access to the board
            pthread_mutex_unlock(&board_mutex);

            pthread_mutex_lock(&print_mutex);
            print_board(&copy, next_piece);
            printf("Score: %d\n", *score);
            printf("\033[%dA", ROWS + 1);
            pthread_mutex_unlock(&print_mutex);
            fall_rate_index = update_fall_rate(*score);
        }
    }
}


/**
 * This function is a thread that is responsible for handling user input.
 * It takes two arguments, a pointer to a Board and a pointer to a Piece.
 * It will continuously read user input and update the board accordingly.
 * If the user presses 'q', the game_over variable will be set to 1 and the thread will exit.
 * If the user presses a valid movement key (left, right, down), the thread will update the board accordingly.
 * If the user presses a valid rotation key (left or right), the thread will update the piece accordingly.
 * If the update is invalid, the thread will do nothing.
 * If the update is valid, but the board needs a new piece, the thread will set the need_new_piece variable to 1.
 */
void* input_thread(void* args) {
    //the argument is the board and the current piece we are working with
    Thread_Args* thread_args = (Thread_Args*) args;
    Board* board = thread_args->board;
    Piece* piece = thread_args->piece;
    Piece* next_piece = thread_args->next_piece;
    int* score = thread_args->score;
    char input;

    char garbage1 = 0;
    char garbage2 = 0;
    double last_time = get_time();
    double now = 0;
    double delta_time = 0;
    while(!game_over) {
        now = get_time();
        delta_time = now - last_time;
        if(delta_time >=frame_rate) {
            last_time = now;
            if (read(STDIN_FILENO, &input, 1) > 0) {
                //TO LOWER CASE
                if (input >= 'A' && input <= 'Z') {
                    input += 'a' - 'A';
                //could be a quit or could be an arrow key (if its an arrow key we gotta clear the buffer)
                } else if (input == 27) {
                    read(STDIN_FILENO, &garbage1, 1);
                    read(STDIN_FILENO, &garbage2, 1);
                }

                //
                pthread_mutex_lock(&board_mutex);

                //save temp copies to validate input before committing
                Piece temp_piece;
                copy_piece(piece, &temp_piece);

                Board temp_board;
                copy_board(board, &temp_board);

                if (input == QUIT) {
                    game_over = 1;
                    pthread_mutex_unlock(&board_mutex);
                    return NULL;
                }

                //apply movement/rotation to temp_piece first
                if (input == ROTATE_LEFT || input == ROTATE_RIGHT) {
                    rotate_piece(&temp_piece, input);
                } 
                else if (input == LEFT || input == RIGHT || input == DROP) {
                    move_piece(&temp_piece, board, input);
                }

                //test the update on temp state
                char result = update_board(&temp_board, &temp_piece, piece);

                if (result == 0 || result == 1) {
                    //valid update commit piece and board
                    copy_piece(&temp_piece, piece);
                    copy_board(&temp_board, board);
                }
                //else: invalid (result == 2) do nothing (old piece and board remain)
                Board copy;
                copy_board(board, &copy);
                pthread_mutex_unlock(&board_mutex);

                pthread_mutex_lock(&print_mutex);
                print_board(&copy, next_piece);
                printf("Score: %d\n", *score);
                printf("\033[%dA", ROWS + 1);
                pthread_mutex_unlock(&print_mutex);
            }
        }
    }
    return NULL;
}

int check_for_clears_and_score(Board* board, Piece* next_piece, int fall_rate_index) {
    //NOTE: we do rows and cols 1 to length-1 because we dont need to worry about the rim of the board

    char rows[ROWS];
    //find row indexs of rows we need to clear
    //tally up the score as well
    int score = 0;
    int num_cleared_in_a_row = 0;
    for(int i = 1; i < ROWS-1; i++) {
        char clear_flag = 1;
        for(int j = 1; j < COLS-1; j++) {
            if(board->character_board[i][j] == EMPTY_SPACE) {
                clear_flag = 0;
                break;
            }
        }

        if(clear_flag) {
            rows[i] = 1;
            ++num_cleared_in_a_row;
            //get the right score
            switch(num_cleared_in_a_row) {
                case 1:
                    score += 100;
                    break;
                case 2:
                    score += 250;
                    break;
                case 3:
                    score += 500;
                    break;
                case 4:
                    score += 1000;
                    break;
                //just in case shenanigans occur
                default:
                    score += 100;
                    break;
            }
        } else {
            rows[i] = 0;
            num_cleared_in_a_row = 0;
        }
    }

    //we can return early if there are no clears
    if(!score) {
        return score;
    }

    //make a copy of the board with the flashed rows state
    Board flashed_board;
    copy_board(board, &flashed_board);

    RGB grey = get_color((enum Piece_Type) -1);
    //fill the filled rows with flashing characters
    for(int i = 1; i < ROWS-1; i++) {
        if(rows[i]) {
            for(int j = 1; j < COLS-1; j++) {
                flashed_board.character_board[i][j] = FLASHING_COMPONENT;
                flashed_board.color_board[i][j].r = grey.r;
                flashed_board.color_board[i][j].g = grey.g;
                flashed_board.color_board[i][j].b = grey.b;
            }
        }
    }

    //now clear the rows (flash them 3 times then remove them)
    int num_flashes = 0;
    pthread_mutex_lock(&print_mutex);
    double last_time = get_time();
    double now = 0;
    double delta_time = 0;
    while(num_flashes < 3){
        now = get_time();
        delta_time = now - last_time;
        //loop waits to print by 1/4 fall rate
        while(delta_time <= fall_rate[fall_rate_index] * frame_rate/4.0) {
            now = get_time();
            delta_time = now - last_time;
        }
        last_time = now;
        //flash the pieces that are getting deleted
        print_board(board, next_piece);
        printf("Score: %d\n", score);
        printf("\033[%dA", ROWS + 1);
        now = get_time();
        delta_time = now - last_time;
        //loop waits to print by 1/4 fall rate
        while(delta_time <= fall_rate[fall_rate_index] * frame_rate/4.0) {
            now = get_time();
            delta_time = now - last_time;
        }
        last_time = now;
        print_board(&flashed_board, next_piece);
        printf("Score: %d\n", score);
        printf("\033[%dA", ROWS + 1);
        ++num_flashes;
        }

    //remove the cleared rows and update the board
    for(int i = 1; i < ROWS-1; i++) {
        if(rows[i]) {
            for(int j = 1; j < COLS-1; j++) {
                board->character_board[i][j] = EMPTY_SPACE;
                board->color_board[i][j].r = grey.r;
                board->color_board[i][j].g = grey.g;
                board->color_board[i][j].b = grey.b;
            }
        }
    }

    //print the cleared board to show the deletion
    print_board(board, next_piece);
    printf("Score: %d\n", score);
    printf("\033[%dA", ROWS + 1);

    //give a half tickrate break
    delta_time = 0;
    last_time = get_time();
    while(delta_time >= fall_rate[fall_rate_index] * frame_rate/2.0) {
        now = get_time();
        delta_time = now - last_time;
    } 

    pthread_mutex_unlock(&print_mutex);

    //if we are in easy mode update the board such that pieces fall as far as they can
    #ifdef EASY_MODE
    //update the board (move all the components that can move down down)
    for(int i = ROWS - 2; i > 1; i--) {
        for(int j = 1; j < COLS-1; j++) {
            //skip unmarked spaces
            if(board->character_board[i][j] == EMPTY_SPACE) {
                continue;
            }

            //move the piece down until it hits something
            int new_row = i;
            while(new_row + 1 < ROWS - 1 && board->character_board[new_row+1][j] == EMPTY_SPACE) ++new_row;
            //move the piece if we have to
            if(new_row != i) {
                RGB color;
                copy_rgb(&board->color_board[i][j], &color);
                board->character_board[new_row][j] = PIECE_COMPONENT;
                board->color_board[new_row][j] = color;
                board->character_board[i][j] = EMPTY_SPACE;
                board->color_board[i][j] = grey;
            }
        }
    }

    //if we are not in easy mode update the board such that rows fill in and pieces dont fall
    #else
    int write_row = ROWS - 2;
    for (int i = ROWS - 2; i > 0; i--) {
        //check if the current row is empty
        int empty = 1;
        for (int j = 1; j < COLS - 1; j++) {
            if (board->character_board[i][j] != EMPTY_SPACE) {
                empty = 0;
                break;
            }
        }

        //if the current row is not empty, write it to the write row
        if (!empty) {
            if (write_row != i) {
                for(int j = 1; j < COLS-1; j++) {
                    //swap rows
                    char tmp = board->character_board[i][j];
                    board->character_board[i][j] = board->character_board[write_row][j];
                    board->character_board[write_row][j] = tmp;

                    RGB tmp2 = board->color_board[i][j];
                    board->color_board[i][j] = board->color_board[write_row][j];
                    board->color_board[write_row][j] = tmp2;
                }
            }

            write_row--;
        }
    }


    //fill in the empty rows
    for (int i = write_row; i > 0; i--) {
        for (int j = 1; j < COLS - 1; j++) {
            board->character_board[i][j] = EMPTY_SPACE;
            board->color_board[i][j] = grey;
        }
    }

    #endif

    //print the new board
    pthread_mutex_lock(&print_mutex);
    print_board(board, next_piece);
    printf("Score: %d\n", score);
    printf("\033[%dA", ROWS + 1);
    pthread_mutex_unlock(&print_mutex);
    //all done return the score
    return score;
}

/**
 * Returns the index of the fall rate based on score.
 *
 * The fall tick rate determines how often the piece should fall down one row.
 * The higher the score, the faster the piece should fall.
 *
 *
 * @param score The current score.
 * @return The updated fall tick rate.
 */
int update_fall_rate(int score) {
    if(score < 1000) {
        return 0;
    } else if(score < 2500) {
        return 1;
    } else if(score < 6000) {
        return 2;
    } else if(score < 10000) {
        return 3;
    } else if(score < 20000) {
        return 4;
    } else {
        return 5;
    }
}

/**
    * This function returns the current time in seconds with nanosecond persision.
**/
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}
