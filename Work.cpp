// cs_snake.c
// Written by <ZHIRUI ZHAO> <Z5640918> on <21/10/2025>
//
// Description: <INSERT DESCRIPTION OF PROGRAM>

// Provided Libraries
#include <stdio.h>

// Add your own #include statements below this line


// Provided constants
#define ROWS 10 
#define COLS 10

#define NO_SNAKE -1


// Add your own #define constants below this line
#define POINTS_PER_APPLE 5
#define POINTS_PER_REVERSE_APPLE 10
#define GAME_STATE_PLAYING 0
#define GAME_STATE_WON 1
#define GAME_STATE_LOST 2
#define MAX_SNAKE_LEN (ROWS * COLS)
// Provided enums
// Enum for features on the game board
enum entity {
    BODY_SEGMENT,
    EXIT_LOCKED,
    EXIT_UNLOCKED,
    WALL,
    APPLE_NORMAL,
    APPLE_REVERSE,
    APPLE_SPLIT,
    APPLE_EXPLODE,
    EXPLOSION,
    PASSAGE_UP,
    PASSAGE_DOWN,
    PASSAGE_LEFT,
    PASSAGE_RIGHT,
    PORTAL,
    EMPTY
};

// Add your own enums below this line

// Provided structs
// Represents a tile/cell on the game board
struct tile {
    enum entity entity;
};

// Add your own structs below this line
struct game_stats {
    int total_points;
    int moves_made;
    int num_apples_eaten;
    int num_initial_apples;
};
struct pos {
    int row;
    int col;
};

// Provided Function Prototypes
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS], 
    int snake_row, 
    int snake_col
);
void print_game_statistics(
    int points,
    int moves_made,
    int num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
);
void print_game_statistics_with_rival(
    int original_points,
    int original_moves_made,
    int original_num_apples_eaten,
    int rival_points,
    int rival_moves_made,
    int rival_num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
);
void print_board_line(void);
void print_tile_spacer(void);
void print_board_header(void);

// Add your function prototypes below this line
int is_position_valid(int row, int col); 
int is_tile_empty(struct tile board[ROWS][COLS], int row, int col);
void check_and_add_long_wall(struct tile board[ROWS][COLS]);
void print_game_stats(struct tile board[ROWS][COLS], struct game_stats *stats);
int calculate_apples_remaining(struct tile board[ROWS][COLS]);
int snake_movement(
    struct tile board[ROWS][COLS], 
    char command, 
    int *snake_row, 
    int *snake_col,
    struct game_stats *stats,
    struct pos snake_body[],
    int *snake_length
);
void check_and_unlock_exits(struct tile board[ROWS][COLS]);
int calculate_max_points_remaining(struct tile board[ROWS][COLS]);

