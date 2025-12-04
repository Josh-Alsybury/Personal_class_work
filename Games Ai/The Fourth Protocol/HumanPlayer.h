
/**
 * @file HumanPlayer.h
 * @brief Human player input handling
 *
 * Manages human player interactions including piece selection,
 * placement, and movement for both game phases.
 */

#pragma once
#include "Player.h"
#include "Board.h"

 /**
  * @class HumanPlayer
  * @brief Handles human player input and interactions
  *
  * Inherits from Player base class. Manages:
  * - Mouse click handling
  * - Piece selection via keyboard
  * - Valid move generation and visualization
  * - Two-step movement selection
  */
class HumanPlayer : public Player
{
public:
	/**
	 * @brief Default constructor
	 */
	HumanPlayer() = default;

	/**
	 * @brief Resets player to initial state
	 */
	void reset();

	/**
	 * @brief Handles mouse click during placement phase
	 *
	 * @param pixelX Mouse X coordinate in pixels
	 * @param pixelY Mouse Y coordinate in pixels
	 * @param board Reference to game board
	 * @return true if piece was successfully placed
	 * @return false otherwise
	 */
	bool handleClick(int pixelX, int pixelY, Board& board);

	/**
	 * @brief Checks if all pieces have been placed
	 *
	 * @return true if all piece counts are zero
	 * @return false otherwise
	 */
	bool allPiecesPlaced() const;

	/**
	 * @brief Handles mouse click during movement phase
	 *
	 * @param pixelX Mouse X coordinate in pixels
	 * @param pixelY Mouse Y coordinate in pixels
	 * @param board Reference to game board
	 * @return true if valid move completed
	 * @return false if piece selected or invalid click
	 */
	bool handleMovementClick(int pixelX, int pixelY, Board& board);

	/**
	 * @brief Generates all valid movement destinations for a piece
	 *
	 * @param board Current game board state
	 * @param x Piece's x-coordinate
	 * @param y Piece's y-coordinate
	 * @param type Type of piece
	 * @return std::vector<std::pair<int, int>> List of valid destinations
	 */
	std::vector<std::pair<int, int>> generateValidMoves(
		const Board& board, int x, int y, PieceType type);

	/**
	 * @brief Selects a piece type for placement
	 *
	 * @param piece The piece type to select
	 */
	void selectPiece(PieceType piece);

	bool hasSelectedBoardPiece = false; ///< Whether a piece is selected for movement
	int celX = -1;                      ///< X-coordinate of selected piece
	int celY = -1;                      ///< Y-coordinate of selected piece
	std::vector<std::pair<int, int>> validMoves; ///< Valid destinations for selected piece

private:
	/**
	 * @brief Gets pointer to stock count of selected piece
	 *
	 * @return int* Pointer to piece count, or nullptr if invalid
	 */
	int* getSelectedStock();
};