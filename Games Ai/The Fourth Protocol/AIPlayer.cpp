#include "AIPlayer.h"
#include <algorithm>
#include <iostream>

// Finds best move for AI tries to block human first, then chooses offensive move
Move AIPlayer::findBestMove(Board& board, const Player& humanPlayer)
{
    // Check if AI needs to block human's winning move
    Move blockMove = findBlockingMove(board, humanPlayer);
    if (blockMove.x != -1)
    {
        std::cout << "Blocking human at (" << blockMove.x << ", " << blockMove.y << ")\n";
        return blockMove;
    }

    // Otherwise find best offensive move
    return findOffensiveMove(board);
}

// Check every empty cell to see if human could win there next turn, if yes, block it
Move AIPlayer::findBlockingMove(Board& board, const Player& humanPlayer)
{
    Move blockMove; // Default invalid move

    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            if (board.isEmpty(col, row))
            {
                // Simulate human placing piece here
                board[row][col] = { humanPlayer.selected, Owner::Human };
                int humanScore = evaluateBoard(board, Owner::Human);

                // Undo simulation
                board[row][col] = { PieceType::None, Owner::None };

                // If human would win, block this cell
                if (humanScore >= 1000)
                {
                    blockMove.x = col;
                    blockMove.y = row;
                    blockMove.score = 100000; // High priority
                    return blockMove;
                }
            }
        }
    }
    return blockMove; // No block needed
}

// Finds best offensive move by checking empty cells and scoring them
Move AIPlayer::findOffensiveMove(Board& board)
{
    Move bestMove;
    bestMove.score = -999999; // Very low initial score

    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            if (board.isEmpty(col, row))
            {
                // Simulate AI placing piece here
                board[row][col] = { selected, Owner::NPC };

                int aiScore = evaluateBoard(board, Owner::NPC);
                int humanScore = evaluateBoard(board, Owner::Human);

                int moveScore = (aiScore >= 10000) ? 900000 : (aiScore - humanScore);

                // Undo simulation
                board[row][col] = { PieceType::None, Owner::None };

                // Update best move
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

// Evaluates board score for player based on consecutive pieces in a row
int AIPlayer::evaluateBoard(const Board& board, Owner player)
{
    int totalScore = 0;

    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            if (board[row][col].owner == player && board[row][col].type != PieceType::None)
            {
                int horiz = countInRow(board, col, row, 1, 0, player);
                int vert = countInRow(board, col, row, 0, 1, player);
                int diagRight = countInRow(board, col, row, 1, 1, player);
                int diagLeft = countInRow(board, col, row, -1, 1, player);

                int maxRun = std::max({ horiz, vert, diagRight, diagLeft });

                if (maxRun >= 5) totalScore += 10000;
                else if (maxRun == 4) totalScore += 1000;
                else if (maxRun == 3) totalScore += 100;
                else if (maxRun == 2) totalScore += 10;
                else totalScore += 1;
            }
        }
    }
    return totalScore;
}

// Counts consecutive pieces in one direction and the opposite for player
int AIPlayer::countInRow(const Board& board, int x, int y, int dx, int dy, Owner player)
{
    int count = 1; // Start counting from this piece

    // Positive direction
    int cx = x + dx;
    int cy = y + dy;
    while (cx >= 0 && cy >= 0 && cx < GRID_WIDTH && cy < GRID_HEIGHT)
    {
        if (board[cy][cx].owner == player && board[cy][cx].type != PieceType::None)
        {
            count++;
            cx += dx;
            cy += dy;
        }
        else break;
    }

    // Negative direction
    cx = x - dx;
    cy = y - dy;
    while (cx >= 0 && cy >= 0 && cx < GRID_WIDTH && cy < GRID_HEIGHT)
    {
        if (board[cy][cx].owner == player && board[cy][cx].type != PieceType::None)
        {
            count++;
            cx -= dx;
            cy -= dy;
        }
        else break;
    }

    return count;
}

