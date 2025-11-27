#include "AIPlayer.h"
#include <algorithm>
#include <iostream>

// ============================================================================
// PLACEMENT PHASE FUNCTIONS
// ============================================================================

// Main decision function for placement phase
// Tries to block human first, then finds best offensive move
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

// Checks every empty cell to see if human could win there next turn
// If yes, returns that cell to block it
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
                // SIM Place human's piece here temporarily
                board[row][col] = { humanPlayer.selected, Owner::Human };

                // EVAL Would this give human 3+ in a row?
                int humanScore = evaluateBoard(board, Owner::Human);

                // UNDO Remove the simulated piece
                board[row][col] = { PieceType::None, Owner::None };

                // If human would get 3+ in a row score >= 100, block this cell!
                if (humanScore >= 100)
                {
                    blockMove.x = col;
                    blockMove.y = row;
                    blockMove.score = 100000; // High priority
                    return blockMove; // Return immediately blocking is critical
                }
            }
        }
    }

    return blockMove; // No block needed returns invalid move
}

// Finds best offensive move by checking empty cells and scoring them
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
                // SIM Place AI piece here temporarily
                board[row][col] = { selected, Owner::NPC };

                // EVAL How good is this position for AI?
                int aiScore = evaluateBoard(board, Owner::NPC);
                int humanScore = evaluateBoard(board, Owner::Human);

                // Calculate move score
                // If AI would win (5 in a row), give huge score
                // Otherwise, score = (how good for AI) - (how good for human)
                int moveScore = (aiScore >= 10000) ? 900000 : (aiScore - humanScore);

                // UNDO Remove the simulated piece
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

// ==============================
// BOARD EVALUATION FUNCTIOS
// =========================

// Evaluates board position and returns a score for the player
// Higher score = better position
int AIPlayer::evaluateBoard(const Board& board, Owner player)
{
    int totalScore = 0;

    // Scan entire board looking for player's pieces
    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            // Check if this cell has a piece owned by the player we're evaluating
            if (board[row][col].owner == player && board[row][col].type != PieceType::None)
            {
                // Count consecutive pieces in all 4 directions from this piece
                int horizontalCount = countInRow(board, col, row, 1, 0, player);
                int verticalCount = countInRow(board, col, row, 0, 1, player);
                int diagonalRightCount = countInRow(board, col, row, 1, 1, player);
                int diagonalLeftCount = countInRow(board, col, row, -1, 1, player);

                // Take the best run from all 4 directions
                int longestRun = std::max({ horizontalCount, verticalCount,
                                           diagonalRightCount, diagonalLeftCount });

                // Convert run length to score
                if (longestRun >= 5)
                    totalScore += 10000;  // WIN!
                else if (longestRun == 4)
                    totalScore += 1000;   // Critical threat
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

// Counts consecutive pieces in a line both directions from starting point
int AIPlayer::countInRow(const Board& board, int x, int y,
    int directionX, int directionY, Owner player)
{
    int count = 1; // Start counting from this piece the starting piece itself

    // =============================
    // Count in POSITIVE direction 
    // ===========================
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
            break; // Hit empty cell or opponent's piece  stop counting
        }
    }

    // ===========================
    // Count in NEGATIVE direction 
    // ==========================
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
// MOVEMENT GENERATION FUNCTIONS
// ============================================================================

// Generates all valid moves for a specific piece based on its type
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

    // Helper function Add destination if it's valid and empty
    auto addIfEmpty = [&](int newX, int newY) {
        if (board.isValid(newX, newY) && board.isEmpty(newX, newY))
            validMoves.emplace_back(newX, newY);
        };

    // ==============================
    // DONKEY Moves only 4 directions 
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

    // ======================================
    // SNAKE Moves in all 8 directions
    // ===================================
    else if (type == PieceType::Snake)
    {
        for (const auto& direction : allDirections)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            addIfEmpty(newX, newY);
        }
    }

    // ===============================================
    // FROG Moves in 8 directions + can jump over pieces
    // ===============================================
    else if (type == PieceType::Frog)
    {
        // Normal moves One step in all 8 directions
        for (const auto& direction : allDirections)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            addIfEmpty(newX, newY);
        }

        // Special moves Jump over pieces (2 spaces away)
        for (const auto& direction : allDirections)
        {
            // First cell The piece we might jump over
            int adjacentX = x + direction.first;
            int adjacentY = y + direction.second;

            // Make sure adjacent cell is valid (prevents wrapping around board)
            if (!board.isValid(adjacentX, adjacentY))
                continue;

            // Is there a piece to jump over
            if (board.isValid(adjacentX, adjacentY) &&
                !board.isEmpty(adjacentX, adjacentY))
            {
                // Landing spot after the jump
                int jumpX = adjacentX + direction.first;
                int jumpY = adjacentY + direction.second;

                // Can we land there
                if (board.isValid(jumpX, jumpY) && board.isEmpty(jumpX, jumpY))
                {
                    validMoves.emplace_back(jumpX, jumpY); // Valid jump
                }
            }
        }
    }

    return validMoves;
}

