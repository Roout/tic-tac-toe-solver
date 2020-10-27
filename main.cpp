#include <iostream>
#include <chrono>

#include "Board.hpp"
#include "Minimax.hpp"

using namespace game;

int main(int, char**) {
    Minimax algo {'o'};
    game::Board board {};
    
    auto IsFinished = [](game::Board board) {
        using State = game::Board::State;
        bool isFinished { false };
        switch(game::GetGameState(board)) {
            case State::WIN_X: {
                std::cout << "You win!\n";
                isFinished = true;
            } break;
            case State::WIN_O: {
                std::cout << "You lose!\n";
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

    // Simple tic-tac-toe game against minimax
    while(true) {
        std::cout << board;

        // Player move:
        std::cout << "Enter row {0, 1, 2} & column {0, 1, 2}: ";
        size_t row, col; 
        std::cin >> row >> col;
        board.assign(row, col, 'x');

        if(IsFinished(board)) {
            std::cout << board;
            break;
        }

        // AI move
        auto start = std::chrono::high_resolution_clock::now();
        auto move = algo.Run(board);
        auto finish = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        std::cout << "Took time to make decision: " << elapsed / 1'000.f << " s" << std::endl;
        board.assign(move / 3, move % 3, 'o');       
        if(IsFinished(board)) {
            std::cout << board;
            break;
        }
        // delimiter
        std::cout << "\n";
    }
    return 0;
}
