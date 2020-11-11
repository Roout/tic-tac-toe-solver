#include "MCTS.hpp"

#include <cassert>
#include <chrono>

MCTS::MCTS(uint64_t timeLimit)
    : m_timeLimit(timeLimit)
    , m_engine()
{}

MCTS::Node* MCTS::Select(MCTS::Node* parent) const noexcept {
    Node * best { nullptr };
    return best;
}

MCTS::Node* MCTS::Expand(MCTS::Node* node) {
    auto actions { 0u };
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        auto row = i / 3;
        auto col = i % 3;
        actions += node->m_state.at(row, col) == '.';
    }
    // random in-tree policy
    auto randomAction = m_engine() % (actions + 1u);
    actions = 0u;
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        auto row = i / 3;
        auto col = i % 3;
        actions += node->m_state.at(row, col) == '.'; 
        if(actions >= randomAction) {
            // expand this node
            auto child = m_pool.Aquire();
            child->m_parent = node;
            child->m_symbol = node->m_symbol == 'x'? 'o': 'x';
            child->m_state = node->m_state;
            child->m_state.assign(row, col, child->m_symbol);
            node->m_children.emplace_back(child);
            return child;
        }
    }
    assert(false && "Cannot find node to expand");
    return nullptr;
}

MCTS::Node* MCTS::Simulate(MCTS::Node* node) {
    (void) node;
    assert(false && "Not implemented!");
    // use out-of-tree policy for play-out
    return nullptr;
}

void MCTS::UpdateScores(MCTS::Node * node) {
    (void) node;
    assert(false && "Not implemented!");
}

size_t MCTS::Run(game::Board board) {
    auto start = std::chrono::system_clock::now();
    auto timeLimit { m_timeLimit };
    // initialize root node
    auto root = m_pool.Aquire();
    // Iterate an algorithm
    while(timeLimit > 0) {
        // Selection
        auto leaf = this->Select(root);
        // Expansion
        if(this->IsTerminal(leaf)) {
            leaf = this->Expand(leaf);
        }
        // Simulation
        auto result = this->Simulate(leaf); // play-out
        // Backpropogation
        this->UpdateScores(result);
        // update timer
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        timeLimit -= elapsed;
    }
    
}

size_t MCTS::ExpandedNodesCount() const noexcept {

}