// Generate all moves for AI pieces (adjacent empty cells)
std::vector<Move> AIPlayer::generateMoves(const Board& board)
{
    std::vector<Move> moves;

    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            if (board[row][col].owner == Owner::NPC)
            {
                const int directions[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

                for (auto& d : directions)
                {
                    int nx = col + d[0];
                    int ny = row + d[1];
                    if (board.isValid(nx, ny) && board.isEmpty(nx, ny))
                    {
                        Move m;
                        m.fromX = col;
                        m.fromY = row;
                        m.x = nx;
                        m.y = ny;
                        moves.push_back(m);
                    }
                }
            }
        }
    }
    return moves;
}

// Generate all moves for Human pieces (adjacent empty cells)
std::vector<Move> AIPlayer::generateHumanMoves(const Board& board, const Player& human)
{
    std::vector<Move> moves;

    for (int row = 0; row < GRID_HEIGHT; ++row)
    {
        for (int col = 0; col < GRID_WIDTH; ++col)
        {
            if (board[row][col].owner == Owner::Human)
            {
                const int directions[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

                for (auto& d : directions)
                {
                    int nx = col + d[0];
                    int ny = row + d[1];
                    if (board.isValid(nx, ny) && board.isEmpty(nx, ny))
                    {
                        Move m;
                        m.fromX = col;
                        m.fromY = row;
                        m.x = nx;
                        m.y = ny;
                        moves.push_back(m);
                    }
                }
            }
        }
    }
    return moves;
}

int AIPlayer::minimax(Board& board, int depth, bool isAITurn, const Player& human)
{
    if (depth == 0)
    {
        // Evaluate board from AI perspective
        int aiScore = evaluateBoard(board, Owner::NPC);
        int humanScore = evaluateBoard(board, Owner::Human);
        return aiScore - humanScore;
    }

    if (isAITurn)
    {
        int maxEval = -999999;

        // Generate all AI moves 
        auto moves = generateMoves(board);

        for (auto& move : moves)
        {
            // Save state to undo later
            Cell savedFrom = board[move.fromY][move.fromX];
            Cell savedTo = board[move.y][move.x];

            // Make move
            board.movePiece(move.fromX, move.fromY, move.x, move.y);

            // Recurse for human turn
            int eval = minimax(board, depth - 1, false, human);

            // Undo move
            board[move.fromY][move.fromX] = savedFrom;
            board[move.y][move.x] = savedTo;

            if (eval > maxEval)
                maxEval = eval;
        }

        return maxEval;
    }
    else // Human turn minimizing player
    {
        int minEval = 999999;

        auto humanMoves = generateHumanMoves(board, human);

        for (auto& move : humanMoves)
        {
            // Save state
            Cell savedFrom = board[move.fromY][move.fromX];
            Cell savedTo = board[move.y][move.x];

            // Make human move
            board.movePiece(move.fromX, move.fromY, move.x, move.y);

            // Recurse for AI turn
            int eval = minimax(board, depth - 1, true, human);

            // Undo move
            board[move.fromY][move.fromX] = savedFrom;
            board[move.y][move.x] = savedTo;

            if (eval < minEval)
                minEval = eval;
        }

        return minEval;
    }
}

// Finds best movement move for AI by simulating moves and human replies (2 ply)
Move AIPlayer::findBestMoveMovement(Board& board, const Player& human, int depth)
{
    Move bestMove;
    bestMove.score = -999999;

    auto aiMoves = generateMoves(board);

    for (auto& move : aiMoves)
    {
        // Save state
        Cell savedFrom = board[move.fromY][move.fromX];
        Cell savedTo = board[move.y][move.x];

        // Make AI move
        board.movePiece(move.fromX, move.fromY, move.x, move.y);

        // Use depth from parameter for minimax
        int moveScore = minimax(board, depth, false, human);

        // Undo AI move
        board[move.fromY][move.fromX] = savedFrom;
        board[move.y][move.x] = savedTo;

        if (moveScore > bestMove.score)
        {
            bestMove = move;
            bestMove.score = moveScore;
        }
    }

    return bestMove;
}

// Actually move AI piece on board
void AIPlayer::executeMovement(const Move& move, Board& board)
{
    board.movePiece(move.fromX, move.fromY, move.x, move.y);
}

// Check if AI has placed all pieces on the board
bool AIPlayer::allPiecesPlaced() const
{
    return donkeys == 0 && snake == 0 && frog == 0;
}
