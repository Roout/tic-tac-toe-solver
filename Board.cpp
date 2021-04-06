#include "Board.hpp"

namespace game {
    std::ostream& operator<<(std::ostream& os, Board board) {
        for(size_t i = 0; i < Board::SIZE; i++) {
            const auto cell = board.at(i / Board::ROWS, i % Board::COLS);
            os << (cell == Board::Cell::X ? 'x' : 
                (cell == Board::Cell::O? 'o' : '.'));
            os << (i % Board::COLS == Board::COLS - 1U? "\n" : " | ");
        }
        return os;
    }

    std::pair<Board::State, Board::Cell> GetGameState(Board board) noexcept {
        int freeSpaceCounter { 0 };
        int x { 0 }, o { 0 };

        // any column 
        for(size_t row = 0; row < Board::ROWS; row++) {
            x = o = 0;
            for(size_t col = 0; col < Board::COLS; col++) {
                auto value { board.at(row, col) };
                x += value == Board::Cell::X;
                o += value == Board::Cell::O;
                freeSpaceCounter += value == Board::Cell::FREE;
            }
            if(x == 3) return { Board::State::WIN, Board::Cell::X };
            else if(o == 3) return { Board::State::WIN, Board::Cell::O };
        }

        // any row 
        for(size_t col = 0; col < Board::COLS; col++) {
            x = o = 0;
            for(size_t row = 0; row < Board::ROWS; row++) {
                auto value { board.at(row, col) };
                x += value == Board::Cell::X;
                o += value == Board::Cell::O;
            }
            if(x == 3) return { Board::State::WIN, Board::Cell::X };
            else if(o == 3) return { Board::State::WIN, Board::Cell::O };
        }

        // main diag 
        x = o = 0;
        for(size_t i = 0; i < game::Board::ROWS; i++) {
            const auto value = board.at(i, i);
            x += value == Board::Cell::X;
            o += value == Board::Cell::O;
        }
        if(x == 3) return { Board::State::WIN, Board::Cell::X };
        else if(o == 3) return { Board::State::WIN, Board::Cell::O };

        // Points from the other diagonal
        x = o = 0;
        for(size_t i = 0; i < game::Board::ROWS; i++) {
            const auto value = board.at(i, game::Board::ROWS - i - 1);
            x += value == Board::Cell::X;
            o += value == Board::Cell::O;
        }
        if(x == 3) return { Board::State::WIN, Board::Cell::X };
        else if(o == 3) return { Board::State::WIN, Board::Cell::O };

        return (freeSpaceCounter? 
            std::make_pair(Board::State::ONGOING, Board::Cell::X) :  // second part of pair doesn't matter
            std::make_pair(Board::State::DRAW, Board::Cell::X) // second part of pair doesn't matter
        );
    }
}

void TestBoard() {
    using game::Board;
    std::cerr << "Test the board...\n";
    Board board;
    // is clear
    for(size_t i = 0; i < Board::ROWS; i++) {
        for(size_t j = 0; j < Board::COLS; j++) {
            assert(board.at(i, j) == Board::Cell::FREE && "Unexpected default symbol at the board");
        }
    }

    // Assign something
    size_t xRows[] = { 2, 2, 2};
    size_t xCols[] = { 1, 0, 2};

    size_t oRows[] = { 1, 1, 1};
    size_t oCols[] = { 1, 0, 2};

    for(size_t i = 0; i < 3; i++) {
        board.assign(xRows[i], xCols[i], Board::Cell::X);
        board.assign(oRows[i], oCols[i], Board::Cell::O);
    }

    size_t xCount { 0 };
    size_t oCount { 0 };
    for(size_t i = 0; i < Board::ROWS; i++) {
        for(size_t j = 0; j < Board::COLS; j++) {
            if(board.at(i, j) == Board::Cell::X) xCount++; 
            if(board.at(i, j) == Board::Cell::O) oCount++; 
        }
    }
    assert(xCount == 3 && oCount == 3 && "Wrong number of known tockens");

    // clear 'x'
    for(size_t i = 0; i < 3; i++) {
        board.clear(xRows[i], xCols[i]);
    }
    for(size_t i = 0; i < 3; i++) {
        assert(board.at(xRows[i], xCols[i]) == Board::Cell::FREE && "Failed to clear the board");
    }

    // finished
    for(size_t i = 0; i < Board::Details::SIZE; i++) {
       board.clear(i / 3u, i % 3); 
    }
    for(size_t i = 0; i < Board::Details::SIZE; i+=2) {
        board.assign(i/3u, i%3u, Board::Cell::X);
    }
    for(size_t i = 1; i < Board::Details::SIZE; i+=2) {
        board.assign(i/3u, i%3u, Board::Cell::O);
    }
    assert(board.finished() && "Failed finished board check");
    board.clear(1u,1u);
    assert(!board.finished() && "Failed finished board check");
    board.clear(1u,2u);
    assert(!board.finished() && "Failed finished board check");

    std::cerr << "Complete test.\n";
}
