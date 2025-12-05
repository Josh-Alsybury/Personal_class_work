/**
 * @file AIPlayer.cpp
 * @brief AI player implementation with minimax algorithm and strategic evaluation
 *
 * Implements intelligent opponent behavior for both placement and movement phases.
 * Uses minimax with alpha-beta pruning for optimal decision making.
 */

#include "AIPlayer.h"
#include <algorithm>
#include <iostream>

 // ============================================================================
 // PLACEMENT PHASE FUNCTIONS
 // ============================================================================

 /**
  * @brief Main decision function for placement phase
  *
  * @param board Current game board state
  * @param humanPlayer Reference to human player for threat assessment
  * @return Move The best move found (blocking or offensive)
  *
  * Prioritizes blocking human's winning moves, then finds best offensive position.
  */
Move AIPlayer::findBestMove(Board& board, const Player& humanPlayer)
{
    // Check if AI needs to block human's winning move
    Move blockMove = findBlockingMove(board, humanPlayer);

    // If we found a block needed (x is not -1), return that blocking move
    if (blockMove.x != -1)
    {
        std::cout << "Blocking human at (" << blockMove.x << ", " << blockMove.y << ")\n";
        return blockMove;
    }

    // Otherwise find best offensive move
    return findOffensiveMove(board);
}

/**
 * @brief Searches for moves that block human's immediate winning threats
 *
 * @param board Current game board state
 * @param humanPlayer Reference to human player for piece type
 * @return Move Blocking move if threat found, invalid move (-1,-1) otherwise
 *
 * Simulates placing human pieces on empty cells to detect if they would
 * create 3+ in a row (score >= 1000), indicating need to block.
 */
Move AIPlayer::findBlockingMove(Board& board, const Player& humanPlayer)
{
    Move blockMove; // Default invalid move (x = -1)

    // Scan entire board
    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            // Only check empty cells
            if (board.isEmpty(col, row))
            {
                // SIM: Place human's piece here temporarily
                board[row][col] = { humanPlayer.selected, Owner::Human };

                // EVAL: Would this give human 3+ in a row?
                int humanScore = evaluateBoard(board, Owner::Human);

                // UNDO: Remove the simulated piece
                board[row][col] = { PieceType::None, Owner::None };

                // If human would get 3+ in a row score >= 1000, block this cell!
                if (humanScore >= 1000)
                {
                    blockMove.x = col;
                    blockMove.y = row;
                    blockMove.score = 100000; // High priority
                    return blockMove; // Return immediately - blocking is critical
                }
            }
        }
    }

    return blockMove; // No block needed - returns invalid move
}

/**
 * @brief Finds the best offensive placement move
 *
 * @param board Current game board state
 * @return Move The highest-scoring offensive move
 *
 * Evaluates all empty cells by simulating AI piece placement and calculating
 * score differential (AI advantage - Human advantage). Instant win moves
 * (5 in a row) receive maximum priority.
 */
Move AIPlayer::findOffensiveMove(Board& board)
{
    Move bestMove;
    bestMove.score = -999999; // Start with very low score

    // Scan entire board for empty cells
    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            // Only consider empty cells
            if (board.isEmpty(col, row))
            {
                // SIM: Place AI piece here temporarily
                board[row][col] = { selected, Owner::NPC };

                // EVAL: How good is this position for AI?
                int aiScore = evaluateBoard(board, Owner::NPC);
                int humanScore = evaluateBoard(board, Owner::Human);

                // Calculate move score
                // If AI would win (5 in a row), give huge score
                // Otherwise, score = (how good for AI) - (how good for human)
                int moveScore = (aiScore >= 10000) ? 900000 : (aiScore - humanScore);

                // UNDO: Remove the simulated piece
                board[row][col] = { PieceType::None, Owner::None };

                // Update best move if this is better
                if (moveScore > bestMove.score)
                {
                    bestMove.x = col;
                    bestMove.y = row;
                    bestMove.score = moveScore;
                }
            }
        }
    }

    return bestMove;
}

// ============================================================================
// BOARD EVALUATION FUNCTIONS
// ============================================================================

