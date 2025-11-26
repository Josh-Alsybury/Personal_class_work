#pragma once
#pragma once
#include "Piece.h"
#include "Constants.h"
#include <array>

// Board class to manage game state
class Board
{
public:
	Board() = default;

	// Access cell at position [y][x]
	Cell& at(int y, int x) { return m_cells[y][x]; }
	const Cell& at(int y, int x) const { return m_cells[y][x]; }

	// Arraystyle access operator
	std::array<Cell, GRID_WIDTH>& operator[](int y) { return m_cells[y]; }
	const std::array<Cell, GRID_WIDTH>& operator[](int y) const { return m_cells[y]; }

	// Clear the entire board
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

	// Check if position is valid
	bool isValid(int x, int y) const
	{
		return x >= 0 && y >= 0 && x < GRID_WIDTH && y < GRID_HEIGHT;
	}

	// Check if cell is empty
	bool isEmpty(int x, int y) const
	{
		return isValid(x, y) && m_cells[y][x].type == PieceType::None;
	}

	// Place a piece at position
	bool placePiece(int x, int y, PieceType type, Owner owner)
	{
		if (!isEmpty(x, y)) return false;
		m_cells[y][x] = { type, owner };
		return true;
	}


	bool Board::movePiece(int fromX, int fromY, int toX, int toY)
	{
		// target must be insde board and emty
		if (!isValid(toX, toY)) return false;
		if (m_cells[toY][toX].type != PieceType::None) return false;

		m_cells[toY][toX] = m_cells[fromY][fromX];   // move
		m_cells[fromY][fromX] = Cell{};           // clear origin
		return true;
	}

private:
	std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> m_cells;
};