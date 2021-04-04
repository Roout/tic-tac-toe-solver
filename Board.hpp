#ifndef BOARD_HPP
#define BOARD_HPP

#include <cassert>
// TODO: remove ostream from the header
#include <ostream>

// Contain game logic implementation
namespace game {

    class Board {
    public:
        enum Details: size_t { ROWS = 3, COLS = 3, SIZE = 9};
        enum class State: size_t { WIN_X, WIN_O, ONGOING, DRAW };
        // TODO: add enums to mark a player: 'o', 'x'

        constexpr char at(size_t row, size_t col) const noexcept {
            const auto bitIndex = row * Details::ROWS + col;
            return  (m_desk & (1U << bitIndex)) > 0U? 'x' : 
                    (m_desk & (1U << (bitIndex + Details::SIZE))) > 0U? 'o' : '.';
        }

        // TODO: add constexpr assignment and clear methods which will return the new board 
        // without modifying this one

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

        /**
         * @return the indication whethere the state of board is final or not, i.e.
         * returns true if the game is finished, false - ongoing!
         */
        constexpr bool finished() const noexcept {
            const size_t players[2] = {
                // clear the bits used by second player - 'o', 
                // leave the bits of player - 'x' untouched
                m_desk & (~((~(1U << Details::SIZE)) << Details::SIZE)),  // 'x'
                m_desk >> Details::SIZE             // 'o'
            };
            constexpr auto fullBoard { 0b111111111 };
            
            assert(players[0] <= fullBoard && "Wrong bit conversation");
            assert(players[1] <= fullBoard && "Wrong bit conversation");
            assert(((players[0] & players[1]) == 0U) && "Logic error: moves overlaped");
            
            return (players[0]|players[1]) == fullBoard;
        }

        // return an unsigned integer which represent the board
        constexpr size_t unwrap() const noexcept {
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

    inline std::ostream& operator<<(std::ostream& os, Board board) {
        for(size_t i = 0; i < Board::SIZE; i++) {
            os << board.at(i / Board::ROWS, i % Board::COLS);
            os << (i % Board::COLS == Board::COLS - 1U? "\n" : " | ");
        }
        return os;
    }
    
    inline Board::State GetGameState(Board board) noexcept {
        int freeSpaceCounter { 0 };
        // any column 
        for(size_t row = 0; row < Board::ROWS; row++) {
            int x { 0 }, o { 0 };
            for(size_t col = 0; col < Board::COLS; col++) {
                auto value { board.at(row, col) };
                x += value == 'x';
                o += value == 'o';
                freeSpaceCounter += value == '.';
            }
            if(x == 3) return Board::State::WIN_X;
            else if(o == 3) return Board::State::WIN_O;
        }
        if(!freeSpaceCounter) return Board::State::DRAW;

        // any row 
        for(size_t col = 0; col < Board::COLS; col++) {
            int x { 0 }, o { 0 };
            for(size_t row = 0; row < Board::ROWS; row++) {
                auto value { board.at(row, col) };
                x += value == 'x';
                o += value == 'o';
            }
            if(x == 3) return Board::State::WIN_X;
            else if(o == 3) return Board::State::WIN_O;
        }

        // main diag 
        int x { 0 }, o { 0 };
        for(size_t i = 0; i < game::Board::ROWS; i++) {
            const auto value = board.at(i, i);
            x += value == 'x';
            o += value == 'o';
        }
        if(x == 3) return Board::State::WIN_X;
        else if(o == 3) return Board::State::WIN_O;

        // Points from the other diagonal
        x = o = 0;
        for(size_t i = 0; i < game::Board::ROWS; i++) {
            const auto value = board.at(i, game::Board::ROWS - i - 1);
            x += value == 'x';
            o += value == 'o';
        }
        if(x == 3) return Board::State::WIN_X;
        else if(o == 3) return Board::State::WIN_O;

        return Board::State::ONGOING;
    }

}

void TestBoard();

#endif // BOARD_HPP