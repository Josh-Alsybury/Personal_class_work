/**
 * @file Board.h
 * @brief Game board representation and manipulation
 *
 * Provides the Board class which manages the game state grid,
 * piece placement, movement, and board queries.
 */

#pragma once
#include "Piece.h"
#include "Constants.h"
#include <array>

 /**
  * @class Board
  * @brief Manages the game board state and operations
  *
  * Maintains a 2D grid of cells and provides methods for:
  * - Accessing cells
  * - Placing pieces
  * - Moving pieces
  * - Validating positions
  * - Clearing the board
  */
class Board
{
public:
	/**
	 * @brief Default constructor
	 */
	Board() = default;

	/**
	 * @brief Access cell at position [y][x]
	 *
	 * @param y Row index
	 * @param x Column index
	 * @return Cell& Reference to the cell
	 */
	Cell& at(int y, int x) { return m_cells[y][x]; }

	/**
	 * @brief Access cell at position [y][x] (const version)
	 *
	 * @param y Row index
	 * @param x Column index
	 * @return const Cell& Const reference to the cell
	 */
	const Cell& at(int y, int x) const { return m_cells[y][x]; }

	/**
	 * @brief Array-style access operator
	 *
	 * @param y Row index
	 * @return std::array<Cell, GRID_WIDTH>& Reference to row
	 */
	std::array<Cell, GRID_WIDTH>& operator[](int y) { return m_cells[y]; }

	/**
	 * @brief Array-style access operator (const version)
	 *
	 * @param y Row index
	 * @return const std::array<Cell, GRID_WIDTH>& Const reference to row
	 */
	const std::array<Cell, GRID_WIDTH>& operator[](int y) const { return m_cells[y]; }

	/**
	 * @brief Clears the entire board
	 *
	 * Sets all cells to empty (None type, None owner)
	 */
	void clear()
	{
		for (auto& row : m_cells)
		{
			for (auto& cell : row)
			{
				cell = { PieceType::None, Owner::None };
			}
		}
	}

	/**
	 * @brief Checks if position is within board bounds
	 *
	 * @param x X-coordinate
	 * @param y Y-coordinate
	 * @return true if position is valid
	 * @return false otherwise
	 */
	bool isValid(int x, int y) const
	{
		return x >= 0 && y >= 0 && x < GRID_WIDTH && y < GRID_HEIGHT;
	}

	/**
	 * @brief Checks if cell is empty
	 *
	 * @param x X-coordinate
	 * @param y Y-coordinate
	 * @return true if cell is valid and contains no piece
	 * @return false otherwise
	 */
	bool isEmpty(int x, int y) const
	{
		return isValid(x, y) && m_cells[y][x].type == PieceType::None;
	}

	/**
	 * @brief Places a piece at the specified position
	 *
	 * @param x X-coordinate
	 * @param y Y-coordinate
	 * @param type Type of piece to place
	 * @param owner Owner of the piece
	 * @return true if placement successful
	 * @return false if cell is occupied or invalid
	 */
	bool placePiece(int x, int y, PieceType type, Owner owner)
	{
		if (!isEmpty(x, y)) return false;
		m_cells[y][x] = { type, owner };
		return true;
	}

	/**
	 * @brief Moves a piece from one position to another
	 *
	 * @param fromX Source X-coordinate
	 * @param fromY Source Y-coordinate
	 * @param toX Destination X-coordinate
	 * @param toY Destination Y-coordinate
	 * @return true if move successful
	 * @return false if destination is invalid or occupied
	 */
	bool movePiece(int fromX, int fromY, int toX, int toY)
	{
		// Target must be inside board and empty
		if (!isValid(toX, toY)) return false;
		if (m_cells[toY][toX].type != PieceType::None) return false;
		m_cells[toY][toX] = m_cells[fromY][fromX];   // Move
		m_cells[fromY][fromX] = Cell{};              // Clear origin
		return true;
	}

private:
	std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> m_cells; ///< 2D grid of cells
};