#include "Board.hpp"

#include <iostream>


void TestBoard() {
    std::cerr << "Test the board...\n";
    game::Board board;
    // is clear
    for(size_t i = 0; i < game::Board::ROWS; i++) {
        for(size_t j = 0; j < game::Board::COLS; j++) {
            assert(board.at(i, j) == '.' && "Unexpected default symbol at the board");
        }
    }

    // Assign something
    size_t xRows[] = { 2, 2, 2};
    size_t xCols[] = { 1, 0, 2};

    size_t oRows[] = { 1, 1, 1};
    size_t oCols[] = { 1, 0, 2};

    for(size_t i = 0; i < 3; i++) {
        board.assign(xRows[i], xCols[i], 'x');
        board.assign(oRows[i], oCols[i], 'o');
    }

    size_t xCount { 0 };
    size_t oCount { 0 };
    for(size_t i = 0; i < game::Board::ROWS; i++) {
        for(size_t j = 0; j < game::Board::COLS; j++) {
            if(board.at(i, j) == 'x') xCount++; 
            if(board.at(i, j) == 'o') oCount++; 
        }
    }
    assert(xCount == 3 && oCount == 3 && "Wrong number of known tockens");

    // clear 'x'
    for(size_t i = 0; i < 3; i++) {
        board.clear(xRows[i], xCols[i]);
    }
    for(size_t i = 0; i < 3; i++) {
        assert(board.at(xRows[i], xCols[i]) == '.' && "Failed to clear the board");
    }

    // finished
    for(size_t i = 0; i < game::Board::Details::SIZE; i++) {
       board.clear(i / 3u, i % 3); 
    }
    for(size_t i = 0; i < game::Board::Details::SIZE; i+=2) {
        board.assign(i/3u, i%3u, 'x');
    }
    for(size_t i = 1; i < game::Board::Details::SIZE; i+=2) {
        board.assign(i/3u, i%3u, 'o');
    }
    assert(board.finished() && "Failed finished board check");
    board.clear(1u,1u);
    assert(!board.finished() && "Failed finished board check");
    board.clear(1u,2u);
    assert(!board.finished() && "Failed finished board check");

    std::cerr << "Complete test.\n";
}