// Provided sample main() function (you will need to modify this)
int main(void) {
    struct game_stats stats = {0};
    printf("Welcome to CS Snake!\n\n");
    struct tile board[ROWS][COLS];
    initialise_board(board);
    printf("--- Map Setup ---\n");
    char command = '\0';
    int row, col;
    char apple_type;
    while (scanf(" %c", &command) == 1) {
        if (command == 's') {
            break;
        } else if (command == 'W') {
            check_and_add_long_wall(board);    
        } else if (command == 'w') {
            scanf("%d %d", &row, &col);
            if (!is_position_valid(row, col)) {
                printf("ERROR: Invalid position, %d %d is out of bounds!\n", 
                row, col);
            } else if (!is_tile_empty(board, row, col)) {
                printf("ERROR: Invalid tile, %d %d is occupied!\n", row, col);
            } else{
                board[row][col].entity = WALL;
            }
        } else if (command == 'e') {
            scanf("%d %d", &row, &col);
            if (!is_position_valid(row, col)) {
                printf("ERROR: Invalid position, %d %d is out of bounds!\n", 
                row, col);
            } else if (!is_tile_empty(board, row, col)) {
                printf("ERROR: Invalid tile, %d %d is occupied!\n", row, col);
            } else{    
                board[row][col].entity = EXIT_LOCKED; 
            }
        } else if (command == 'a') {
            scanf(" %c %d %d", &apple_type, &row, &col);
            if (!is_position_valid(row, col)) {
                printf("ERROR: Invalid position, %d %d is out of bounds!\n",
                row, col);
            } else if (!is_tile_empty(board, row, col)) {
                printf("ERROR: Invalid tile, %d %d is occupied!\n", row, col);
            } else {
                if (apple_type == 'n') { 
                    board[row][col].entity = APPLE_NORMAL;
                    stats.num_initial_apples = stats.num_initial_apples + 1 ; 
                } else if (apple_type == 'r') { 
                    board[row][col].entity = APPLE_REVERSE; 
                    stats.num_initial_apples = stats.num_initial_apples + 1 ;
                }
            }
        }
    }     
    print_board(board, NO_SNAKE, NO_SNAKE);
    printf("--- Spawning Snake ---\n");
    int snake_row = NO_SNAKE;
    int snake_col = NO_SNAKE;
    struct pos snake_body[MAX_SNAKE_LEN];
    int snake_length = 0;
    while (snake_row == NO_SNAKE) { 
        printf("Enter the snake's starting position: ");
        if (scanf("%d %d", &row, &col) != 2) {
            break; 
        }
        if (!is_position_valid(row, col)) {
            printf("ERROR: Invalid position, %d %d is out of bounds!\n", row, col);
        } else if (!is_tile_empty(board, row, col)) {
            printf("ERROR: Invalid tile, %d %d is occupied!\n", row, col);
        } else {
            snake_row = row;
            snake_col = col;
            snake_body[0].row = snake_row; 
            snake_body[0].col = snake_col;
            snake_length = 1;
        }
    }
    int game_state = GAME_STATE_PLAYING;
    if (snake_row != NO_SNAKE) {
        print_board(board, snake_row, snake_col);
        check_and_unlock_exits(board);
        printf("--- Gameplay Phase ---\n");
        while (game_state == GAME_STATE_PLAYING && scanf(" %c", &command) == 1) {
            if (command == 'p') {
                print_game_stats(board, &stats);
            }
            else if (command == 'w' || command == 'a' || command == 's' || command == 'd') {
                game_state = snake_movement(board, command, &snake_row, &snake_col, &stats, snake_body, &snake_length);
                if (game_state == GAME_STATE_PLAYING) {
                    check_and_unlock_exits(board);
                }
                print_board(board, snake_row, snake_col);
            }
        }
    }
    if (game_state == GAME_STATE_WON) {
        printf("--- Game Over ---\n");
        printf("Ssslithered out with a full stomach!\n");
        print_game_stats(board, &stats);
    } else if (game_state == GAME_STATE_LOST) {
        printf("--- Game Over ---\n");
        printf("Guessss I was the prey today.\n");
        print_game_stats(board, &stats);
    } else {
        printf("--- Quitting Game ---\n");
    }
    return 0;
}

int snake_movement(
    struct tile board[ROWS][COLS], 
    char command, 
    int *snake_row, 
    int *snake_col,
    struct game_stats *stats,
    struct pos snake_body[],
    int *snake_length
) {
    int old_row = *snake_row;
    int old_col = *snake_col;
    int new_row = old_row;
    int new_col = old_col;
    
    if (command == 'w') {
        new_row = old_row - 1; 
    } else if (command == 's') {
        new_row = old_row + 1; 
    } else if (command == 'a') {
        new_col = old_col - 1; 
    } else if (command == 'd') {
        new_col = old_col + 1; 
    }
    
    stats->moves_made = stats->moves_made + 1;
    
    // Check if new position is valid
    if (!is_position_valid(new_row, new_col)) {
        return GAME_STATE_LOST;
    }
    
    enum entity target_entity = board[new_row][new_col].entity;
    
    // Check win condition
    if (target_entity == EXIT_UNLOCKED) {
        return GAME_STATE_WON;
    }
    
    // Check collision
    if (target_entity == WALL || target_entity == BODY_SEGMENT || target_entity == EXIT_LOCKED) {
        return GAME_STATE_LOST; 
    }
    
    // Check apple type
    int ate_normal_apple = (target_entity == APPLE_NORMAL);
    int ate_reverse_apple = (target_entity == APPLE_REVERSE);

    if (ate_normal_apple) {
        stats->total_points += POINTS_PER_APPLE; 
        stats->num_apples_eaten++;
        board[new_row][new_col].entity = EMPTY;
    } else if (ate_reverse_apple) {
        stats->total_points += POINTS_PER_REVERSE_APPLE; 
        stats->num_apples_eaten++;
        board[new_row][new_col].entity = EMPTY;
    }
    
    // Handle reverse apple differently
    if (ate_reverse_apple) {
        // Clear old body from board
        for (int i = 0; i < *snake_length; i++) {
            if (is_position_valid(snake_body[i].row, snake_body[i].col)) {
                board[snake_body[i].row][snake_body[i].col].entity = EMPTY;
            }
        }
        
        // Reverse the entire snake body (swap head and tail)
        int start = 0;
        int end = *snake_length - 1;
        while (start < end) {
            struct pos temp = snake_body[start];
            snake_body[start] = snake_body[end];
            snake_body[end] = temp;
            start = start + 1;
            end = end - 1;
        }
        
        // Update head position to the new head after reversal (old tail)
        *snake_row = snake_body[0].row;
        *snake_col = snake_body[0].col;
        
        // Draw new body on board
        for (int i = 1; i < *snake_length; i++) {
            if (is_position_valid(snake_body[i].row, snake_body[i].col)) {
                board[snake_body[i].row][snake_body[i].col].entity = BODY_SEGMENT;
            }
        }
    } else {
        // Normal movement - always grow the snake
        // Shift body segments to make room
        for (int i = *snake_length; i > 0; i--) {
            if (i < MAX_SNAKE_LEN) {
                snake_body[i] = snake_body[i - 1];
            }
        }
        
        // Add new head position
        snake_body[0].row = new_row;
        snake_body[0].col = new_col;
        *snake_row = new_row; 
        *snake_col = new_col;
        
        // Always increase length
        if (*snake_length < MAX_SNAKE_LEN) { 
            (*snake_length)++; 
        }
        
        // Draw body segments on board
        for (int i = 1; i < *snake_length; i++) {
            if (is_position_valid(snake_body[i].row, snake_body[i].col)) {
                board[snake_body[i].row][snake_body[i].col].entity = BODY_SEGMENT;
            }
        }
    }
    
    return GAME_STATE_PLAYING;
}

