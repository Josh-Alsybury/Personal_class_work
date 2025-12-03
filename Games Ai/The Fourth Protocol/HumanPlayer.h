#pragma once
#include "Player.h"
#include "Board.h"

// Human Player class that handles player input and piece selection
class HumanPlayer : public Player
{
public:
	HumanPlayer() = default;
	void reset();
	// Handle mouse click at pixel coordinates
	bool handleClick(int pixelX, int pixelY, Board& board);
	// checks pieces all Placed
	bool HumanPlayer::allPiecesPlaced() const;

	// Handle movement click position
	bool handleMovementClick(int pixelX, int pixelY, Board& board);

	// Handles valid moves for each piece
	std::vector<std::pair<int, int>> generateValidMoves(const Board& board, int x, int y, PieceType type);

	// Handle keyboard input for piece selection
	void selectPiece(PieceType piece);

	bool hasSelectedBoardPiece = false;
	int celX = -1, celY = -1;
	std::vector<std::pair<int, int>> validMoves;
private:
	// Get remaining stock for currently selected piece
	int* getSelectedStock();
};