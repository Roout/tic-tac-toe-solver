#include "Minimax.hpp"

#include <iostream>
#include <limits>

using game::operator<<;

Minimax::Minimax(char value) :
    m_symbol { value },
    m_opponent { value == 'x'? 'o': 'x' }
{
}

size_t Minimax::Run(game::Board board) {
    // Look through all possible moves
    // and choose the one with best heuristic value.
    auto bestHeuristic { -10000.f };
    size_t bestMove { 0 };
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        const auto row = i / 3;
        const auto col = i % 3;
        if (board.at(row, col) == '.') {
            board.assign(row, col, m_symbol);
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

float Minimax::Apply(game::Board target, int depth, bool isMaximizingPlayer) {
    if (!depth || this->IsTerminal(target)) {
        return this->GetHeuristic(target, depth);
    }
    if (isMaximizingPlayer) {
        auto heuristic = -10000.f;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(target.at(row, col) == '.') {
                target.assign(row, col, m_symbol);
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
            if(target.at(row, col) == '.') {
                target.assign(row, col, m_opponent);
                heuristic = std::min(heuristic, this->Apply(target, depth - 1, true));
                target.clear(row, col);
            }
        }
        return heuristic;
    }
}

bool Minimax::IsTerminal(game::Board state) const noexcept {
    using State = game::Board::State;
    return game::GetGameState(state) != State::ONGOING; // don't have any children
}

float Minimax::GetHeuristic(game::Board state, int depth) const noexcept {
    using State = game::Board::State;

    int score = 0;
    auto result = game::GetGameState(state);
    switch(result) {
        case State::ONGOING: [[fallthrough]]
        case State::DRAW: score = depth; break;
        case State::WIN_X: score = m_symbol == 'x'? 20 + depth: -20 - depth; break;
        case State::WIN_O: score = m_symbol == 'o'? 20 + depth: -20 - depth; break;
        default: break;
    }
    return static_cast<float>(score);
}
