/**
 * @file Piece.h
 * @brief Core game piece definitions
 *
 * Defines enumerations for piece ownership, types, and the Cell structure
 * that represents a board position.
 */

#pragma once
#include <cstdint>

 /**
  * @enum Owner
  * @brief Represents piece ownership
  */
enum class Owner : std::uint8_t {
	None,  ///< No owner (empty cell)
	Human, ///< Owned by human player
	NPC    ///< Owned by AI player
};

/**
 * @enum PieceType
 * @brief Represents different piece types with unique movement rules
 */
enum class PieceType : std::uint8_t {
	None,   ///< No piece (empty cell)
	Donkey, ///< Moves 1 space in 4 orthogonal directions
	Snake,  ///< Moves 1 space in 8 directions
	Frog    ///< Moves 1 space in 8 directions or jumps over pieces
};

/**
 * @struct Cell
 * @brief Represents a single board cell
 *
 * Contains information about what piece (if any) occupies
 * the cell and who owns it.
 */
struct Cell
{
	PieceType type{ PieceType::None }; ///< Type of piece in this cell
	Owner owner{ Owner::None };        ///< Owner of the piece
};
