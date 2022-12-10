#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "buttons.h"
#include "minimax.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"

#define INIT_STATE_WAIT_TIME 2 // Wait 3 second before game starts at beginning
#define RESET_STATE_WAIT_TIME                                                  \
  2 // Wait 3 seconds for player select before playing
#define INIT_MESSAGE                                                           \
  "   Touch board to play X\n\r          -OR-\n\r   wait for the "             \
  "computer\n\r      & play O."
#define INIT_MESSAGE_X 0
#define INIT_MESSAGE_Y (DISPLAY_HEIGHT * 2 / 5)
#define INIT_TEXT_SIZE 2
#define ACTUALLY_PRINT false
#define ERASE true
#define IT_WAS_Xs_TURN true
#define IT_WAS_Os_TURN false
#define RESET_COUNTER 0
#define PLAYER_REALLY_IS_X true
#define COMPUTER_PLAYS_NEXT true

// *******************************************
#define DEBUG_MODE_ACTIVE 0
// DEBUG STATE MESSAGES
#define INIT_STATE_MESSAGE "init state\n"
#define INIT_WAIT_MESSAGE "INIT WAIT state\n"
#define RESET_MESSAGE "reset state\n"
#define RESET_WAIT_MESSAGE "RESET WAIT state\n"
#define COMP_MOVE_MESSAGE "comp move state\n"
#define PLAYER_MOVE_MESSAGE "PLAYER_MOVE state\n"
#define DRAW_O_MESSAGE "DRAW_O state\n"
#define DRAW_X_MESSAGE "DRAW_X state\n"
#define GAME_OVER_MESSAGE "GAME over state\n"
#define ERROR_ST_MESSAGE "!~~~YOU'RE IN THE ERROR STATE~~~!"
// *******************************************

static bool player_is_x;
static bool is_player_turn;
static uint8_t initStateWait = RESET_COUNTER;
static uint8_t initWaitTicks = RESET_COUNTER;
static uint8_t resetStateWait = RESET_COUNTER;
static uint8_t resetWaitTicks = RESET_COUNTER;
static tictactoe_location_t move;
static tictactoe_board_t theBoard;
static enum ticTacToeState {
  INIT,
  INIT_WAIT,
  RESET_BOARD,
  RESET_WAIT,
  COMP_MOVE,
  PLAYER_MOVE,
  DRAW_O,
  DRAW_X,
  GAME_OVER
} currentState = INIT;

// This is a helper function to print or erase the default text from the screen
// "erase" parameter tells the function whether to write in black or in green.
static void initTextPrint(bool erase) {
  // If erase is true, draw in blue
  if (erase) {
    display_setTextColor(DISPLAY_DARK_BLUE);
  }
  // Otherwise, let's draw it in green.
  else {
    display_setTextColor(DISPLAY_GREEN);
  }
  display_setCursor(INIT_MESSAGE_X, INIT_MESSAGE_Y);
  display_setTextSize(INIT_TEXT_SIZE);
  display_print(INIT_MESSAGE);
}

// This is a helper function that will clear away old symbols and reset the
// board values
static void EraseBoardSymbols() {
  for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
    // Comb through rows and columns of board
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      tictactoe_location_t currentLocation;
      currentLocation.row = row;
      currentLocation.column = column;
      // Erase any X already on the board
      if (theBoard.squares[row][column] == MINIMAX_X_SQUARE) {
        ticTacToeDisplay_drawX(currentLocation, ERASE);
        theBoard.squares[row][column] = MINIMAX_EMPTY_SQUARE;
      }
      // Erase any O already on the board
      else if (theBoard.squares[row][column] == MINIMAX_O_SQUARE) {
        ticTacToeDisplay_drawO(currentLocation, ERASE);
        theBoard.squares[row][column] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }
}

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
static void Print_State_Message() {
  static enum ticTacToeState previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case INIT:
      printf(INIT_STATE_MESSAGE);
      break;
    case INIT_WAIT:
      printf(INIT_WAIT_MESSAGE);
      break;
    case RESET_BOARD:
      printf(RESET_MESSAGE);
      break;
    case RESET_WAIT:
      printf(RESET_WAIT_MESSAGE);
      break;
    case PLAYER_MOVE:
      printf(PLAYER_MOVE_MESSAGE);
      break;
    case COMP_MOVE:
      printf(COMP_MOVE_MESSAGE);
      break;
    case DRAW_X:
      printf(DRAW_X_MESSAGE);
      break;
    case DRAW_O:
      printf(DRAW_O_MESSAGE);
      break;
    case GAME_OVER:
      printf(GAME_OVER_MESSAGE);
      break;
    default:
      printf(ERROR_ST_MESSAGE);
      break;
    }
  }
}

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s) {
  // Initialize the drivers for the hardware
  buttons_init();
  display_init();
  touchscreen_init(period_s);

  display_fillScreen(DISPLAY_DARK_BLUE);
  initTextPrint(ACTUALLY_PRINT);

  initWaitTicks = INIT_STATE_WAIT_TIME / period_s;
  resetWaitTicks = RESET_STATE_WAIT_TIME / period_s;
}

