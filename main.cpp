#include <iostream>
#include <array>
#include <cassert>

#define TEST

// Contain game logic implementation
namespace game {
    class Board {
    public:
        enum Details: size_t { ROWS = 3, COLS = 3, SIZE = 9};

        constexpr char at(size_t row, size_t col) const noexcept {
            const auto bitIndex = row * Details::ROWS + col;
            return (m_desk & (1U << bitIndex)) > 0U? 'x' : 
                    (m_desk & (1U << (bitIndex + Details::SIZE))) > 0U? 'o' : '.';
        }

        void assign(size_t row, size_t col, char value) noexcept {
            assert((value == 'x' || value == 'o') && "Trying to assign unknown tocken");

            const auto bitIndex = row * Details::ROWS + col;
            m_desk |= value == 'x'? (1U << bitIndex) : 
                        value == 'o'? (1U << (bitIndex + Details::SIZE)): 0U;
        }

        void clear(size_t row, size_t col) noexcept {
            const auto bitIndex = row * Details::ROWS + col;
            m_desk &= ~(1U << bitIndex);
            m_desk &= ~(1U << (bitIndex + Details::SIZE));
        }

        friend std::ostream& print(std::ostream&, Board board);

    private:
        // `size_t` is at least 32bit value, so let assign it's bits a specific value:
        // from lsb
        // [0...8]   - indicates whether the cell at i-th place is 'x' or not
        // [9...17]  - indicates whether the cell at i-th place is 'o' or not
        // [18...32 (64)] - unspecified
        size_t m_desk { 0U };
    };

    std::ostream& print(std::ostream& os, Board board) {
        for(size_t i = 0; i < Board::SIZE; i++) {
            os << board.at(i / Board::ROWS, i % Board::COLS);
            os << (i % Board::COLS == Board::COLS - 1U? "\n" : " | ");
        }
        return os;
    }
}

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

    std::cerr << "Complete test.\n";
}

int main(int, char**) {
#ifdef TEST
    TestBoard();
#endif
    
    return 0;
}
