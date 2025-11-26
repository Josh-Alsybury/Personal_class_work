#pragma once
#include "Player.h"
#include "Board.h"

// Structure to represent a potential move and its evaluation score
struct Move
{
	int x = -1;      // destination X
	int y = -1;      // destination Y
	int fromX = -1;  // starting X 
	int fromY = -1;  // starting Y 
	int score = -999999;
};

// AI Player class that handles NPC decision making
class AIPlayer : public Player
{
public:
	AIPlayer() = default;

	// Find and return the best move for the AI
	Move findBestMove(Board& board, const Player& humanPlayer);

	void executeMovement(const Move& m, Board& board);

	bool AIPlayer::allPiecesPlaced() const;

	Move findBestMoveMovement(Board& board, const Player& human, int depth);
private:
	// Evaluate the board state for a given player
	int evaluateBoard(const Board& board, Owner player);

	// Count consecutive pieces in a row starting from (x,y) in direction (dx,dy)
	int countInRow(const Board& board, int x, int y, int dx, int dy, Owner player);

	std::vector<Move> generateMoves(const Board& board);
	std::vector<Move> generateHumanMoves(const Board& board, const Player& human);

	int minimax(Board& board, int depth, bool isAITurn, const Player& human);

	// Check if blocking is needed and return blocking move
	Move findBlockingMove(Board& board, const Player& humanPlayer);

	// Find the best offensive move
	Move findOffensiveMove(Board& board);
};