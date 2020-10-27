#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include "Board.hpp"

class Minimax {
public:
    /**
     * @param minimaxMark indicate which mark does algorithm use for itself ('x' by default)
     */
    Minimax(char minimaxMark);

    /**
     * Run minimax algorithm with the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(game::Board board);

private:

    float Apply(game::Board, int depth, bool isMaximizingPlayer);

    bool IsTerminal(game::Board node) const noexcept;

    float GetHeuristic(game::Board node, int depth) const noexcept;

private:
    // character which represent AI's mark
    char m_symbol { 'x' };
    // mark of the opponent
    char m_opponent { 'o' };
};

#endif // MINIMAX_HPP