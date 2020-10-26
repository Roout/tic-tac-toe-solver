#include "Minimax.hpp"

#include <iostream>
#include <cmath>
#include <limits>

Minimax::Minimax(char value) :
    m_symbol { value },
    m_opponent { value == 'x'? 'o': 'x' },
    m_nodes { CAPACITY }
{
}

void Minimax::Start(game::Board board) {
    m_count = 1U; // root node is already there
    m_nodes[0].state = board;
    m_nodes[0].count = 0;
    m_nodes[0].heuristic = this->Apply(m_nodes[0], 9, true);
    
    std::cerr << "choosen heuristic: " << m_nodes[0].heuristic << std::endl;
    std::cerr << "expanded nodes: " << m_count << " * " << sizeof(Node) << " = " << m_count * sizeof(Node) << " bytes\n";
}

// call after the algorithm run the minimax
game::Board Minimax::GetBestMove() const noexcept {
    // find the same heuristic value in children as the root node has
    for(size_t i = 0; i < m_nodes[0].count; i++) {
        auto id { m_nodes[0].children[i] };
        if(fabs(m_nodes[0].heuristic - m_nodes[id].heuristic) <= 0.001f) {
            return m_nodes[id].state;
        }
    }
    std::cerr << "Fail to choose\n";
    assert(m_nodes[m_nodes[0].children.front()].count > 0 && "No children generated");
    return m_nodes[m_nodes[0].children.front()].state;
}

float Minimax::Apply(Node& target, int depth, bool isMaximizingPlayer) {
    this->Expand(target, isMaximizingPlayer);
    if (!depth || this->IsTerminal(target)) {
        auto heuristic = this->GetHeuristic(target, depth);
        return heuristic;
    }
    if (isMaximizingPlayer) {
        float heuristic = -std::numeric_limits<float>::max();
        for(size_t i = 0; i < target.count; i++) {
            assert(target.count <= 9 && "Unexpected child number");
            const auto id = target.children[i];
            assert(id < CAPACITY && "Unexpected ID of the child");
            m_nodes[id].heuristic = this->Apply(m_nodes[id], depth - 1, false);
            heuristic = std::max(heuristic, m_nodes[id].heuristic);
        }
        return heuristic;
    }
    else {
        float heuristic = std::numeric_limits<float>::max();
        for(size_t i = 0; i < target.count; i++) {
            assert(target.count <= 9 && "Unexpected child number");
            const auto id = target.children[i];
            assert(id < CAPACITY && "Unexpected ID of the child");
            m_nodes[id].heuristic = this->Apply(m_nodes[id], depth - 1, true);
            heuristic = std::min(heuristic, m_nodes[id].heuristic);
        }
        return heuristic;
    }
}

bool Minimax::IsTerminal(const Node & node) const noexcept {
    using State = game::Board::State;
    return node.count == 0U || game::GetGameState(node.state) != State::ONGOING; // don't have any children
}

float Minimax::GetHeuristic(const Node & node, int depth) const noexcept {
    (void) depth; // ignore for now
    auto & state = node.state;
    int add { 0 }, substract { 0 };
    /**
     * depth: The earlier we reach the final => the better (not implemented)
     * - winning state gives 25 points
     * - losing state gives -25 points
     * - otherwise number of two-in-sequence-x with empty slot 
     *  minus number of two-in-sequence-o with empty slot 
     */
    auto eval = [symbol = m_symbol](int& add, int&sub, int x, int o) {
        int scores[2] = { 0, 0 };
        // by default we add scores for good X-combination
        if(x == 3) scores[0] += 25;
        else if(o == 3) scores[1] += 25; 
        else if(x == 2 && !o) scores[0] += 2;
        else if(x == 1 && !o) scores[0] += 1;
        else if(o == 2 && !x) scores[1] += 2;
        else if(o == 1 && !x) scores[1] += 1;

        if(symbol == 'o') {
            std::swap(scores[0], scores[1]);
        }
        add += scores[0];
        sub += scores[1];
    };
    // Points from the rows
    for(size_t i = 0; i < game::Board::ROWS; i++) {
        int x { 0 }, o { 0 };
        for(size_t j = 0; j < game::Board::COLS; j++) {
            const auto value = state.at(i,j);
            x += value == 'x';
            o += value == 'o';
        }
        eval(add, substract, x, o);
    }
    // Points from the cols
    for(size_t col = 0; col < game::Board::COLS; col++) {
        int x { 0 }, o { 0 };
        for(size_t row = 0; row < game::Board::ROWS; row++) {
            const auto value = state.at(row, col);
            x += value == 'x';
            o += value == 'o';
        }
        eval(add, substract, x, o);
    }
    // Points from the main diagonal
    int x { 0 }, o { 0 };
    for(size_t i = 0; i < game::Board::ROWS; i++) {
        const auto value = state.at(i, i);
        x += value == 'x';
        o += value == 'o';
    }
    eval(add, substract, x, o);
    // Points from the other diagonal
    x = 0;
    o = 0;
    for(size_t i = 0; i < game::Board::ROWS; i++) {
        const auto value = state.at(i, game::Board::ROWS - i - 1);
        x += value == 'x';
        o += value == 'o';
    }
    eval(add, substract, x, o);    

    return add - substract + 0.f;
}

void Minimax::Expand(Node& target, bool isMaximizing) noexcept {
    const char value = isMaximizing? m_symbol : m_opponent;
    // trying to generate 9 or less children (all possible)
    for(size_t i = 0U; i < target.children.size(); i++) {
        const auto row = i / 3U;
        const auto col = i % 3U;

        assert(m_count <= CAPACITY && "Overflow");
        
        Node child{};
        // copy parent state
        child.state = target.state;
        // if we can make a move
        if(child.state.at(row, col) == '.') {
            // make a move
            child.state.assign(row, col, value);
            // update id of children in the parent node
            target.children[target.count] = m_count;
            // update number of children the parent has
            target.count++;
            // copy child to node pool 
            m_nodes[m_count] = child;
            // and increment the number of used nodes 
            m_count++;
        } 
        // otherwise ignore child
    }
}