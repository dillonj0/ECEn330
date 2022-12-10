/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "minimax.h"
#include <stdio.h>

#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2

#define NUMBER_OF_BOARDS_BY_DILLON 10

// Test the next move code, given several boards.
// You need to also create 10 boards of your own to test.
void testBoards() {
  tictactoe_board_t board1; // Board 1 is the main example in the web-tutorial
                            // that I use on the web-site.
  board1.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board1.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board1.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board1.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board1.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board1.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board1.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board1.squares[BOT][MID] = MINIMAX_O_SQUARE;
  board1.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board2;
  board2.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board2.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board2.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board2.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board2.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board2.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board2.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board2.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board2.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board3;
  board3.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board3.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board3.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board3.squares[MID][LFT] = MINIMAX_O_SQUARE;
  board3.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board3.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board3.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board3.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board3.squares[BOT][RGT] = MINIMAX_X_SQUARE;

  tictactoe_board_t board4;
  board4.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board4.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board4.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board4.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board4.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board4.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board4.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board4.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board4.squares[BOT][RGT] = MINIMAX_X_SQUARE;

  tictactoe_board_t board5;
  board5.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board5.squares[TOP][MID] = MINIMAX_X_SQUARE;
  board5.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[MID][MID] = MINIMAX_O_SQUARE;
  board5.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board5.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // MY TEN BOARDS FOR DEBUGGING & TESTING
  tictactoe_board_t board_dillon_1;
  board_dillon_1.squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_1.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_1.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_1.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board_dillon_1.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_1.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_1.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board_dillon_1.squares[BOT][MID] = MINIMAX_O_SQUARE;
  board_dillon_1.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board_dillon_2;
  board_dillon_2.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board_dillon_2.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_2.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_2.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_2.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_2.squares[MID][RGT] = MINIMAX_O_SQUARE;
  board_dillon_2.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_2.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_2.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board_dillon_3;
  board_dillon_3.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board_dillon_3.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_3.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_3.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_3.squares[MID][MID] = MINIMAX_X_SQUARE;
  board_dillon_3.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_3.squares[BOT][LFT] = MINIMAX_O_SQUARE;
  board_dillon_3.squares[BOT][MID] = MINIMAX_O_SQUARE;
  board_dillon_3.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board_dillon_4;
  board_dillon_4.squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_4.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_4.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_4.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_4.squares[MID][MID] = MINIMAX_X_SQUARE;
  board_dillon_4.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_4.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_4.squares[BOT][MID] = MINIMAX_O_SQUARE;
  board_dillon_4.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board_dillon_5;
  board_dillon_5.squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_5.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_5.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_5.squares[MID][LFT] = MINIMAX_O_SQUARE;
  board_dillon_5.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_5.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_5.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board_dillon_5.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_5.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board_dillon_6;
  board_dillon_6.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board_dillon_6.squares[TOP][MID] = MINIMAX_O_SQUARE;
  board_dillon_6.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board_dillon_6.squares[MID][LFT] = MINIMAX_O_SQUARE;
  board_dillon_6.squares[MID][MID] = MINIMAX_X_SQUARE;
  board_dillon_6.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_6.squares[BOT][LFT] = MINIMAX_O_SQUARE;
  board_dillon_6.squares[BOT][MID] = MINIMAX_X_SQUARE;
  board_dillon_6.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board_dillon_7;
  board_dillon_7.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board_dillon_7.squares[TOP][MID] = MINIMAX_O_SQUARE;
  board_dillon_7.squares[TOP][RGT] = MINIMAX_O_SQUARE;
  board_dillon_7.squares[MID][LFT] = MINIMAX_O_SQUARE;
  board_dillon_7.squares[MID][MID] = MINIMAX_O_SQUARE;
  board_dillon_7.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_7.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_7.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_7.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board_dillon_8;
  board_dillon_8.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board_dillon_8.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_8.squares[TOP][RGT] = MINIMAX_O_SQUARE;
  board_dillon_8.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_8.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_8.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_8.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_8.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_8.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board_dillon_9;
  board_dillon_9.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board_dillon_9.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_9.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_9.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_9.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_9.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_9.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board_dillon_9.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_9.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board_dillon_10;
  board_dillon_10.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board_dillon_10.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_10.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_10.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_10.squares[MID][MID] = MINIMAX_O_SQUARE;
  board_dillon_10.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_10.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board_dillon_10.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board_dillon_10.squares[BOT][RGT] = MINIMAX_X_SQUARE;

  // END DEBUGGING & TESTING ADDITIONAL BOARDS
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  tictactoe_location_t move;

  bool is_Xs_turn = true;

  move = minimax_computeNextMove(&board1, is_Xs_turn);
  printf("next move for board1: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board2, is_Xs_turn);
  printf("next move for board2: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board3, is_Xs_turn);
  printf("next move for board3: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board4, !is_Xs_turn);
  printf("next move for board4: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board5, !is_Xs_turn);
  printf("next move for board5: (%d, %d)\n", move.row, move.column);

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // PRINT THE OUTPUT FOR MY TEN BOARDS
  move = minimax_computeNextMove(&board_dillon_1, is_Xs_turn);
  printf("next move for board_dillon_1: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_2, is_Xs_turn);
  printf("next move for board_dillon_2: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_3, is_Xs_turn);
  printf("next move for board_dillon_3: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_4, is_Xs_turn);
  printf("next move for board_dillon_4: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_5, is_Xs_turn);
  printf("next move for board_dillon_5: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_6, is_Xs_turn);
  printf("next move for board_dillon_6: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_7, is_Xs_turn);
  printf("next move for board_dillon_7: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_8, is_Xs_turn);
  printf("next move for board_dillon_8: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_9, is_Xs_turn);
  printf("next move for board_dillon_9: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board_dillon_10, is_Xs_turn);
  printf("next move for board_dillon_10: (%d, %d)\n", move.row, move.column);
  // DONE PRINTING BOARD BEST MOVES
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}