// Tick the tic-tac-toe controller state machine
void ticTacToeControl_tick() {
  // Print a debug message if we're in the debug mode
  if (DEBUG_MODE_ACTIVE) {
    Print_State_Message();
  }
  // Transition state machine
  switch (currentState) {
  case INIT:
    currentState = INIT_WAIT;
    break;
  case INIT_WAIT:
    // Move to the reset state if the timer expires
    if (initStateWait == initWaitTicks) {
      initStateWait = RESET_COUNTER;
      currentState = RESET_BOARD;
    }
    break;
  case RESET_BOARD:
    currentState = RESET_WAIT;
    break;
  case RESET_WAIT:
    // If the time expires with no input, start play with computer as X
    if (resetWaitTicks == resetStateWait) {
      resetStateWait = RESET_COUNTER;
      currentState = COMP_MOVE;
      player_is_x = !PLAYER_REALLY_IS_X;
      is_player_turn = !COMPUTER_PLAYS_NEXT;
    }
    // If player makes a move, record it and play as X
    else if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      resetStateWait = RESET_COUNTER;
      touchscreen_ack_touch();
      move = ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());
      theBoard.squares[move.row][move.column] = MINIMAX_X_SQUARE;
      currentState = DRAW_X;
      player_is_x = PLAYER_REALLY_IS_X;
      is_player_turn = COMPUTER_PLAYS_NEXT;
    }
    break;
  case COMP_MOVE:
    // Record the move that the computer made in the overall board, then...
    // If the computer is X, go to draw X with past move, now recorded
    if (!player_is_x) {
      theBoard.squares[move.row][move.column] = MINIMAX_X_SQUARE;
      currentState = DRAW_X;
    }
    // Otherwise, draw our O on the board.
    else {
      theBoard.squares[move.row][move.column] = MINIMAX_O_SQUARE;
      currentState = DRAW_O;
    }
    break;
  case PLAYER_MOVE:
    // Stay in this state until the player has made a move on a legal square
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      tictactoe_location_t temp_move =
          ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());
      // Verify that the move was a legal move before proceeding
      if (theBoard.squares[temp_move.row][temp_move.column] ==
          MINIMAX_EMPTY_SQUARE) {
        move = temp_move;
        // Record move information, then transition to the appropriate state to
        // draw their move
        if (player_is_x) {
          theBoard.squares[move.row][move.column] = MINIMAX_X_SQUARE;
          currentState = DRAW_X;
        }
        // Transition option two:
        else {
          theBoard.squares[move.row][move.column] = MINIMAX_O_SQUARE;
          currentState = DRAW_O;
        }
      }
    }
    break;
  case DRAW_O:
    // If game over, go to game over
    if (minimax_isGameOver(
            minimax_computeBoardScore(&theBoard, IT_WAS_Os_TURN))) {
      currentState = GAME_OVER;
    }
    // If it's now the player's turn, go to their state
    else if (is_player_turn) {
      currentState = PLAYER_MOVE;
    }
    // Otherwise, go to computer's turn
    else {
      currentState = COMP_MOVE;
    }
    break;
  case DRAW_X: {
    // If game over, go to game over
    minimax_score_t boardScore =
        minimax_computeBoardScore(&theBoard, IT_WAS_Xs_TURN);
    if (minimax_isGameOver(boardScore)) {
      currentState = GAME_OVER;
    }
    // If it's now the player's turn, go to their state
    else if (is_player_turn) {
      currentState = PLAYER_MOVE;
    }
    // Otherwise, go to computer's turn
    else {
      currentState = COMP_MOVE;
    }
    break;
  }
  case GAME_OVER:
    // If btn0 is being pressed, reset the game
    if ((buttons_read() & BUTTONS_BTN0_MASK) == BUTTONS_BTN0_MASK) {
      currentState = RESET_BOARD;
    }
    break;
  default:
    printf(ERROR_ST_MESSAGE);
    break;
  }
  // Action state machine
  switch (currentState) {
  case INIT:
    initWaitTicks = RESET_COUNTER;
    break;
  case INIT_WAIT:
    initStateWait++;
    break;
  case RESET_BOARD:
    initTextPrint(ERASE);
    EraseBoardSymbols();
    ticTacToeDisplay_init();
    break;
  case RESET_WAIT:
    resetStateWait++;
    break;
  case COMP_MOVE:
    move = minimax_computeNextMove(&theBoard, !player_is_x);
    break;
  case PLAYER_MOVE:
    // State action depends on player
    break;
  case DRAW_O:
    is_player_turn = !is_player_turn;
    ticTacToeDisplay_drawO(move, ACTUALLY_PRINT);
    break;
  case DRAW_X:
    is_player_turn = !is_player_turn;
    ticTacToeDisplay_drawX(move, ACTUALLY_PRINT);
    break;
  case GAME_OVER:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
    }
    break;
  default:
    printf(ERROR_ST_MESSAGE);
    break;
  }
}