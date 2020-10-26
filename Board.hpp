#ifndef BOARD_HPP
#define BOARD_HPP

#include <cassert>
#include <ostream>

// Contain game logic implementation
namespace game {

    class Board {
    public:
        enum Details: size_t { ROWS = 3, COLS = 3, SIZE = 9};
        enum class State: size_t { WIN_X, WIN_O, ONGOING, DRAW };

        constexpr char at(size_t row, size_t col) const noexcept {
            const auto bitIndex = row * Details::ROWS + col;
            return  (m_desk & (1U << bitIndex)) > 0U? 'x' : 
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
            // clear 'x'
            m_desk &= ~(1U << bitIndex);
            // clear 'o'
            m_desk &= ~(1U << (bitIndex + Details::SIZE));
        }

        // return an unsigned integer which represent the board
        size_t unwrap() const noexcept {
            return m_desk;
        }

        friend std::ostream& operator<<(std::ostream&, Board board);

    private:
        // `size_t` is at least 32bit value, so let assign it's bits a specific value:
        // from lsb
        // [0...8]   - indicates whether the cell at i-th place is 'x' or not
        // [9...17]  - indicates whether the cell at i-th place is 'o' or not
        // [18...32 (64)] - unspecified
        size_t m_desk { 0U };
    };

    std::ostream& operator<<(std::ostream&, Board board);
    
    Board::State GetGameState(Board board) noexcept;
}

inline void TestBoard();

#endif // BOARD_HPP