#ifndef SOLVER_HPP_
#define SOLVER_HPP_

#include "Board.hpp"

#include <functional>
#include <ostream>
#include <cstdint>

namespace solution {

class Solver {
public:
    using State_t = game::Board;
    using Mapping_t = std::function<State_t::Cell(uint8_t)>;

    /**
     * @param player identity (basicaly correspond to his turn in the game)
     * @param mapping maps player index to cell
     */
    Solver(uint8_t player, Mapping_t&& mapping)
        : m_player { player }
        , m_playerMapping { std::move(mapping) }
    {}

    ~Solver() = default;

    /**
     * Run minimax algorithm for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    virtual size_t Run(State_t state) = 0;

    virtual void Print(std::ostream& os) const = 0;

protected:

    virtual bool IsTerminal(State_t state) const noexcept;

    virtual uint8_t GetNextPlayer(uint8_t player) const noexcept;

protected:

    uint8_t     m_player{ 0 }; 
    Mapping_t   m_playerMapping{};
};

inline bool Solver::IsTerminal(State_t state) const noexcept {
    // can't continue
    return game::GetGameState(state).first != game::Board::State::ONGOING;
}

inline uint8_t Solver::GetNextPlayer(uint8_t player) const noexcept {
    return ++player & 1;
}

} // namespace solver

#endif // SOLVER_HPP_