/**
 * @brief Evaluates board position for a specific player
 *
 * @param board Current game board state
 * @param player Player to evaluate position for
 * @return int Position score (higher is better for the player)
 *
 * Scoring system:
 * - Positional values: Center tiles worth more than edges
 * - 4 in a row: +1000 points (winning threat)
 * - 3 in a row: +100 points (strong position)
 * - 2 in a row: +10 points (building position)
 * - Single piece: +1 point plus positional value
 */
int AIPlayer::evaluateBoard(const Board& board, Owner player)
{
    int totalScore = 0;

    // ============================================
    // POSITIONAL VALUE MAP for 5x5 board
    // ============================================
    // Center is most valuable, edges less so, corners have strategic value
    const int positionValue[5][5] = {
        { 3,  2,  4,  2,  3},   // Row 0 (top)
        { 2,  6,  8,  6,  2},   // Row 1
        { 4,  8, 10,  8,  4},   // Row 2 (middle) - CENTER is king!
        { 2,  6,  8,  6,  2},   // Row 3
        { 3,  2,  4,  2,  3}    // Row 4 (bottom)
    };

    // Scan entire board looking for player's pieces
    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            // Check if this cell has a piece owned by the player we're evaluating
            if (board[row][col].owner == player && board[row][col].type != PieceType::None)
            {
                totalScore += positionValue[row][col];

                // Count consecutive pieces in all 4 directions from this piece
                int horizontalCount = countInRow(board, col, row, 1, 0, player);
                int verticalCount = countInRow(board, col, row, 0, 1, player);
                int diagonalRightCount = countInRow(board, col, row, 1, 1, player);
                int diagonalLeftCount = countInRow(board, col, row, -1, 1, player);

                // Take the best run from all 4 directions
                int longestRun = std::max({ horizontalCount, verticalCount,
                                           diagonalRightCount, diagonalLeftCount });

                // Convert run length to score
                if (longestRun == 4)
                    totalScore += 1000;   // WIN
                else if (longestRun == 3)
                    totalScore += 100;    // Dangerous
                else if (longestRun == 2)
                    totalScore += 10;     // Building
                else
                    totalScore += 1;      // Single piece
            }
        }
    }

    return totalScore;
}

/**
 * @brief Counts consecutive pieces in a line bidirectionally
 *
 * @param board Current game board state
 * @param x Starting x-coordinate
 * @param y Starting y-coordinate
 * @param directionX X-direction increment (1, 0, or -1)
 * @param directionY Y-direction increment (1, 0, or -1)
 * @param player Player whose pieces to count
 * @return int Total consecutive pieces in both directions
 *
 * Counts from starting position in both positive and negative direction,
 * stopping when encountering empty cells or opponent pieces.
 */
int AIPlayer::countInRow(const Board& board, int x, int y,
    int directionX, int directionY, Owner player)
{
    int count = 1; // Start counting from this piece - the starting piece itself

    // =============================
    // Count in POSITIVE direction
    // =============================
    int currentX = x + directionX;
    int currentY = y + directionY;

    // Keep going while we're on the board
    while (currentX >= 0 && currentY >= 0 &&
        currentX < GRID_WIDTH && currentY < GRID_HEIGHT)
    {
        // Check if this cell has player's piece
        if (board[currentY][currentX].owner == player &&
            board[currentY][currentX].type != PieceType::None)
        {
            count++; // Found another piece in this line
            currentX += directionX;  // Move to next cell
            currentY += directionY;
        }
        else
        {
            break; // Hit empty cell or opponent's piece - stop counting
        }
    }

    // =============================
    // Count in NEGATIVE direction
    // =============================
    currentX = x - directionX;
    currentY = y - directionY;

    while (currentX >= 0 && currentY >= 0 &&
        currentX < GRID_WIDTH && currentY < GRID_HEIGHT)
    {
        // Check if this cell has player's piece
        if (board[currentY][currentX].owner == player &&
            board[currentY][currentX].type != PieceType::None)
        {
            count++;
            currentX -= directionX;  // Continue in negative direction
            currentY -= directionY;
        }
        else
        {
            break; // Stop when we hit empty or opponent
        }
    }

    return count; // Total from both directions
}

// ============================================================================
// MOVE GENERATION FUNCTIONS
// ============================================================================

