#ifndef MCTS_HPP
#define MCTS_HPP

#include "Board.hpp"
#include "ElementPool.hpp"
#include "Solver.hpp"

#include <cstdint>
#include <random>

namespace solution {

// TODO: remove parent from the node
// TODO: get rid of the fucking std::vector
struct Node final {
    Solver::State_t m_state {};
    std::vector<Node*> m_children {}; 
    Node*           m_parent { nullptr };
    uint32_t        m_visits { 0u };
    float           m_reward { 0.f };
    uint8_t         m_player { 0 };
};

/**
 * MCTS doesn't evaluate each node, only leaf 
 * Constrains: time & memory
 */
class MCTS final : public Solver {
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
        , Mapping_t && playerMapping
    );

    /**
     * Run MCTS algorithm for the given board state
     * @param board is a current game state
     * @return the best move. To extract row and col do the following:
     * - row = return_value / 3; 
     * - col = return_value % 3
     */
    size_t Run(State_t board) override;

    void Print(std::ostream& os) const override;

private:

    void Expand(Node* node);
   
    Node* Select(Node* node) const noexcept;

    float Simulate(Node* node);

    void Backup(Node* node, float reward);

    void BackupNegamax(Node* node, float reward);

    bool IsLeaf(const Node* node) const noexcept;

    // upper confidence bound of the tree
    float UCT(const Node* node, const Node* parent) const noexcept;

private:
    // Time constrain for the algorithm (in microseconds)
    // Default value: 0.1s
    const uint64_t m_timeLimit { 100'000 }; 
    const uint64_t m_iterations { 2000 };
    const uint64_t m_treeSize { 10'000 };

    ElementPool<Node> m_pool{};
    std::mt19937 m_engine{};
};

inline bool MCTS::IsLeaf(const Node* node) const noexcept {
    return node->m_children.empty();
}

} // namespace solution

#endif // MCTS_HPP