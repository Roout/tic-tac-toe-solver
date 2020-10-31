#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include "Board.hpp"

class Minimax {
public:
    using Heuristic_t = float;

    static constexpr Heuristic_t INFINITY { 1000000.f };

    /**
     * @param minimaxMark indicate which mark does algorithm use for itself ('x' by default)
     */
    Minimax(char minimaxMark);

    /**
     * Run minimax algorithm for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(game::Board board);

    /**
     * Run minimax algorithm with alpha-beta pruning for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t RunAlphaBeta(game::Board board);

    size_t ExpandedNodesCount() const noexcept;

private:

    Heuristic_t Apply(game::Board, int depth, bool isMaximizingPlayer);

    Heuristic_t Apply(game::Board, int depth, Heuristic_t alpha, Heuristic_t beta, bool isMaximizingPlayer);

    bool IsTerminal(game::Board node) const noexcept;

    Heuristic_t GetHeuristic(game::Board node, int depth) const noexcept;

private:
    // character which represent AI's mark
    char m_symbol { 'x' };
    // mark of the opponent
    char m_opponent { 'o' };

    // statistics:
    // number of expanded nodes
    size_t m_expanded { 0u };
};

inline size_t Minimax::ExpandedNodesCount() const noexcept {
    return m_expanded;
}


#endif // MINIMAX_HPP