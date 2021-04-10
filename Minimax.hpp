#ifndef MINIMAX_HPP_
#define MINIMAX_HPP_

#include "Solver.hpp"

namespace solution {

class Minimax : public Solver {
public:

    /**
     * @param player identity (basicaly correspond to his turn in the game)
     * @param mapping maps player index to cell
     */
    Minimax(uint8_t player, Mapping_t&& mapping);

    /**
     * Run minimax algorithm for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(State_t state) override;

    void Print(std::ostream& os) const override;

protected:

    float GetHeuristic(State_t node, int depth) const noexcept;

protected:
    // statistics:
    // number of opened nodes
    size_t m_expanded { 0u };

private:

    float Apply(State_t, int depth, bool isMaximizingPlayer);

};

class AlphaBettaMinimax: public Minimax {
public:
    static constexpr float INF { 1000000.f };

    using Minimax::Minimax;

    /**
     * Run minimax algorithm with alpha-beta pruning for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(State_t state) override;

private:
    float Apply(State_t
        , int depth
        , float alpha
        , float beta
        , bool isMaximizingPlayer
    );
};

} // namespace solution

#endif // MINIMAX_HPP_