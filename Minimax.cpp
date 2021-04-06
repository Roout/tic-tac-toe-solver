#include "Minimax.hpp"

#include <iostream>
#include <limits>

using game::operator<<;

Minimax::Minimax(
    uint8_t player
    , std::function<game::Board::Cell(uint8_t)> && playerMapping
) 
    : m_player { player }
    , m_playerMapping { std::move(playerMapping) }
{
    assert(m_player <= 1);
}

size_t Minimax::Run(game::Board board) {
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

size_t Minimax::RunAlphaBeta(game::Board board) {
    using game::Board;

    m_expanded = 0u;
    // Look through all possible moves
    // and choose the one with best heuristic value.
    auto bestHeuristic { -INFINITY };
    size_t bestMove { 0 };
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        const auto row = i / 3;
        const auto col = i % 3;
        if (board.at(row, col) == Board::Cell::FREE) {
            m_expanded++;
            board.assign(row, col, m_playerMapping(m_player));
            auto heuristic { this->Apply(board, 8, -INFINITY, +INFINITY, false) };
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

 Minimax::Heuristic_t Minimax::Apply(
    game::Board target
    , int depth
    , Heuristic_t alpha
    , Heuristic_t beta
    , bool isMaximizingPlayer
) {
    using game::Board;

    if (!depth || this->IsTerminal(target)) {
        return this->GetHeuristic(target, depth);
    }
    if (isMaximizingPlayer) {
        Heuristic_t heuristic = -INFINITY;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(target.at(row, col) == Board::Cell::FREE) {
                m_expanded++;
                target.assign(row, col, m_playerMapping(m_player));
                heuristic = std::max(heuristic, this->Apply(target, depth - 1, alpha, beta, false));
                alpha = std::max(heuristic, alpha);
                if(alpha >= beta) {
                    break;
                }
                target.clear(row, col);
            }
        }
        return heuristic;
    }
    else {
        Heuristic_t heuristic = INFINITY;
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            if(target.at(row, col) == Board::Cell::FREE) {
                m_expanded++;
                target.assign(row, col, m_playerMapping(GetNextPlayer(m_player)));
                heuristic = std::min(heuristic, this->Apply(target, depth - 1, alpha, beta, true));
                beta = std::min(heuristic, beta);
                if(beta <= alpha) {
                    break;
                }
                target.clear(row, col);
            }
        }
        return heuristic;
    }
}

 Minimax::Heuristic_t Minimax::Apply(game::Board target, int depth, bool isMaximizingPlayer) {
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

bool Minimax::IsTerminal(game::Board state) const noexcept {
    using State = game::Board::State;
    return game::GetGameState(state).first != State::ONGOING; // don't have any children
}

 Minimax::Heuristic_t Minimax::GetHeuristic(game::Board state, int depth) const noexcept {
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
