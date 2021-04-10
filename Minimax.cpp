#include "Minimax.hpp"
#include <cassert>
#include <algorithm>

namespace solution {

Minimax::Minimax(
    uint8_t player
    , Mapping_t && playerMapping
) 
    : Solver { player, std::move(playerMapping) }
{
    assert(m_player <= 1);
}

size_t Minimax::Run(State_t board) {
    using game::Board;
    m_expanded = 0u;
    // Look through all possible moves
    // and choose the one with best heuristic value.
    auto bestHeuristic { -10000.f };
    size_t bestMove { 0 };
    for(size_t i = 0; i < Board::SIZE; i++) {
        const auto row = i / 3;
        const auto col = i % 3;
        if (board.at(row, col) == Board::Cell::FREE) {
            m_expanded++;
            board.assign(row, col, m_playerMapping(m_player));
            auto heuristic { this->Apply(board, 8, false) };
            if (bestHeuristic < heuristic) {
                bestHeuristic = heuristic;
                bestMove = i;
            }
            board.clear(row, col);
        }
    }
    std::cerr << "choosen heuristic: " << bestHeuristic << std::endl;
    return bestMove;
}

float Minimax::Apply(State_t target, int depth, bool isMaximizingPlayer) {
    using game::Board;

    if (!depth || this->IsTerminal(target)) {
        return this->GetHeuristic(target, depth);
    }
    if (isMaximizingPlayer) {
        auto heuristic = -10000.f;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(target.at(row, col) == Board::Cell::FREE) {
                m_expanded++;
                target.assign(row, col, m_playerMapping(m_player));
                heuristic = std::max(heuristic, this->Apply(target, depth - 1, false));
                target.clear(row, col);
            }
        }
        return heuristic;
    }
    else {
        auto heuristic = 10000.f;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(target.at(row, col) == Board::Cell::FREE) {
                m_expanded++;
                target.assign(row, col, m_playerMapping(GetNextPlayer(m_player)));
                heuristic = std::min(heuristic, this->Apply(target, depth - 1, true));
                target.clear(row, col);
            }
        }
        return heuristic;
    }
}

void Minimax::Print(std::ostream& os) const {
    os << "Look through: " << m_expanded << " nodes\n";
}

size_t AlphaBettaMinimax::Run(State_t board) {
    using game::Board;

    m_expanded = 0u;
    // Look through all possible moves
    // and choose the one with best heuristic value.
    auto bestHeuristic { -INF };
    size_t bestMove { 0 };
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        const auto row = i / 3;
        const auto col = i % 3;
        if (board.at(row, col) == Board::Cell::FREE) {
            m_expanded++;
            board.assign(row, col, m_playerMapping(m_player));
            auto heuristic { this->Apply(board, 8, -INF, +INF, false) };
            if (bestHeuristic < heuristic) {
                bestHeuristic = heuristic;
                bestMove = i;
            }
            board.clear(row, col);
        }
    }
    std::cerr << "choosen heuristic: " << bestHeuristic << std::endl;
    return bestMove;
}

float AlphaBettaMinimax::Apply(
    State_t state
    , int depth
    , float alpha
    , float betta
    , bool isMaximizingPlayer
) {
    using game::Board;


    if (!depth || this->IsTerminal(state)) {
        return this->GetHeuristic(state, depth);
    }
    if (isMaximizingPlayer) {
        float heuristic = -INF;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(state.at(row, col) == Board::Cell::FREE) {
                m_expanded++;
                state.assign(row, col, m_playerMapping(m_player));
                heuristic = std::max(heuristic, this->Apply(state, depth - 1, alpha, betta, false));
                alpha = std::max(heuristic, alpha);
                if(alpha >= betta) {
                    break;
                }
                state.clear(row, col);
            }
        }
        return heuristic;
    }
    else {
        float heuristic = INF;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(state.at(row, col) == Board::Cell::FREE) {
                m_expanded++;
                state.assign(row, col, m_playerMapping(GetNextPlayer(m_player)));
                heuristic = std::min(heuristic, this->Apply(state, depth - 1, alpha, betta, true));
                betta = std::min(heuristic, betta);
                if(betta <= alpha) {
                    break;
                }
                state.clear(row, col);
            }
        }
        return heuristic;
    }
}


float Minimax::GetHeuristic(State_t state, int depth) const noexcept {
    using State = game::Board::State;

    int score = 0;
    auto result = game::GetGameState(state);
    switch(result.first) {
        case State::ONGOING: case State::DRAW: score = depth; break;
        case State::WIN: score = m_player == static_cast<uint8_t>(result.second)? 20 + depth: -20 - depth; break;
        default: break;
    }
    return static_cast<float>(score);
}

}