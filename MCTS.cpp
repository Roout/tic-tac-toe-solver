#include "MCTS.hpp"

#include <cassert>
#include <chrono>
#include <algorithm>

#include <iostream>

MCTS::MCTS(uint64_t timeLimit
    , uint64_t iterations
    , uint64_t treeSize
    , uint8_t player
    , std::function<game::Board::Cell(uint8_t)> && playerMapping
)
    : m_timeLimit { timeLimit }
    , m_iterations { iterations }
    , m_treeSize { treeSize }
    , m_player { player }
    , m_playerMapping { std::move(playerMapping) }
    , m_engine{}
{
    assert(m_treeSize + game::Board::SIZE < m_pool.Capacity() 
        && "Can't be greater or equal to memory pool capacity because expansion may fail to get Node");
    assert(m_player <= 1 
        && "Player ID must belong to range [0, 1");
}

float MCTS::UCT(const Node* node, const Node* parent) const noexcept {
    const auto C = 2.f;
    const auto exploit = node->m_reward / node->m_visits;
    const auto explore = sqrtf(logf(static_cast<float>(parent->m_visits)) / static_cast<float>(node->m_visits));
    return exploit + C * explore;
}

// recursively selected node using utility function
// return selected base on utility function node (it can be 
// either terminal either non-expanded) 
MCTS::Node* MCTS::Select(Node* node) const noexcept {
    while(!IsLeaf(node) && !IsTerminal(node)) {
        // avoid std::max_element because it performs too many useless calls to UCT
        Node *bestNode { nullptr };
        auto bestUCT = 0.f; 
        for(auto child: node->m_children) {
            if(!child->m_visits) {
                bestNode = child;
                break;
            }
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
        node = bestNode;
    }
    return node;
}

bool MCTS::IsTerminal(Node* node) const noexcept {
    return (game::GetGameState(node->m_state).first != game::Board::State::ONGOING);
}

// expand selected node adding all possible children
void MCTS::Expand(Node* node) {
    for(size_t i = 0; i < game::Board::SIZE; i++) {
        auto row = i / 3;
        auto col = i % 3;
        // is empty so we can take action
        if(node->m_state.at(row, col) == game::Board::Cell::FREE) {
            auto child = m_pool.Acquire();
            *child = MCTS::Node{};
            child->m_parent = node;
            child->m_player = this->GetNextPlayer(node->m_player);
            child->m_state = node->m_state;
            child->m_state.assign(row, col, m_playerMapping(child->m_player));
            node->m_children.emplace_back(child);
        }
    }
}

// Is run from expanded node and return reward
float MCTS::Simulate(Node* expanded) {
    using game::Board;
    // use out-of-tree policy for play-out
    auto state = expanded->m_state;
    auto player = expanded->m_player;
    auto gameState = game::GetGameState(state);
    while(gameState.first == Board::State::ONGOING) {
        // perform random action
        std::vector<size_t> actions;
        actions.reserve(Board::SIZE - 1);
        for(size_t i = 0; i < Board::SIZE; i++) {
            const auto row = i / 3;
            const auto col = i % 3;
            // is empty so we can take action
            if(state.at(row, col) == Board::Cell::FREE) {
                actions.push_back(i);
            }
        }
        // choose action
        const size_t randomAction = m_engine() % actions.size();
        player = this->GetNextPlayer(player);
        state.assign(actions[randomAction] / 3, actions[randomAction] % 3, m_playerMapping(player));
        gameState = game::GetGameState(state);
    }
    float reward = 0.f;
    switch(gameState.first) {
        case Board::State::WIN: {
            const auto winner = static_cast<decltype(m_player)>(gameState.second);
            reward = (m_player == winner? 1.f: 0.f); 
        } break;
        case Board::State::DRAW: {
            reward = 0.3f; 
        } break;
        case Board::State::ONGOING: {
            assert(false && "[ERROR] unreachable state!"); 
        } break;
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

void MCTS::BackupNegamax(Node* node, float reward) {
    assert(node && "[ERROR] can't backup nullptr!");
    node->m_reward += node->m_player == m_player? reward : -reward;
    node->m_visits++;

    if(node->m_parent) {
        this->BackupNegamax(node->m_parent, reward);
    }
}

size_t MCTS::Run(game::Board board) {
    m_pool.Reset();

    const auto start = std::chrono::system_clock::now();
    uint64_t elapsedTime { 0LL };
    uint64_t simulationLimit { 5000 };
    
    // initialize root node
    const auto root = m_pool.Acquire();
    *root = MCTS::Node{};
    root->m_state = board;
    // init with opponent
    root->m_player = this->GetNextPlayer(m_player);

    // Iterate an algorithm
    while(simulationLimit > 0 
        && elapsedTime < m_timeLimit 
        && m_treeSize < m_pool.Capacity()
    ) {
        simulationLimit--;
        auto selected = this->Select(root);
        if(!this->IsTerminal(selected)) {
            assert(IsLeaf(selected) && "[ERROR] Unexpected node was selected!");
            this->Expand(selected);
            assert(!selected->m_children.empty() && "[ERROR] problems with expand function!");
            // select random value base on random tree-policy
            const auto randomChild = m_engine() % selected->m_children.size();
            selected = selected->m_children[randomChild];
        }

        const auto reward = this->Simulate(selected);
        this->BackupNegamax(selected, reward);
        // update timer
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        elapsedTime += static_cast<uint64_t>(elapsed);
    }

    std::cerr << "Visits: " << root->m_visits << "; Reward: " << root->m_reward << '\n';
    // chose best action
    auto max = std::max_element(root->m_children.cbegin(), root->m_children.cend(), [](Node* lhs, Node*rhs) {
        return lhs->m_reward < rhs->m_reward;
    });
    std::cerr << "visits-rewards: ";
    for(auto node: root->m_children) {
        std::cerr << "{" << node->m_visits << ", " << node->m_reward << "}, ";
    }
    std::cerr << '\n';

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