int calculate_apples_remaining(struct tile board[ROWS][COLS]) {
    int remaining = 0;
    int r = 0;
    while (r < ROWS) {
        int c = 0;
        while (c < COLS) {
            if (board[r][c].entity == APPLE_NORMAL ||
                board[r][c].entity == APPLE_REVERSE ||
                board[r][c].entity == APPLE_SPLIT ||
                board[r][c].entity == APPLE_EXPLODE) {
                remaining = remaining + 1;
            }
            c = c + 1;
        }
        r = r + 1;
    }
    return remaining;
}

int calculate_max_points_remaining(struct tile board[ROWS][COLS]) {
    int max_points = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (board[r][c].entity == APPLE_NORMAL) {
                max_points += POINTS_PER_APPLE; 
            } else if (board[r][c].entity == APPLE_REVERSE) {
                max_points += POINTS_PER_REVERSE_APPLE; 
            }
        }
    }
    return max_points;
}

void print_game_stats(struct tile board[ROWS][COLS], struct game_stats *stats) {
    int num_remaining = calculate_apples_remaining(board);
    double completion_percentage = 0.0;
    if (stats->num_initial_apples > 0) {
        completion_percentage = 100.0 * stats->num_apples_eaten / stats->num_initial_apples;
    } else {
        completion_percentage = 100.0;
    }
    int max_points_remaining = calculate_max_points_remaining(board);
    
    print_game_statistics(
        stats->total_points,
        stats->moves_made,
        stats->num_apples_eaten,
        num_remaining,
        completion_percentage,
        max_points_remaining
    );
}

void check_and_unlock_exits(struct tile board[ROWS][COLS]) {
    int apples_remaining = 0;
    int r = 0;
    while (r < ROWS) {
        int c = 0;
        while (c < COLS) {
            if (board[r][c].entity == APPLE_NORMAL ||
                board[r][c].entity == APPLE_REVERSE ||
                board[r][c].entity == APPLE_SPLIT ||
                board[r][c].entity == APPLE_EXPLODE) {
                
                apples_remaining = 1;
                break; 
            }
            c = c + 1;
        }
        if (apples_remaining) {
            break; 
        }
        r = r + 1;
    }
    if (!apples_remaining) {
        r = 0; 
        while (r < ROWS) {
            int c = 0; 
            while (c < COLS) {
                if (board[r][c].entity == EXIT_LOCKED) {
                    board[r][c].entity = EXIT_UNLOCKED;
                }
                c = c + 1;
            }
            r = r + 1;
        }
    }
}

// Add your function definitions below this line
int is_position_valid(int row, int col) {
    return row < ROWS && row >= 0 && col >= 0 && col < COLS;
}

int is_tile_empty(struct tile board[ROWS][COLS], int row, int col) {
    return board[row][col].entity == EMPTY;
}