/**
 * @brief Generates all valid moves for AI pieces
 *
 * @param board Current game board state
 * @return std::vector<Move> List of all possible AI moves
 *
 * Scans board for AI pieces and generates valid destinations based
 * on each piece's movement rules.
 */
std::vector<Move> AIPlayer::generateMoves(const Board& board)
{
    std::vector<Move> allMoves;

    // Scan entire board looking for AI pieces
    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            // Found an AI piece
            if (board[row][col].owner == Owner::NPC)
            {
                // Get the type of this piece
                PieceType pieceType = board[row][col].type;

                // Generate valid moves based on piece type
                auto validDestinations = generateValidMovesForPiece(board, col, row, pieceType);

                // Convert each destination to a Move struct
                for (const auto& destination : validDestinations)
                {
                    Move move;
                    move.fromX = col;                  // Starting X position
                    move.fromY = row;                  // Starting Y position
                    move.x = destination.first;        // Destination X
                    move.y = destination.second;       // Destination Y
                    allMoves.push_back(move);
                }
            }
        }
    }

    return allMoves;
}

/**
 * @brief Generates all valid moves for Human pieces
 *
 * @param board Current game board state
 * @param human Reference to human player
 * @return std::vector<Move> List of all possible human moves
 *
 * Similar to generateMoves but for human pieces. Used in minimax
 * to simulate opponent's responses.
 */
std::vector<Move> AIPlayer::generateHumanMoves(const Board& board, const Player& human)
{
    std::vector<Move> allMoves;

    // Scan entire board looking for Human pieces
    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            // Found a Human piece
            if (board[row][col].owner == Owner::Human)
            {
                // Get the type of this piece
                PieceType pieceType = board[row][col].type;

                // Generate valid moves based on piece type
                auto validDestinations = generateValidMovesForPiece(board, col, row, pieceType);

                // Convert each destination to a Move struct
                for (const auto& destination : validDestinations)
                {
                    Move move;
                    move.fromX = col;                  // Starting X position
                    move.fromY = row;                  // Starting Y position
                    move.x = destination.first;        // Destination X
                    move.y = destination.second;       // Destination Y
                    allMoves.push_back(move);
                }
            }
        }
    }

    return allMoves;
}

// ============================================================================
// MINIMAX ALGORITHM
// ============================================================================

/**
 * @brief Minimax algorithm with alpha-beta pruning
 *
 * @param board Current game board state (modified during search)
 * @param depth Remaining search depth
 * @param isAITurn True if maximizing (AI's turn), false if minimizing (Human's turn)
 * @param human Reference to human player
 * @param alpha Alpha value for pruning
 * @param beta Beta value for pruning
 * @return int Best evaluation score at current depth
 *
 * Recursively searches game tree assuming both players play optimally.
 * AI maximizes score, human minimizes. Alpha-beta pruning eliminates
 * branches that cannot affect final decision.
 */
int AIPlayer::minimax(Board& board, int depth, bool isAITurn, const Player& human, int alpha, int beta)
{
    if (depth == 0)
    {
        int aiScore = evaluateBoard(board, Owner::NPC);
        int humanScore = evaluateBoard(board, Owner::Human);
        return aiScore - humanScore;
    }

    if (isAITurn)
    {
        int maxEvaluation = -999999;
        auto possibleMoves = generateMoves(board);

        // OPTIMIZATION: Sort moves by quick evaluation (best first)
        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [&](const Move& a, const Move& b) {
                // Quick heuristic: prefer center moves
                int scoreA = (2 - abs(a.x - 2)) + (2 - abs(a.y - 2));
                int scoreB = (2 - abs(b.x - 2)) + (2 - abs(b.y - 2));
                return scoreA > scoreB;
            });

        for (auto& move : possibleMoves)
        {
            Cell savedFromCell = board[move.fromY][move.fromX];
            Cell savedToCell = board[move.y][move.x];

            board.movePiece(move.fromX, move.fromY, move.x, move.y);
            int evaluation = minimax(board, depth - 1, false, human, alpha, beta);

            board[move.fromY][move.fromX] = savedFromCell;
            board[move.y][move.x] = savedToCell;

            maxEvaluation = std::max(maxEvaluation, evaluation);
            alpha = std::max(alpha, maxEvaluation);

            if (beta <= alpha)
                break; // Prune
        }

        return maxEvaluation;
    }
    else
    {
        // Same optimization for human's turn
        int minEvaluation = 999999;
        auto possibleMoves = generateHumanMoves(board, human);

        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [&](const Move& a, const Move& b) {
                int scoreA = (2 - abs(a.x - 2)) + (2 - abs(a.y - 2));
                int scoreB = (2 - abs(b.x - 2)) + (2 - abs(b.y - 2));
                return scoreA > scoreB;
            });

        for (auto& move : possibleMoves)
        {
            Cell savedFromCell = board[move.fromY][move.fromX];
            Cell savedToCell = board[move.y][move.x];

            board.movePiece(move.fromX, move.fromY, move.x, move.y);
            int evaluation = minimax(board, depth - 1, true, human, alpha, beta);

            board[move.fromY][move.fromX] = savedFromCell;
            board[move.y][move.x] = savedToCell;

            minEvaluation = std::min(minEvaluation, evaluation);
            beta = std::min(beta, minEvaluation);

            if (beta <= alpha)
                break; // Prune
        }

        return minEvaluation;
    }
}

