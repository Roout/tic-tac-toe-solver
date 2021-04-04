#include "MCTS.hpp"

#include <cassert>
#include <chrono>
#include <algorithm>

#include <iostream>

MCTS::MCTS(int64_t timeLimit, char mark)
    : m_timeLimit { timeLimit }
    , m_mark { mark }
    , m_engine{}
{}

float MCTS::UCT(MCTS::Node* node, MCTS::Node* parent) const noexcept {
    const auto C = 1.4142135f; // sqrtf(2.f);
    const auto exploit = node->m_reward / node->m_visits;
    const auto explore = sqrtf(2.f * logf(static_cast<float>(parent->m_visits)) / static_cast<float>(node->m_visits));
    return exploit + C * explore;
}

// recursively selected node using utility function
// return selected base on utility function node (it can be 
// either terminal either non-expanded) 
MCTS::Node* MCTS::Select(MCTS::Node* node) const noexcept {
    if(IsLeaf(node) || IsTerminal(node)) return node;

    Node *bestNode { nullptr };
    auto bestUCT = 0.f; 
    for(auto child: node->m_children) {
        if(!child->m_visits) return child;
        const auto uct = this->UCT(child, node);
        if(bestNode == nullptr) {
            bestNode = child;
            bestUCT = uct;
        }
        else if(uct > bestUCT) {
            bestNode = child;
            bestUCT = uct;
        }
    }

    assert(bestNode != nullptr && "Can't be equal NULL");
    return this->Select(bestNode);
}

bool MCTS::IsLeaf(MCTS::Node* node) const noexcept {
    return node->m_children.empty();
}

// expand selected node adding all possible children
void MCTS::Expand(MCTS::Node* node) {
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        auto row = i / 3;
        auto col = i % 3;
        // is empty so we can take action
        if(node->m_state.at(row, col) == '.') {
            auto child = m_pool.Aquire();
            child->m_parent = node;
            child->m_symbol = node->m_symbol == 'x'? 'o': 'x';
            child->m_state = node->m_state;
            child->m_state.assign(row, col, child->m_symbol);
            node->m_children.emplace_back(child);
        }
    }
}

// Is run from expanded node and return reward
float MCTS::Simulate(MCTS::Node* expanded) {
    // use out-of-tree policy for play-out
    auto state = expanded->m_state;
    auto mark = expanded->m_symbol;
    while(!state.finished()) {
        // perform random action
        std::vector<size_t> actions;
        actions.reserve(game::Board::SIZE - 1);
        for(size_t i = 0; i < game::Board::SIZE; i++) {
            auto row = i / 3;
            auto col = i % 3;
            // is empty so we can take action
            if(state.at(row, col) == '.') {
                actions.push_back(i);
            }
        }
        // choose action
        size_t randomAction = m_engine() % actions.size();
        mark = (mark == 'x'? 'o' : 'x');
        state.assign(actions[randomAction] / 3, actions[randomAction] % 3, mark);
    }
    float reward = 0.f;
    switch(game::GetGameState(state)) {
        case game::Board::State::WIN_X: reward = (m_mark == 'x'? 1.f: 0.f); break;
        case game::Board::State::WIN_O: reward = (m_mark == 'o'? 1.f: 0.f); break;
        case game::Board::State::DRAW:  reward = 0.1f; break;
        case game::Board::State::ONGOING: assert(false && "[ERROR] unreachable state!"); break;
        default: break;
    }
    return reward;
}

void MCTS::Backup(MCTS::Node* node, float reward) {
    assert(node && "[ERROR] can't backup nullptr!");
    node->m_reward += reward;
    node->m_visits++;

    if(node->m_parent) {
        this->Backup(node->m_parent, reward);
    }
}

size_t MCTS::Run(game::Board board) {
    auto start = std::chrono::system_clock::now();
    int64_t elapsedTime { 0LL };
    
    // initialize root node
    auto root = m_pool.Aquire();
    root->m_state = board;
    root->m_symbol = (m_mark == 'x'? 'o' : 'x');
    this->Expand(root);

    // Iterate an algorithm
    while(elapsedTime < m_timeLimit) {
        auto selected = this->Select(root);
        if(!this->IsTerminal(selected)) {
            assert(IsLeaf(selected) && "[ERROR] Unexpected node was selected!");
            this->Expand(selected);
            assert(!selected->m_children.empty() && "[ERROR] problems with expand function!");
            // select random value base on random tree-policy
            const auto randomChild = m_engine() % selected->m_children.size();
            selected = selected->m_children[randomChild];
        }
        auto reward = this->Simulate(selected);
        this->Backup(selected, reward);
        // update timer
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        elapsedTime += elapsed;
    }
    
    // chose best action
    auto max = std::max_element(root->m_children.cbegin(), root->m_children.cend(), [](Node* lhs, Node*rhs) {
        return lhs->m_reward < rhs->m_reward;
    });
    std::cerr << "visits-rewards: ";
    for(auto node: root->m_children) {
        std::cerr << "{" << node->m_visits << ", " << node->m_reward << "}, ";
    }
    assert(max != root->m_children.cend());
    auto state = (*max)->m_state;

    size_t bestMove = 0;
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        auto row = i / 3;
        auto col = i % 3;
        if(board.at(row, col) != state.at(row, col)) {
            bestMove = i;
            break;
        }
    }
    return bestMove;
}

size_t MCTS::ExpandedNodesCount() const noexcept {
    return m_pool.Size();
}
