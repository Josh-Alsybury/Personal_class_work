/**
 * @file Npc.h
 * @brief Legacy NPC structure (deprecated)
 *
 * @deprecated This struct is superseded by AIPlayer class.
 * Kept for backwards compatibility but not actively used.
 */

#pragma once
#include "Piece.h"

 /**
  * @struct Npc
  * @brief Legacy NPC piece management
  * @deprecated Use AIPlayer instead
  */
struct Npc
{
	int donkeys = 3;  ///< Number of donkeys remaining
	int snake = 1;    ///< Number of snakes remaining
	int frog = 1;     ///< Number of frogs remaining

	PieceType selected = PieceType::Donkey; ///< Currently selected piece type

	/**
	 * @brief Consumes one unit of selected piece
	 * @return true if piece consumed successfully
	 * @return false if no pieces of type remaining
	 */
	bool take()
	{
		if (selected == PieceType::Donkey && donkeys > 0) { --donkeys; return true; }
		if (selected == PieceType::Snake && snake > 0) { --snake; return true; }
		if (selected == PieceType::Frog && frog > 0) { --frog; return true; }
		return false;
	}

	/**
	 * @brief Checks if all pieces have been used
	 * @return true if no pieces remaining
	 */
	bool empty() const { return donkeys + snake + frog == 0; }
};