// ============================================================================
// MOVEMENT PHASE MAIN DECISION FUNCTION
// ============================================================================

/**
 * @brief Finds the best movement during movement phase
 *
 * @param board Current game board state
 * @param human Reference to human player
 * @param depth Search depth for minimax
 * @return Move The best move found
 *
 * Three-tier decision making:
 * 1. Check if AI can win immediately (4+ in a row)
 * 2. Check if human threatens to win next turn and block
 * 3. Use minimax to find strategically best move
 */
Move AIPlayer::findBestMoveMovement(Board& board, const Player& human, int maxDepth)
{
    Move bestMove;
    bestMove.score = -999999;

    // Generate all possible AI moves
    auto aiMoves = generateMoves(board);

    std::cout << "\n=== AI TURN DEBUG ===\n";
    std::cout << "AI has " << aiMoves.size() << " possible moves\n";

    // ===============================
    // TIER 1: Check if AI can win immediately
    // ===============================
    for (auto& move : aiMoves)
    {
        // SAVE current state
        Cell savedFromCell = board[move.fromY][move.fromX];
        Cell savedToCell = board[move.y][move.x];

        // TRY this move
        board.movePiece(move.fromX, move.fromY, move.x, move.y);

        // EVAL: Does this win the game?
        int aiScore = evaluateBoard(board, Owner::NPC);

        // UNDO the move
        board[move.fromY][move.fromX] = savedFromCell;
        board[move.y][move.x] = savedToCell;

        // If this move wins (4+ in a row), take it immediately
        if (aiScore >= 1000)
        {
            std::cout << "*** AI CAN WIN! Moving from (" << move.fromX << ","
                << move.fromY << ") to (" << move.x << "," << move.y
                << ") ***\n";
            return move;
        }
    }

    // ===============================
    // TIER 2: Check if Human can win next turn and block it
    // ===============================
    auto humanMoves = generateHumanMoves(board, human);
    std::cout << "Human has " << humanMoves.size() << " possible moves\n";

    bool foundThreat = false;

    for (auto& humanMove : humanMoves)
    {
        // SAVE current state
        Cell savedFromCell = board[humanMove.fromY][humanMove.fromX];
        Cell savedToCell = board[humanMove.y][humanMove.x];

        // SIM: What if Human makes this move?
        board.movePiece(humanMove.fromX, humanMove.fromY, humanMove.x, humanMove.y);

        // EVAL: Would Human win?
        int humanScore = evaluateBoard(board, Owner::Human);

        // UNDO simulation
        board[humanMove.fromY][humanMove.fromX] = savedFromCell;
        board[humanMove.y][humanMove.x] = savedToCell;

        // Show all serious threats (3+ in a row)
        if (humanScore >= 100)
        {
            std::cout << "THREAT DETECTED: Human can move from ("
                << humanMove.fromX << "," << humanMove.fromY << ") to ("
                << humanMove.x << "," << humanMove.y << ") - Score: "
                << humanScore << "\n";
        }

        // CRITICAL: Human would win next turn!
        if (humanScore >= 1000)
        {
            foundThreat = true;
            std::cout << "*** CRITICAL THREAT: Human can WIN next turn! ***\n";

            // Find best blocking move
            Move blockMove;
            blockMove.score = -999999;

            // Try each AI move to see which best blocks the threat
            for (auto& move : aiMoves)
            {
                // SAVE state
                Cell savedFromCell2 = board[move.fromY][move.fromX];
                Cell savedToCell2 = board[move.y][move.x];

                // TRY our blocking move
                board.movePiece(move.fromX, move.fromY, move.x, move.y);

                // TEST: Can human still win after our block?
                board.movePiece(humanMove.fromX, humanMove.fromY, humanMove.x, humanMove.y);
                int humanScoreAfterBlock = evaluateBoard(board, Owner::Human);

                // UNDO human's test move
                board.movePiece(humanMove.x, humanMove.y, humanMove.fromX, humanMove.fromY);

                // EVAL position after blocking
                int aiScore = evaluateBoard(board, Owner::NPC);
                int humanScore = evaluateBoard(board, Owner::Human);

                // Calculate blocking effectiveness
                int blockScore = aiScore - (humanScore * 3);

                // If this actually prevents the win
                if (humanScoreAfterBlock < 1000)
                    blockScore += 50000;

                // UNDO blocking move
                board[move.fromY][move.fromX] = savedFromCell2;
                board[move.y][move.x] = savedToCell2;

                std::cout << "  Block option: (" << move.fromX << ","
                    << move.fromY << ") to (" << move.x << "," << move.y
                    << ") - Score: " << blockScore << "\n";

                // Track best blocking move
                if (blockScore > blockMove.score)
                {
                    blockMove = move;
                    blockMove.score = blockScore;
                }
            }

            std::cout << "BLOCKING with move: (" << blockMove.fromX << ","
                << blockMove.fromY << ") to (" << blockMove.x << ","
                << blockMove.y << ")\n";
            return blockMove;
        }
    }

    if (!foundThreat)
    {
        std::cout << "No immediate winning threats detected.\n";
    }

    // ===============================
    // TIER 3: No immediate threats - use ITERATIVE DEEPENING minimax
    // ===============================
    std::cout << "Using iterative deepening up to depth " << maxDepth << "\n";

    // ITERATIVE DEEPENING: Search progressively deeper, using previous results to order moves
    for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++)
    {
        std::cout << "  Searching at depth " << currentDepth << "...\n";

        // MOVE ORDERING: Sort moves based on scores from previous iteration
        // This dramatically improves alpha-beta pruning efficiency
        if (currentDepth > 1)
        {
            std::sort(aiMoves.begin(), aiMoves.end(),
                [](const Move& a, const Move& b) {
                    return a.score > b.score; // Best moves first
                });
        }
        else
        {
            // First iteration: use simple heuristic (prefer center)
            std::sort(aiMoves.begin(), aiMoves.end(),
                [](const Move& a, const Move& b) {
                    int scoreA = (2 - abs(a.x - 2)) + (2 - abs(a.y - 2));
                    int scoreB = (2 - abs(b.x - 2)) + (2 - abs(b.y - 2));
                    return scoreA > scoreB;
                });
        }

        for (auto& move : aiMoves)
        {
            // SAVE state
            Cell savedFromCell = board[move.fromY][move.fromX];
            Cell savedToCell = board[move.y][move.x];

            // TRY this move
            board.movePiece(move.fromX, move.fromY, move.x, move.y);

            // EVALUATE using minimax at current depth
            int moveScore = minimax(board, currentDepth, false, human, -999999, 999999);

            // UNDO the move
            board[move.fromY][move.fromX] = savedFromCell;
            board[move.y][move.x] = savedToCell;

            // Store score for next iteration's move ordering
            move.score = moveScore;

            // Track the best scoring move across all depths
            if (moveScore > bestMove.score)
            {
                bestMove = move;
                bestMove.score = moveScore;
            }
        }

        std::cout << "    Depth " << currentDepth << " complete. Best so far: ("
            << bestMove.fromX << "," << bestMove.fromY << ") to ("
            << bestMove.x << "," << bestMove.y << ") - Score: "
            << bestMove.score << "\n";
    }

    std::cout << "Best move selected after depth " << maxDepth << ": ("
        << bestMove.fromX << "," << bestMove.fromY << ") to ("
        << bestMove.x << "," << bestMove.y << ") - Score: "
        << bestMove.score << "\n";
    std::cout << "=== END AI TURN ===\n\n";

    return bestMove;
}
// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Executes a move on the board
 *
 * @param move The move to execute
 * @param board Board to modify
 */
