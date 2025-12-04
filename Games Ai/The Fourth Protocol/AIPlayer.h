/**
 * @file AIPlayer.h
 * @brief AI player interface for intelligent opponent behavior
 *
 * Defines the AIPlayer class which implements strategic decision-making
 * using minimax algorithm with alpha-beta pruning for both placement
 * and movement phases of the game.
 */

#pragma once
#include "Player.h"
#include "Board.h"

 /**
  * @struct Move
  * @brief Represents a potential move with its evaluation score
  *
  * Used to store move information during AI decision-making.
  * For placement phase, only x and y are used.
  * For movement phase, all four coordinates are used.
  */
struct Move
{
	int x = -1;          ///< Destination X coordinate (-1 indicates invalid move)
	int y = -1;          ///< Destination Y coordinate (-1 indicates invalid move)
	int fromX = -1;      ///< Starting X coordinate (for movement phase)
	int fromY = -1;      ///< Starting Y coordinate (for movement phase)
	int score = -999999; ///< Evaluation score (higher is better for AI)
};

/**
 * @class AIPlayer
 * @brief AI opponent with strategic decision-making capabilities
 *
 * Inherits from Player base class. Implements intelligent gameplay using:
 * - Board evaluation with positional scoring
 * - Threat detection and blocking
 * - Minimax algorithm for lookahead
 * - Alpha-beta pruning for optimization
 */
class AIPlayer : public Player
{
public:
	/**
	 * @brief Default constructor
	 */
	AIPlayer() = default;

	/**
	 * @brief Finds the best placement move during placement phase
	 *
	 * @param board Current game board state
	 * @param humanPlayer Reference to human player for threat assessment
	 * @return Move The best move found (blocking or offensive)
	 */
	Move findBestMove(Board& board, const Player& humanPlayer);

	/**
	 * @brief Executes a move on the board
	 *
	 * @param m The move to execute
	 * @param board Board to modify
	 */
	void executeMovement(const Move& m, Board& board);

	/**
	 * @brief Checks if all AI pieces have been placed
	 *
	 * @return true if all pieces are on the board
	 * @return false otherwise
	 */
	bool allPiecesPlaced() const;

	/**
	 * @brief Finds the best move during movement phase
	 *
	 * @param board Current game board state
	 * @param human Reference to human player
	 * @param depth Search depth for minimax algorithm
	 * @return Move The best move found
	 */
	Move findBestMoveMovement(Board& board, const Player& human, int depth);

	/**
	 * @brief Counts consecutive pieces in a line bidirectionally
	 *
	 * @param board Current game board state
	 * @param x Starting x-coordinate
	 * @param y Starting y-coordinate
	 * @param dx X-direction increment (1, 0, or -1)
	 * @param dy Y-direction increment (1, 0, or -1)
	 * @param player Player whose pieces to count
	 * @return int Total consecutive pieces in both directions
	 */
	int countInRow(const Board& board, int x, int y, int dx, int dy, Owner player);

	/**
	 * @brief Resets AI to initial state
	 */
	void reset();

private:
	/**
	 * @brief Evaluates board position for a specific player
	 *
	 * @param board Current game board state
	 * @param player Player to evaluate position for
	 * @return int Position score (higher is better)
	 */
	int evaluateBoard(const Board& board, Owner player);

	/**
	 * @brief Generates valid movement destinations for a piece
	 *
	 * @param board Current game board state
	 * @param x Piece's x-coordinate
	 * @param y Piece's y-coordinate
	 * @param type Type of piece
	 * @return std::vector<std::pair<int, int>> List of valid destinations
	 */
	std::vector<std::pair<int, int>> generateValidMovesForPiece(
		const Board& board, int x, int y, PieceType type);

	/**
	 * @brief Generates all possible AI moves
	 *
	 * @param board Current game board state
	 * @return std::vector<Move> List of all possible moves
	 */
	std::vector<Move> generateMoves(const Board& board);

	/**
	 * @brief Generates all possible human moves
	 *
	 * @param board Current game board state
	 * @param human Reference to human player
	 * @return std::vector<Move> List of all possible moves
	 */
	std::vector<Move> generateHumanMoves(const Board& board, const Player& human);

	/**
	 * @brief Minimax algorithm with alpha-beta pruning
	 *
	 * @param board Current game board state
	 * @param depth Remaining search depth
	 * @param isAITurn True if maximizing, false if minimizing
	 * @param human Reference to human player
	 * @param alpha Alpha value for pruning
	 * @param beta Beta value for pruning
	 * @return int Best evaluation score at current depth
	 */
	int minimax(Board& board, int depth, bool isAITurn,
		const Player& human, int alpha = -999999, int beta = 999999);

	/**
	 * @brief Searches for moves that block human's immediate threats
	 *
	 * @param board Current game board state
	 * @param humanPlayer Reference to human player
	 * @return Move Blocking move if threat found, invalid move otherwise
	 */
	Move findBlockingMove(Board& board, const Player& humanPlayer);

	/**
	 * @brief Finds the best offensive placement move
	 *
	 * @param board Current game board state
	 * @return Move The highest-scoring offensive move
	 */
	Move findOffensiveMove(Board& board);
};