// Generates all possible moves for AI pieces
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

// Generates all possible moves for Human pieces
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

                // Generate valid moves based on piece type (same rules as AI)
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

// ========================
// MINIMAX ALGORITHM
// ===========================

// Recursively looks ahead to find best move, assuming both players play optimal
int AIPlayer::minimax(Board& board, int depth, bool isAITurn, const Player& human)
{
    //  Reached search depth limit
    if (depth == 0)
    {
        // Stop recursion and evaluate current position
        int aiScore = evaluateBoard(board, Owner::NPC);
        int humanScore = evaluateBoard(board, Owner::Human);

        // Return difference: positive = good 4 AI, negative = good  4 Human
        return aiScore - humanScore;
    }

    // ==========================
    // AI's Turn Maximize score wants highest value
    // ==========================
    if (isAITurn)
    {
        int maxEvaluation = -999999; // Start with very low value

        // Generate all possible AI moves
        auto possibleMoves = generateMoves(board);

        // Try each move and see what happens
        for (auto& move : possibleMoves)
        {
            // SAVE the current board state so we can undo later
            Cell savedFromCell = board[move.fromY][move.fromX];  // Save source cell
            Cell savedToCell = board[move.y][move.x];            // Save destination cell

            // MAKE the move
            board.movePiece(move.fromX, move.fromY, move.x, move.y);

            // RECURSE: Evaluate this position (it's now Human's turn)
            int evaluation = minimax(board, depth - 1, false, human);

            // UNDO the move (restore board state)
            board[move.fromY][move.fromX] = savedFromCell;
            board[move.y][move.x] = savedToCell;

            // Track the best score found
            if (evaluation > maxEvaluation)
                maxEvaluation = evaluation;
        }

        return maxEvaluation; // Return best score AI can achieve
    }

    // ==========================================
    // Human's Turn Minimize score wants lowest value for AI
    // ==========================================
    else
    {
        int minEvaluation = 999999; // Start with very high value

        // Generate all possible Human moves
        auto possibleMoves = generateHumanMoves(board, human);

        // Try each human move and see what happens
        for (auto& move : possibleMoves)
        {
            // SAVE the current board state
            Cell savedFromCell = board[move.fromY][move.fromX];
            Cell savedToCell = board[move.y][move.x];

            // MAKE humans move
            board.movePiece(move.fromX, move.fromY, move.x, move.y);

            // RECURSE Evaluate this position switches back to AIs turn
            int evaluation = minimax(board, depth - 1, true, human);

            // UNDO the move
            board[move.fromY][move.fromX] = savedFromCell;
            board[move.y][move.x] = savedToCell;

            // Track the worst score for AI 
            if (evaluation < minEvaluation)
                minEvaluation = evaluation;
        }

        return minEvaluation; // Return worst score for AI
    }
}

// =================================
// MOVEMENT PHASE MAIN DECISION FUNCTION
// =====================

