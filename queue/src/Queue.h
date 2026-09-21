#pragma once
#include "Deque.h"

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

template <typename _Tp, typename Container = Deque<_Tp>> class queue
{
    using allocator_type = typename Container::allocator_type;

  public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;

    static_assert(std::is_same_v<_Tp, value_type>,
                  "container adaptors require consistent types");

    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;

  protected:
    friend Container;
    Container c;
    size_type cap = 10;

  public:
    queue() : c() {}
    explicit queue(const queue& x) : c(x.c) {}
    explicit queue(queue&& x) : c(std::move(x.c)) {}
    explicit queue(size_type capacity) : c(), cap(capacity == 0 ? 10 : capacity)
    {
    }
    explicit queue(value_type value, size_type sz)
        : c(sz, value), cap(sz == 0 ? 10 : sz)
    {
    }
    template <typename InputIter,
              std::enable_if_t<
                  std::is_convertible_v<typename std::iterator_traits<
                                            InputIter>::iterator_category,
                                        std::input_iterator_tag>,
                  int> = 0>
    queue(InputIter first, InputIter last) : c(first, last)
    {
    }
    template <typename Alloc,
              std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    explicit queue(const Alloc& alloc) : c(alloc)
    {
    }
    template <typename Alloc,
              std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    constexpr queue(const Container& c, const Alloc& alloc)
    {
    }
    template <
        typename Alloc,
        std::enable_if_t<std::uses_allocator_v<Container, Alloc>, int> = 0>
    constexpr queue(Container&& c, const Alloc& alloc) : c(c, alloc)
    {
    }
    template <
        typename Alloc,
        std::enable_if_t<std::uses_allocator_v<Container, Alloc>, int> = 0>
    constexpr queue(const queue& q, const Alloc& alloc) : c(q.c, alloc)
    {
    }
    template <
        typename Alloc,
        std::enable_if_t<std::uses_allocator_v<Container, Alloc>, int> = 0>
    constexpr queue(queue&& q, const Alloc& alloc) : c(std::move(q.c), alloc)
    {
    }
    template <typename InputIter, typename Alloc,
              std::enable_if_t<
                  std::conjunction_v<
                      std::uses_allocator<Container, Alloc>,
                      std::is_convertible<typename std::iterator_traits<
                                              InputIter>::iterator_category,
                                          std::input_iterator_tag>>,
                  int> = 0>
    constexpr queue(InputIter first, InputIter last, const Alloc& alloc)
        : c(first, last, alloc)
    {
    }

    queue& operator=(const queue& x)
    {
        c = x.c;
        return *this;
    }
    queue& operator=(queue&& x)
    {
        c = std::move(x.c);
        return *this;
    }

    queue(std::initializer_list<value_type> l)
        : c(l), cap(std::max(l.size(), 10ULL))
    {
    }

    bool empty() const { return c.empty(); }
    bool isEmpty() const { return empty(); }
    size_type size() const { return c.size(); }
    size_type capacity() const { return cap; }

    const_reference peek() const { return front(); }
    reference front() { return c.front(); }
    const_reference front() const { return c.front(); }
    reference back() { return c.back(); }
    const_reference back() const { return c.back(); }
    void push(const value_type& x)
    {
        if (c.size() >= cap)
            cap *= 2;
        c.push_back(x);
    }
    void push(value_type&& x)
    {
        if (c.size() >= cap)
            cap *= 2;
        c.push_back(std::move(x));
    }
    template <typename... Args> decltype(auto) emplace(Args&&... args)
    {
        return c.emplace_back(std::forward<Args>(args)...);
    }

    reference peek()
    {
        if (c.empty())
            throw std::logic_error("queue::peek: queue is empty");
        return front();
    }
    value_type pop()
    {
        if (c.empty())
            throw std::logic_error("queue::pop: queue is empty");
        _Tp tmp = c.front();
        c.pop_front();

        return tmp;
    }
    void clear() { c.clear(); }

    iterator begin() noexcept { return c.begin(); }
    iterator end() noexcept { return c.end(); }
    const_iterator begin() const noexcept { return c.begin(); }
    const_iterator end() const noexcept { return c.end(); }
    const_iterator cbegin() const noexcept { return c.cbegin(); }
    const_iterator cend() const noexcept { return c.cend(); }

    constexpr void swap(queue& q) noexcept(
        std::is_nothrow_swappable_v<Container>)
    {
        using std::swap;
        swap(c, q.c);
    }

    friend constexpr bool operator==(const queue& x, const queue& y)
    {
        return x.c == y.c;
    }
    friend constexpr bool operator<(const queue& x, const queue& y)
    {
        return x.c < y.c;
    }
    friend constexpr bool operator!=(const queue& x, const queue& y)
    {
        return !(x == y);
    }
    friend constexpr bool operator>(const queue& x, const queue& y)
    {
        return y < x;
    }
    friend constexpr bool operator<=(const queue& x, const queue& y)
    {
        return !(y < x);
    }
    friend constexpr bool operator>=(const queue& x, const queue& y)
    {
        return !(x < y);
    }
};

template <typename T, typename Container, typename Alloc>
struct std::uses_allocator<queue<T, Container>, Alloc>
    : std::uses_allocator<Container, Alloc>::type
{
};

namespace std
{
template <class T, class Container,
          std::enable_if_t<std::is_swappable_v<Container>, int> = 0>
constexpr void swap(queue<T, Container>& x,
                    queue<T, Container>& y) noexcept(noexcept(x.swap(y)));
}

template <typename Container>
queue(Container) -> queue<typename Container::value_type, Container>;

template <typename It,
          typename = typename std::iterator_traits<It>::iterator_category>
queue(It, It) -> queue<typename std::iterator_traits<It>::value_type>;

template <typename Container, typename Alloc,
          typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
queue(Container, Alloc) -> queue<typename Container::value_type, Container>;

template <typename It, typename Alloc,
          typename = typename std::iterator_traits<It>::iterator_category>
queue(It, It, Alloc)
    -> queue<typename std::iterator_traits<It>::value_type,
             Deque<typename std::iterator_traits<It>::value_type, Alloc>>;

template <typename T, typename Container>
bool operator==(const queue<T, Container>& x, const queue<T, Container>& y)
{
    return x.c == y.c;
}

template <typename T, typename Container>
bool operator!=(const queue<T, Container>& x, const queue<T, Container>& y)
{
    return x.c != y.c;
}

template <typename T, typename Container>
bool operator<(const queue<T, Container>& x, const queue<T, Container>& y)
{
    return x.c < y.c;
}

template <typename T, typename Container>
bool operator>(const queue<T, Container>& x, const queue<T, Container>& y)
{
    return x.c > y.c;
}

template <typename T, typename Container>
bool operator<=(const queue<T, Container>& x, const queue<T, Container>& y)
{
    return x.c <= y.c;
}

template <typename T, typename Container>
bool operator>=(const queue<T, Container>& x, const queue<T, Container>& y)
{
    return x.c >= y.c;
}

using Queue = queue<int>;