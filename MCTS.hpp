#ifndef MCTS_HPP
#define MCTS_HPP

#include "Board.hpp"

#include <cstdint>
#include <random>
#include <vector>
#include <functional>

/**
 * MCTS doesn't evaluate each node, only leaf 
 * Constrains: time & memory
 */
class MCTS final {
public:

    /**
     * @param timeLimit     (stop condition) timelimit for algorithm in microseconds
     * @param iterations    (stop condition) limit on number of algorithm's iterations (main while loop)
     * @param treeSize      (stop condition) max number of nodes algorithm can add to the tree
     * @param player        Define player identity for AI (next action in game is performed by htis player).
     *                      Belongs to integer range [0, 1] inclusive
     * @param playerMapping Maps player to board mark!
    */
    MCTS(uint64_t timeLimit
        , uint64_t iterations
        , uint64_t treeSize
        , uint8_t player
        , std::function<game::Board::Cell(uint8_t)> && playerMapping
    );

    /**
     * Run MCTS algorithm for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(game::Board board);

    /**
     * @return number of allocated nodes (nodes added to the tree)
    */
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
        uint8_t         m_player { 0 };
    };

    class Pool final {
    public:
        static constexpr size_t CAPACITY { 1u << 20u };

        Pool(size_t capacity) : 
            m_nodes { capacity }
        {}

        size_t Size() const noexcept {
            return m_size;
        }

        size_t Capacity() const noexcept {
            return m_nodes.capacity();
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
        std::vector<Node> m_nodes { CAPACITY };
        size_t m_size { 0u };
    };

    void Expand(Node* node);
   
    Node* Select(Node* node) const noexcept;

    float Simulate(Node* node);

    void Backup(Node* node, float reward);

    void BackupNegamax(Node* node, float reward);

    bool IsTerminal(Node* node) const noexcept;

    bool IsLeaf(const Node* node) const noexcept;

    // upper confidence bound of the tree
    float UCT(const Node* node, const Node* parent) const noexcept;

    uint8_t GetNextPlayer(uint8_t player) const noexcept;

private:
    // Time constrain for the algorithm (in microseconds)
    // Default value: 0.1s
    const uint64_t m_timeLimit { 100'000 }; 
    const uint64_t m_iterations { 2000 };
    const uint64_t m_treeSize { 10'000 };
    const uint8_t  m_player { 0 };
    // TODO: Cell - is a type used in BOARD (can be parameterized in template (from board))
    std::function<game::Board::Cell(uint8_t)> m_playerMapping;

    Pool m_pool { Pool::CAPACITY };
    std::mt19937 m_engine{};
};

inline bool MCTS::IsLeaf(const Node* node) const noexcept {
    return node->m_children.empty();
}

inline uint8_t MCTS::GetNextPlayer(uint8_t player) const noexcept {
    return ++player & 1;
}


#endif // MCTS_HPP