#include <iostream>
#include <chrono>
#include <memory>

#include "Board.hpp"
#include "Minimax.hpp"
#include "MCTS.hpp"

using namespace game;

bool IsFinished(Board board) {
    using State = Board::State;
    bool isFinished { false };
    const auto result = GetGameState(board);
    switch(result.first) {
        case State::WIN: {
            std::cout << (result.second == Board::Cell::X? "You win!\n": "You lose!\n");
            isFinished = true;
        } break;
        case State::DRAW: {
            std::cout << "Draw!\n";
            isFinished = true;
        } break;
        case State::ONGOING: break;
        default: break;
    }
    return isFinished;
}

int main(int, char**) {
    TestBoard();
    
    uint64_t microsecs = 16'666;
    uint64_t iterations = 5000;
    uint64_t nodes = 10000;
    uint8_t player = 1;
    auto playerMapping = [](uint8_t player) {
        return player == 0? Board::Cell::X : Board::Cell::O;
    };

    enum Kind { kMCTS, kAlphaBetta, kMinimax };
    using SolverPointer = std::unique_ptr<solution::Solver>;
    SolverPointer algos[3] = {
        SolverPointer { new solution::MCTS { microsecs, iterations, nodes, player, std::move(playerMapping) } }
        , SolverPointer { new solution::AlphaBettaMinimax { player, std::move(playerMapping) } }
        , SolverPointer { new solution::Minimax { player, std::move(playerMapping) } }
    };
    Board board {};
    while(true) {
        std::cout << board;

        // Player move:
        std::cout << "Enter row {0, 1, 2} & column {0, 1, 2}: ";
        size_t row, col; 
        std::cin >> row >> col;
        board.assign(row, col, Board::Cell::X);
        std::cout << board;

        if(IsFinished(board)) break;

        // AI move
        auto move = algos[kAlphaBetta]->Run(board);
        algos[kAlphaBetta]->Print(std::cout);
        board.assign(move / 3, move % 3, Board::Cell::O);       
        if(IsFinished(board)) {
            std::cout << board;
            break;
        }
        // delimiter
        std::cout << "\n";
    }
    return 0;
}
