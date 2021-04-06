#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include "Board.hpp"
#include <cstdint>
#include <functional>

/**
 * @note 
 * The Minimax require to evaluate each node 
 */
class Minimax {
public:
    using Heuristic_t = float;

    static constexpr Heuristic_t INFINITY { 1000000.f };

    /**
     * @param player identity of this(minimax algorighmt) player
     * @param playerMapping maps player index to cell
     */
    Minimax(
        uint8_t player
        , std::function<game::Board::Cell(uint8_t)> && playerMapping
    );

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

    uint8_t GetNextPlayer(uint8_t player) const noexcept;

private:
    // character which represent AI's mark
    uint8_t m_player { 1 };

    std::function<game::Board::Cell(uint8_t)> m_playerMapping;

    // statistics:
    // number of expanded nodes
    size_t m_expanded { 0u };
};

inline size_t Minimax::ExpandedNodesCount() const noexcept {
    return m_expanded;
}

inline uint8_t Minimax::GetNextPlayer(uint8_t player) const noexcept {
    return ++player & 1;
}

#endif // MINIMAX_HPP