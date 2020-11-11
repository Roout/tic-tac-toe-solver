#ifndef MCTS_HPP
#define MCTS_HPP

#include "Board.hpp"

#include <cstdint>
#include <random>
#include <vector>
#include <cassert>

struct policy {};
// random uniform-distributed expansion
struct default_policy : public policy {};
// expand all children
struct tree_policy : public policy {};

/**
 * MCTS doesn't evaluate each node, only leaf 
 * Constrains: time & memory
 */
class MCTS final {
public:

    MCTS(uint64_t timeLimit);

    /**
     * Run MCTS algorithm for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(game::Board board);

    size_t ExpandedNodesCount() const noexcept;

private:

    struct Node final {
        game::Board     m_state {};
        std::vector<Node*> m_children {}; 
        Node*           m_parent { nullptr };
        uint32_t        m_visits { 0u };
        float           m_score { 0.f };
        char            m_symbol {'#'};
    };

    class Pool final {
    public:

        bool IsFull() const noexcept {
            return m_size >= CAPACITY;
        }

        Node* Aquire() noexcept {
            assert(m_size < CAPACITY && "Out of allocated nodes!");
            m_size++;
            return &m_nodes[m_size - 1];
        }

    private:
        // Memory constrain
        static constexpr size_t CAPACITY { 1u << 20u };

        std::vector<Node> m_nodes { CAPACITY };
        size_t m_size { 0u };
    };

    
    Node* Expand(Node* node);

    Node* Select(Node* parent) const noexcept;

    Node* Simulate(Node* node);

    void UpdateScores(Node* node);

    bool IsTerminal(Node* node) const noexcept {
        // TODO: simplify game state test for the terminal node using XOR and AND
        return game::GetGameState(node->m_state) != game::Board::State::ONGOING;
    }
private:

    Pool m_pool;
    // Time constrain for the algorithm (in milliseconds)
    const uint64_t m_timeLimit { 1'000 }; 
    std::mt19937 m_engine{};
};



#endif // MCTS_HPP