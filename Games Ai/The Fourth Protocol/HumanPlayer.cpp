/**
 * @file HumanPlayer.cpp
 * @brief Human player input handling and movement logic
 *
 * Manages human player interactions including piece placement, movement selection,
 * and valid move generation for different piece types.
 */

#include "HumanPlayer.h"
#include "Constants.h"
#include <iostream>

 /**
  * @brief Handles mouse click during placement phase
  *
  * @param pixelX Mouse X coordinate in pixels
  * @param pixelY Mouse Y coordinate in pixels
  * @param board Reference to game board
  * @return true if piece was successfully placed
  * @return false if placement failed (no pieces left, cell occupied, etc.)
  *
  * Converts pixel coordinates to grid coordinates and attempts to place
  * the currently selected piece type at that location.
  */
bool HumanPlayer::handleClick(int pixelX, int pixelY, Board& board)
{
	// Convert pixel coordinates to grid coordinates
	int gridX = static_cast<int>(pixelX / tileWidth);
	int gridY = static_cast<int>(pixelY / tileHeight);

	// Get stock for currently selected piece
	int* stock = getSelectedStock();

	// Check if player has pieces remaining
	if (!stock || *stock <= 0)
	{
		std::cout << "No pieces of this type left. Select another." << std::endl;
		return false;
	}

	// Attempt to place piece
	if (board.placePiece(gridX, gridY, selected, Owner::Human))
	{
		--(*stock);
		std::cout << "Placed " << static_cast<int>(selected)
			<< " at (" << gridX << ", " << gridY << ")" << std::endl;
		return true;
	}
	else
	{
		std::cout << "Cell occupied or invalid. Pick another cell." << std::endl;
		return false;
	}
}

/**
 * @brief Selects a piece type for placement
 *
 * @param piece The piece type to select (Donkey, Snake, or Frog)
 *
 * Updates the currently selected piece and provides console feedback.
 */
void HumanPlayer::selectPiece(PieceType piece)
{
	selected = piece;

	switch (piece)
	{
	case PieceType::Donkey:
		std::cout << "Donkey selected" << std::endl;
		break;
	case PieceType::Snake:
		std::cout << "Snake selected" << std::endl;
		break;
	case PieceType::Frog:
		std::cout << "Frog selected" << std::endl;
		break;
	default:
		break;
	}
}

/**
 * @brief Gets pointer to the stock count of currently selected piece
 *
 * @return int* Pointer to piece count, or nullptr if invalid selection
 */
int* HumanPlayer::getSelectedStock()
{
	switch (selected)
	{
	case PieceType::Donkey:
		return &donkeys;
	case PieceType::Snake:
		return &snake;
	case PieceType::Frog:
		return &frog;
	default:
		return nullptr;
	}
}

/**
 * @brief Checks if all pieces have been placed
 *
 * @return true if all piece counts are zero
 * @return false otherwise
 */
bool HumanPlayer::allPiecesPlaced() const
{
	return donkeys == 0 && snake == 0 && frog == 0;
}

/**
 * @brief Handles mouse click during movement phase
 *
 * @param pixelX Mouse X coordinate in pixels
 * @param pixelY Mouse Y coordinate in pixels
 * @param board Reference to game board
 * @return true if a valid move was completed (switches turn)
 * @return false if piece was just selected or invalid click
 *
 * Two-step process:
 * 1. First click selects a piece and highlights valid moves
 * 2. Second click moves to a valid destination or deselects
 */
