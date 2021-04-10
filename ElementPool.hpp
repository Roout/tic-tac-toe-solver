#ifndef ELEMENT_POOL_HPP_
#define ELEMENT_POOL_HPP_

#include <type_traits>
#include <cassert>
#include <vector>

template<class T, 
    class = std::enable_if_t<std::is_default_constructible_v<T>>
>
class ElementPool final {
public:
    static constexpr size_t CAPACITY { 1u << 20u };

    size_t Size() const noexcept {
        return m_size;
    }

    size_t Capacity() const noexcept {
        return CAPACITY;
    }

    void Reset() noexcept {
        m_size = 0;
    }

    bool IsFull() const noexcept {
        return m_size >= CAPACITY;
    }

    T* Acquire() noexcept {
        assert(m_size < CAPACITY && "Out of allocated nodes!");
        m_size++;
        return &m_elements[m_size - 1];
    }

private:
    std::vector<T> m_elements { CAPACITY };
    size_t m_size { 0u };
};

#endif // ELEMENT_POOL_HPP_