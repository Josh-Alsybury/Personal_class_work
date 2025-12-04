/**
 * @file Player.h
 * @brief Base player structure for piece management
 *
 * Defines the common interface for managing player pieces
 * during the placement phase. Inherited by HumanPlayer and AIPlayer.
 */

#pragma once
#include "Piece.h"

 /**
  * @struct Player
  * @brief Base player structure for piece inventory
  *
  * Manages piece counts and selection during placement phase.
  * Both HumanPlayer and AIPlayer inherit from this structure.
  */
struct Player
{
	int donkeys = 3; ///< Number of donkeys remaining to place
	int snake = 1;   ///< Number of snakes remaining to place
	int frog = 1;    ///< Number of frogs remaining to place

	PieceType selected = PieceType::Donkey; ///< Currently selected piece type

	/**
	 * @brief Consumes one unit of the selected piece type
	 *
	 * @return true if piece consumed successfully
	 * @return false if no pieces of selected type remaining
	 */
	bool take()
	{
		if (selected == PieceType::Donkey && donkeys > 0) { --donkeys; return true; }
		if (selected == PieceType::Snake && snake > 0) { --snake; return true; }
		if (selected == PieceType::Frog && frog > 0) { --frog; return true; }
		return false;
	}

	/**
	 * @brief Checks if all pieces have been placed
	 *
	 * @return true if all piece counts are zero
	 * @return false otherwise
	 */
	bool empty() const { return donkeys + snake + frog == 0; }
};