#include <stdbool.h>
#include <stdint.h>

#include "minimax.h"
#include "ticTacToe.h"

#define BOARD_SIZE (TICTACTOE_BOARD_COLUMNS * TICTACTOE_BOARD_ROWS)

// Location of squares on board
#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2

// Define a score type.
typedef int16_t minimax_score_t;

// Define a struct that keeps track of the score associated with a given move
typedef struct {
  tictactoe_location_t move;
  minimax_score_t score;
} moveScore;

// A file-specific global variable that will store the best move after each
// level of minimax
tictactoe_location_t bestChoice;

// The recursive function that returns successive scores associated with
// possible moves
// - Plays out all possible extensions from an initial setup and returns the
// score associated with the best possible outcome.
static minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn) {
  minimax_score_t currentScore = minimax_computeBoardScore(board, !is_Xs_turn);
  moveScore moveScoreTable[BOARD_SIZE];
  uint8_t scoreTable_NextAvailableSlot = 0;

  // if the game is over, return the value of its score
  if (minimax_isGameOver(currentScore)) {
    return currentScore;
  }

  // otherwise we need to iterate through possible outcomes
  // - Run minimax on each possible open square
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
    // identify blank spaces from all rows and columns
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
      // if the square at [row][column] is blank, simulate playing there
      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {
        // Simulate playing there
        board->squares[row][column] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax on new board;
        currentScore = minimax(board, !is_Xs_turn);

        // Record the outcome in the score table.
        moveScoreTable[scoreTable_NextAvailableSlot].score = currentScore;
        moveScoreTable[scoreTable_NextAvailableSlot].move.column = column;
        moveScoreTable[scoreTable_NextAvailableSlot].move.row = row;

        // Undo our theoretical move
        board->squares[row][column] = MINIMAX_EMPTY_SQUARE;

        // Increment the next location for score/move data in the table;
        scoreTable_NextAvailableSlot++;
      }
    }
  }

  minimax_score_t bestScore = 0;
  // Compute what we want to actually return:
  // - If X is playing, record the best choice that gives highest score
  if (is_Xs_turn) {
    bestScore = -15;
    // Comb the table
    for (uint8_t i = 0; i < scoreTable_NextAvailableSlot; i++) {
      // If the current score is higher than past best, record score and
      // location.
      if (moveScoreTable[i].score >= bestScore) {
        bestChoice = moveScoreTable[i].move;
        bestScore = moveScoreTable[i].score;
      }
    }
  }
  // - If O is playing, record the best choice that gives the lowest score
  else {
    bestScore = 15;
    // Comb the table
    for (uint8_t i = 0; i < scoreTable_NextAvailableSlot; i++) {
      // If the current score is lower than past best, record score and
      // location.
      if (moveScoreTable[i].score <= bestScore) {
        bestChoice = moveScoreTable[i].move;
        bestScore = moveScoreTable[i].score;
      }
    }
  }

  return bestScore;
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board and player.
//
// When called from the controlling state machine, you will call this function
// as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  minimax(board, is_Xs_turn);
  return bestChoice;
}

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
// Assumptions:
// (1) if is_Xs_turn == true, the last thing played was an 'X'.
// (2) if is_Xs_turn == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {

  // Determine what type of square we're looking for
  tictactoe_square_state_t playerType = MINIMAX_O_SQUARE;
  minimax_score_t winningScore = MINIMAX_O_WINNING_SCORE;
  //      Assume the last play was an 'O' unless is_Xs_turn == true
  if (is_Xs_turn) {
    playerType = MINIMAX_X_SQUARE;
    winningScore = MINIMAX_X_WINNING_SCORE;
  }

  // One of the rows is a win, return winning score
  if ( // Top row win
      (board->squares[TOP][LFT] == playerType &&
       board->squares[TOP][MID] == playerType &&
       board->squares[TOP][RGT] == playerType) ||
      // Middle row win
      (board->squares[MID][LFT] == playerType &&
       board->squares[MID][MID] == playerType &&
       board->squares[MID][RGT] == playerType) ||
      // Bottom row win
      (board->squares[BOT][LFT] == playerType &&
       board->squares[BOT][MID] == playerType &&
       board->squares[BOT][RGT] == playerType)) {
    return winningScore;
  }

  // One of the columns is a win
  if ( // Left column win
      (board->squares[TOP][LFT] == playerType &&
       board->squares[MID][LFT] == playerType &&
       board->squares[BOT][LFT] == playerType) ||
      // Middle column win
      (board->squares[TOP][MID] == playerType &&
       board->squares[MID][MID] == playerType &&
       board->squares[BOT][MID] == playerType) ||
      // Right column win
      (board->squares[TOP][RGT] == playerType &&
       board->squares[MID][RGT] == playerType &&
       board->squares[BOT][RGT] == playerType)) {
    return winningScore;
  }

  // One of the diagonals is a win
  if ( // Left-to-right diagonal win
      (board->squares[TOP][LFT] == playerType &&
       board->squares[MID][MID] == playerType &&
       board->squares[BOT][RGT] == playerType) ||
      // Right-to-left diagonal win
      (board->squares[TOP][RGT] == playerType &&
       board->squares[MID][MID] == playerType &&
       board->squares[BOT][LFT] == playerType)) {
    return winningScore;
  }

  // All squares are full and no win yet -> draw
  if (board->squares[TOP][LFT] != MINIMAX_EMPTY_SQUARE &&
      board->squares[TOP][MID] != MINIMAX_EMPTY_SQUARE &&
      board->squares[TOP][RGT] != MINIMAX_EMPTY_SQUARE &&
      board->squares[MID][LFT] != MINIMAX_EMPTY_SQUARE &&
      board->squares[MID][MID] != MINIMAX_EMPTY_SQUARE &&
      board->squares[MID][RGT] != MINIMAX_EMPTY_SQUARE &&
      board->squares[BOT][LFT] != MINIMAX_EMPTY_SQUARE &&
      board->squares[BOT][MID] != MINIMAX_EMPTY_SQUARE &&
      board->squares[BOT][RGT] != MINIMAX_EMPTY_SQUARE) {
    return MINIMAX_DRAW_SCORE;
  }
  // Otherwise return NOT_ENDGAME
  return MINIMAX_NOT_ENDGAME;
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {
  board->squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board->squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board->squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board->squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board->squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board->squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board->squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board->squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board->squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  // If the score that we feed in is -1, the game is not over
  if (score == MINIMAX_NOT_ENDGAME) {
    return false;
  }
  // In all other cases it's either a draw or a win or loss -> game is over
  else {
    return true;
  }
}