#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include "Board.hpp"
#include <array>
#include <vector>

class Minimax {
public:
    struct Node {
        game::Board state {};
        // Keeps IDs of the children. 
        // To access a child with `id` => m_nodes[id]
        std::array<size_t, 9U> children {};
        // Number of expanded children
        size_t count { 0U }; 
        float heuristic { 0.f };
    };

    /**
     * @param minimaxMark indicate which mark does algorithm use for itself ('x' by default)
     */
    Minimax(char minimaxMark);

    /**
     * Run minimax algorithm with the given board state
     * @param board is a current game state
     */
    void Start(game::Board board);

    /**
     * Find best move after algorithm done with evaluation 
     */
    game::Board GetBestMove() const noexcept;

private:
    float Apply(Node& target, int depth, bool isMaximizingPlayer);

    bool IsTerminal(const Node & node) const noexcept;

    float GetHeuristic(const Node & node, int depth) const noexcept;

    void Expand(Node& target, bool isMaximizing) noexcept;

private:
    // number of nodes in the pool
    inline static const size_t CAPACITY = 1U << 20U ;

    // character which represent AI's mark
    char m_symbol { 'x' };
    // mark of the opponent
    char m_opponent { 'o' };
    // pool of nodes (they are created on the heap)
    // Note: static array can't allocate more than 1MB on the stack without magic
    // so I use the heap
    std::vector<Node> m_nodes;
    // number of used nodes
    size_t m_count { 0 };
};

#endif // MINIMAX_HPP