Move AIPlayer::findBestMoveMovement(Board& board, const Player& human, int depth)
{
    Move bestMove;
    bestMove.score = -999999;

    // Generate all possible AI moves
    auto aiMoves = generateMoves(board);

    std::cout << "\n=== AI TURN DEBUG ===\n";
    std::cout << "AI has " << aiMoves.size() << " possible moves\n";

    // ===============================
    // TIER 1: Check if AI can win immediately
    // ==============================
    for (auto& move : aiMoves)
    {
        // SAVE current state
        Cell savedFromCell = board[move.fromY][move.fromX];
        Cell savedToCell = board[move.y][move.x];

        // TRY this move
        board.movePiece(move.fromX, move.fromY, move.x, move.y);

        // EVAl  Does this win the game?
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

    // =========================
    // TIER 2 Check if Human can win next turn and block it
    // ========================
    auto humanMoves = generateHumanMoves(board, human);
    std::cout << "Human has " << humanMoves.size() << " possible moves\n";

    bool foundThreat = false;

    for (auto& humanMove : humanMoves)
    {
        //  current state
        Cell savedFromCell = board[humanMove.fromY][humanMove.fromX];
        Cell savedToCell = board[humanMove.y][humanMove.x];

        // What if Human makes this move 
        board.movePiece(humanMove.fromX, humanMove.fromY,humanMove.x, humanMove.y);

        // EVAL Would Human win
        int humanScore = evaluateBoard(board, Owner::Human);

        // UNDO simulation
        board[humanMove.fromY][humanMove.fromX] = savedFromCell;
        board[humanMove.y][humanMove.x] = savedToCell;

        // Show all serious threats (4+ in a row)
        if (humanScore >= 1000)
        {
            std::cout << "THREAT DETECTED: Human can move from ("
                << humanMove.fromX << "," << humanMove.fromY << ") to ("
                << humanMove.x << "," << humanMove.y << ") - Score: "
                << humanScore << "\n";
        }

        //: Human would win next turn!
        if (humanScore >= 10000)
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

                // TES Can human still win after our block?
                board.movePiece(humanMove.fromX, humanMove.fromY,
                    humanMove.x, humanMove.y);
                int humanScoreAfterBlock = evaluateBoard(board, Owner::Human);

                // UNDO human's test move
                board.movePiece(humanMove.x, humanMove.y,
                    humanMove.fromX, humanMove.fromY);

                // EVAL position after blocking
                int aiScore = evaluateBoard(board, Owner::NPC);
                int humanScore = evaluateBoard(board, Owner::Human);

                // Calculate blocking effectiveness
                int blockScore = aiScore - (humanScore * 3);

                // If this actually prevents the win, give huge bonus!
                if (humanScoreAfterBlock < 10000)
                    blockScore += 50000;

                // UNDO  blocking move
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

    // ===============
    // TIER 3 No immediate threats  use minimax 
    // ===================
    std::cout << "Using minimax with depth " << depth << "\n";

    for (auto& move : aiMoves)
    {
        // SAVE state
        Cell savedFromCell = board[move.fromY][move.fromX];
        Cell savedToCell = board[move.y][move.x];

        // TRY this move
        board.movePiece(move.fromX, move.fromY, move.x, move.y);

        // EVALUATE using minimax (looks ahead multiple turns)
        int moveScore = minimax(board, depth, false, human);

        // UNDO the move
        board[move.fromY][move.fromX] = savedFromCell;
        board[move.y][move.x] = savedToCell;

        // Track the best scoring move
        if (moveScore > bestMove.score)
        {
            bestMove = move;
            bestMove.score = moveScore;
        }
    }

    std::cout << "Best move selected: (" << bestMove.fromX << ","<< bestMove.fromY << ") to (" << bestMove.x << "," << bestMove.y<< ") - Score: " << bestMove.score << "\n";
    std::cout << "=== END AI TURN ===\n\n";

    return bestMove;
}


// =========================
// UTILITY FUNCTIONS
// =========================

// Executes the chosen move on the board
void AIPlayer::executeMovement(const Move& move, Board& board)
{
    board.movePiece(move.fromX, move.fromY, move.x, move.y);
}

// Checks if AI has placed all its pieces
bool AIPlayer::allPiecesPlaced() const
{
    return donkeys == 0 && snake == 0 && frog == 0;
}