void check_and_add_long_wall(struct tile board[ROWS][COLS]) {
    char direction;
    int start_row, start_col, length;
    if (scanf(" %c %d %d %d", &direction, &start_row, &start_col, &length) 
    != 4) {
        return;
    }
    if (!is_position_valid(start_row, start_col)) {
        printf("ERROR: Invalid position, %d %d is out of bounds!\n", 
        start_row, start_col);
        return;
    }
    int i = 0;
    while (i < length) {
        int current_row = start_row;
        int current_col = start_col;
        if (direction == 'h') {
            current_col += i;
        } else if (direction == 'v') {
            current_row += i;
        }
        if (!is_position_valid(current_row, current_col)) {
            printf("ERROR: Invalid position, part of the wall is out of bounds!\n");
            return;
        }
        i = i + 1;
    }        
    i = 0;
    while (i < length) {
        int current_row = start_row;
        int current_col = start_col;

        if (direction == 'h') {
            current_col += i;
        } else if (direction == 'v') {
            current_row += i;
        }
        if (!is_tile_empty(board, current_row, current_col)) {
            printf("ERROR: Invalid tile, part of the wall is occupied!\n");
            return; 
        }
        i = i + 1;
    }
    i = 0;
    while (i < length) {
        int final_row = start_row;
        int final_col = start_col;

        if (direction == 'h') {
            final_col += i;
        } else if (direction == 'v') {
            final_row += i;
        }
        board[final_row][final_col].entity = WALL;
        i = i + 1;
    }
}        

// =============================================================================
// EDITABLE Provided Functions
// =============================================================================

// Given a 2D board array, initialise all tiles to EMPTY.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = EMPTY;
        }
    }
}

// Prints the game board, showing the snake's head position on the board.
void print_board(
    struct tile board[ROWS][COLS], 
    int snake_row, 
    int snake_col
) {
    print_board_line();
    print_board_header();
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        print_tile_spacer();
        for (int col = 0; col < COLS; col++) {
            printf(" ");
            struct tile tile = board[row][col];
            if (row == snake_row && col == snake_col) {
                printf("^~^");
            } else if (tile.entity == WALL) {
                printf("|||");
            } else if (tile.entity == BODY_SEGMENT) {
                printf("###");
            } else if (tile.entity == EXIT_LOCKED) {
                printf("[X]");
            } else if (tile.entity == EXIT_UNLOCKED) {
                printf("[ ]");
            } else if (tile.entity == APPLE_NORMAL) {
                printf("(`)");
            } else if (tile.entity == APPLE_REVERSE) {
                printf("(R)");
            } else if (tile.entity == APPLE_SPLIT) {
                printf("(S)");
            } else if (tile.entity == PASSAGE_UP) {
                printf("^^^");
            } else if (tile.entity == PASSAGE_DOWN) {
                printf("vvv");
            } else if (tile.entity == PASSAGE_LEFT) {
                printf("<<<");
            } else if (tile.entity == PASSAGE_RIGHT) {
                printf(">>>");
            } else if (tile.entity == PORTAL) {
                printf("~O~");
            } else if (tile.entity == EXPLOSION) {
                printf("***");
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
    print_tile_spacer();
}

// =============================================================================
// DO NOT EDIT Provided Functions 
// =============================================================================

// Prints statistics about the game
void print_game_statistics(
    int points,
    int moves_made,
    int num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("============ Game Statistics ============\n");
    printf("Totals:\n");
    printf("  - Points: %d\n", points);
    printf("  - Moves Made: %d\n", moves_made);
    printf("  - Number of Apples Eaten: %d\n", num_apples_eaten);
    printf("Completion:\n");
    printf("  - Number of Apples Remaining: %d\n", num_apples_remaining);
    printf("  - Apple Completion Percentage: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("=========================================\n");
}

// Prints statistics about the game for both snakes when there are two players
void print_game_statistics_with_rival(
    int original_points,
    int original_moves_made,
    int original_num_apples_eaten,
    int rival_points,
    int rival_moves_made,
    int rival_num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("============ Game Statistics ============\n");
    printf("Original Snake Totals:\n");
    printf("  - Points: %d\n", original_points);
    printf("  - Moves Made: %d\n", original_moves_made);
    printf("  - Number of Apples Eaten: %d\n", original_num_apples_eaten);
    printf("Rival Snake Totals:\n");
    printf("  - Points: %d\n", rival_points);
    printf("  - Moves Made: %d\n", rival_moves_made);
    printf("  - Number of Apples Eaten: %d\n", rival_num_apples_eaten);
    printf("Completion:\n");
    printf("  - Number of Apples Remaining: %d\n", num_apples_remaining);
    printf("  - Apple Completion Percentage: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("=========================================\n");
}

// Helper function for print_board().
void print_board_header(void) {
    printf("|            C S _ S N A K E            |\n");
}

// Helper function for print_board(). 
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}

// Helper function for print_board(). 
void print_tile_spacer(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("   +");
    }
    printf("\n");
}