bool HumanPlayer::handleMovementClick(int pixelX, int pixelY, Board& board)
{
	int x = pixelX / tileWidth;
	int y = pixelY / tileHeight;

	// Clicked outside board
	if (!board.isValid(x, y)) return false;

	// STEP 1: Click on your own piece - select it
	if (!hasSelectedBoardPiece)
	{
		const Cell& c = board.at(y, x);
		if (c.owner == Owner::Human)
		{
			hasSelectedBoardPiece = true;
			celX = x;
			celY = y;

			validMoves = generateValidMoves(board, x, y, c.type);
			// Just selected piece, don't switch turn yet!
			return false;
		}
		return false; // not your piece
	}

	// STEP 2: If a piece is already selected:
	// Check if clicked a valid destination - move there
	for (auto& mv : validMoves)
	{
		if (mv.first == x && mv.second == y)
		{
			board.movePiece(celX, celY, x, y);
			hasSelectedBoardPiece = false;
			validMoves.clear();
			return true;
		}
	}

	// STEP 3: Clicked somewhere not valid - deselect
	hasSelectedBoardPiece = false;
	validMoves.clear();
	return false;
}

/**
 * @brief Generates all valid movement destinations for a piece
 *
 * @param board Current game board state
 * @param x Piece's x-coordinate
 * @param y Piece's y-coordinate
 * @param type Type of piece (determines movement rules)
 * @return std::vector<std::pair<int, int>> List of valid (x,y) destinations
 *
 * Movement rules:
 * - Donkey: Can move 1 space in 4 orthogonal directions (up, down, left, right)
 * - Snake: Can move 1 space in 8 directions (orthogonal + diagonal)
 * - Frog: Can move 1 space in 8 directions OR jump over any piece to land 2 spaces away
 */
std::vector<std::pair<int, int>> HumanPlayer::generateValidMoves(const Board& board, int x, int y, PieceType type)
{
	std::vector<std::pair<int, int>> moves;

	// Directions for 4-way and 8-way movement
	const std::vector<std::pair<int, int>> dirs4 = { {1,0}, {-1,0}, {0,1}, {0,-1} };
	const std::vector<std::pair<int, int>> dirs8 = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };

	// Helper lambda: Add coordinate if it's valid and empty
	auto addIfEmpty = [&](int nx, int ny) {
		if (board.isValid(nx, ny) && board.isEmpty(nx, ny))
			moves.emplace_back(nx, ny);
		};

	if (type == PieceType::Donkey)
	{
		// Donkey moves 4 directions only
		for (const auto& d : dirs4)
			addIfEmpty(x + d.first, y + d.second);
	}
	else if (type == PieceType::Snake)
	{
		// Snake moves 8 directions
		for (const auto& d : dirs8)
			addIfEmpty(x + d.first, y + d.second);
	}
	else if (type == PieceType::Frog)
	{
		// Frog moves 8 directions plus long jumps

		// PART 1: One step in 8 directions
		for (const auto& d : dirs8)
			addIfEmpty(x + d.first, y + d.second);

		// PART 2: Long jumps in all 8 directions
		for (const auto& d : dirs8)
		{
			int nx = x + d.first;
			int ny = y + d.second;

			if (!board.isValid(nx, ny)) continue; // Checks jumps valid, stops wrapping of jump over cells

			// Check if the first tile in direction is occupied (the piece to jump over)
			if (board.isValid(nx, ny) && !board.isEmpty(nx, ny))
			{
				// Landing spot after jump
				int jumpX = nx + d.first;
				int jumpY = ny + d.second;

				if (board.isValid(jumpX, jumpY) && board.isEmpty(jumpX, jumpY))
				{
					moves.emplace_back(jumpX, jumpY);
				}
			}
		}
	}

	return moves;
}

// ============================================================================
// Resets player on board
// ============================================================================

/**
 * @brief Resets human player to initial state
 *
 * Restores piece counts to starting values, resets piece selection,
 * and clears any movement phase selection state.
 */
void HumanPlayer::reset()
{
	// Reset piece counts to starting values
	donkeys = 3;
	snake = 3;
	frog = 3;

	// Reset selected piece to default (Donkey)
	selected = PieceType::Donkey;

	// Clear movement phase state
	hasSelectedBoardPiece = false;
	celX = -1;
	celY = -1;
	validMoves.clear();

	std::cout << "Human player reset - ready for new game!\n";
}