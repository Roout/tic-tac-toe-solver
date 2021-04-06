#ifndef MCTS_HPP
#define MCTS_HPP

#include "Board.hpp"

#include <cstdint>
#include <random>
#include <vector>
#include <cassert>

/**
 * MCTS doesn't evaluate each node, only leaf 
 * Constrains: time & memory
 */
class MCTS final {
public:

    MCTS(int64_t timeLimit, char mark);

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

    // TODO: remove parent from the node
    // TODO: get rid of the fucking std::vector
    struct Node final {
        game::Board     m_state {};
        std::vector<Node*> m_children {}; 
        Node*           m_parent { nullptr };
        uint32_t        m_visits { 0u };
        float           m_reward { 0.f };
        char            m_symbol {'#'};
    };

    class Pool final {
    public:

        size_t Size() const noexcept {
            return m_size;
        }

        void Reset() noexcept {
            m_size = 0;
        }

        bool IsFull() const noexcept {
            return m_size >= CAPACITY;
        }

        Node* Acquire() noexcept {
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

    void Expand(Node* node);
   
    Node* Select(Node* node) const noexcept;

    float Simulate(Node* node);

    void Backup(Node* node, float reward);

    void BackupNegamax(Node* node, float reward);

    bool IsTerminal(Node* node) const noexcept {
        return (game::GetGameState(node->m_state) != game::Board::State::ONGOING);
    }

    bool IsLeaf(Node* node) const noexcept;

    // upper confidence bound of the tree
    float UCT(Node* node, Node* parent) const noexcept;

private:
    // Time constrain for the algorithm (in milliseconds)
    const int64_t m_timeLimit { 1'000 }; 
    const char m_mark { '#' };

    Pool m_pool;
    std::mt19937 m_engine{};
};



#endif // MCTS_HPP