void AIPlayer::executeMovement(const Move& move, Board& board)
{
    board.movePiece(move.fromX, move.fromY, move.x, move.y);
}

/**
 * @brief Checks if all AI pieces have been placed
 *
 * @return true if all piece counts are zero
 * @return false otherwise
 */
bool AIPlayer::allPiecesPlaced() const
{
    return donkeys == 0 && snake == 0 && frog == 0;
}

// ============================================================================
// MOVEMENT GENERATION FUNCTIONS
// ============================================================================

/**
 * @brief Generates valid movement destinations for a piece
 *
 * @param board Current game board state
 * @param x Piece's x-coordinate
 * @param y Piece's y-coordinate
 * @param type Type of piece (determines movement rules)
 * @return std::vector<std::pair<int, int>> List of valid (x,y) destinations
 *
 * Movement rules:
 * - Donkey: 4 orthogonal directions (up, down, left, right)
 * - Snake: 8 directions (orthogonal + diagonal)
 * - Frog: 8 directions + can jump over pieces to land 2 spaces away
 */
std::vector<std::pair<int, int>> AIPlayer::generateValidMovesForPiece(
    const Board& board, int x, int y, PieceType type)
{
    std::vector<std::pair<int, int>> validMoves;

    // Define movement directions
    const std::vector<std::pair<int, int>> orthogonalDirections = {
        {1,0}, {-1,0}, {0,1}, {0,-1}  // Right, Left, Down, Up
    };

    const std::vector<std::pair<int, int>> allDirections = {
        {1,0}, {-1,0}, {0,1}, {0,-1},      // 4 ways
        {1,1}, {1,-1}, {-1,1}, {-1,-1}     // Diagonal
    };

    // Helper function: Add destination if it's valid and empty
    auto addIfEmpty = [&](int newX, int newY) {
        if (board.isValid(newX, newY) && board.isEmpty(newX, newY))
            validMoves.emplace_back(newX, newY);
        };

    // =============================
    // DONKEY: Moves only 4 directions
    // =============================
    if (type == PieceType::Donkey)
    {
        for (const auto& direction : orthogonalDirections)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            addIfEmpty(newX, newY);
        }
    }

    // =============================
    // SNAKE: Moves in all 8 directions
    // =============================
    else if (type == PieceType::Snake)
    {
        for (const auto& direction : allDirections)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            addIfEmpty(newX, newY);
        }
    }

    // =============================
    // FROG: Moves in 8 directions + can jump over pieces
    // =============================
    else if (type == PieceType::Frog)
    {
        // Normal moves: One step in all 8 directions
        for (const auto& direction : allDirections)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            addIfEmpty(newX, newY);
        }

        // Special moves: Jump over pieces (2 spaces away)
        for (const auto& direction : allDirections)
        {
            // First cell: The piece we might jump over
            int adjacentX = x + direction.first;
            int adjacentY = y + direction.second;

            // Make sure adjacent cell is valid (prevents wrapping around board)
            if (!board.isValid(adjacentX, adjacentY))
                continue;

            // Is there a piece to jump over?
            if (board.isValid(adjacentX, adjacentY) &&
                !board.isEmpty(adjacentX, adjacentY))
            {
                // Landing spot after the jump
                int jumpX = adjacentX + direction.first;
                int jumpY = adjacentY + direction.second;

                // Can we land there?
                if (board.isValid(jumpX, jumpY) && board.isEmpty(jumpX, jumpY))
                {
                    validMoves.emplace_back(jumpX, jumpY); // Valid jump
                }
            }
        }
    }

    return validMoves;
}

/**
 * @brief Resets AI player to initial state
 *
 * Restores piece counts to starting values and resets piece selection.
 */
void AIPlayer::reset()
{
    // Reset piece counts to starting values
    donkeys = 3;
    snake = 3;
    frog = 3;

    // Reset selected piece to default (Donkey)
    selected = PieceType::Donkey;

    std::cout << "AI player reset - ready for new game!\n";
}