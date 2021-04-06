#include <iostream>
#include <chrono>

#include "Board.hpp"
#include "Minimax.hpp"
#include "MCTS.hpp"

using namespace game;

int main(int, char**) {
    TestBoard();

    auto IsFinished = [](Board board) {
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
    };
    
    uint64_t microsecs = 16'666'666;
    uint64_t iterations = 5000;
    uint64_t nodes = 10000;
    uint8_t player = 1;
    auto playerMapping = [](uint8_t player) {
        return player == 0? Board::Cell::X : Board::Cell::O;
    };

    MCTS algo { microsecs, iterations, nodes, player, std::move(playerMapping) };
    // Minimax algo { player, std::move(playerMapping) };
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
        auto start = std::chrono::high_resolution_clock::now();
        auto move = algo.Run(board);
        auto finish = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        std::cout << "Took time to make decision: " << elapsed / 1'000.f << " s" << std::endl;
        std::cout << "Expand: " << algo.ExpandedNodesCount() << " nodes\n";
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
