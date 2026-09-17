#include "Deque.h"

#include <iterator>
#include <type_traits>

template <typename T, typename Container = Deque<T>> class Queue
{
  public:
    using value_type = typename Container::value_type;
    using reference = typename Container::referene;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;

    static_assert(std::is_same_v<T, value_type>,
                  "container adaptors require consistent types");

  protected:
    friend Container;
    Container c;

  public:
    constexpr Queue() : Queue(Container()) {}
    constexpr explicit Queue(const Container& c) : c(c) {}
    constexpr explicit Queue(Container&& c) : c(std::move(c)) {}
    template <typename InputIter>
    constexpr Queue(InputIter first, InputIter last) : c(first, last)
    {
    }
    template <typename Alloc>
    constexpr explicit Queue(const Alloc& alloc) : c(alloc)
    {
    }
    template <typename Alloc>
    constexpr Queue(const Container& c, const Alloc& alloc)
    {
    }
    template <typename Alloc>
    constexpr Queue(Container&& c, const Alloc& alloc) : c(c, alloc)
    {
    }
    template <typename Alloc>
    constexpr Queue(const Queue& q, const Alloc& alloc) : c(q.c, alloc)
    {
    }
    template <typename Alloc>
    constexpr Queue(Queue&& q, const Alloc& alloc) : c(std::move(q.c), alloc)
    {
    }
    template <typename InputIter, typename Alloc>
    constexpr Queue(InputIter first, InputIter last, const Alloc& alloc)
        : c(first, last, alloc)
    {
    }

    constexpr bool empty() const { return c.empty(); }
    constexpr size_type size() const { return c.size(); }
    constexpr reference front() { return c.front(); }
    constexpr const_reference front() const { return c.front(); }
    constexpr reference back() { return c.back(); }
    constexpr const_reference back() const { return c.back(); }
    constexpr void push(const value_type& x) { c.push_back(x); }
    constexpr void push(value_type&& x) { c.push_back(std::move(x)); }
    template <typename... Args> constexpr decltype(auto) emplace(Args&&... args)
    {
        return c.emplace_back(std::forward<Args>(args)...);
    }
    constexpr void pop() { c.pop_front(); }
    constexpr void swap(Queue& q) noexcept(
        std::is_nothrow_swappable_v<Container>)
    {
        using std::swap;
        swap(c, q.c);
    }

    friend constexpr bool operator==(const Queue& x, const Queue& y)
    {
        return x.c == y.c;
    }
    friend constexpr bool operator<(const Queue& x, const Queue& y)
    {
        return x.c < y.c;
    }
    friend constexpr bool operator!=(const Queue& x, const Queue& y)
    {
        return !(x == y);
    }
    friend constexpr bool operator>(const Queue& x, const Queue& y)
    {
        return y < x;
    }
    friend constexpr bool operator<=(const Queue& x, const Queue& y)
    {
        return !(y < x);
    }
    friend constexpr bool operator>=(const Queue& x, const Queue& y)
    {
        return !(x < y);
    }
};

template <typename T, typename Container, typename Alloc>
struct std::uses_allocator<Queue<T, Container>, Alloc>
    : std::uses_allocator<Container, Alloc>::type
{
};

namespace std
{
template <class T, class Container,
          std::enable_if_t<std::is_swappable_v<Container>, int> = 0>
constexpr void swap(Queue<T, Container>& x,
                    Queue<T, Container>& y) noexcept(noexcept(x.swap(y)));
}

template <typename Container>
Queue(Container) -> Queue<typename Container::value_type, Container>;

template <typename It,
          typename = typename std::iterator_traits<It>::iterator_category>
Queue(It, It) -> Queue<typename std::iterator_traits<It>::value_type>;

template <typename Container, typename Alloc,
          typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
Queue(Container, Alloc) -> Queue<typename Container::value_type, Container>;

template <typename It, typename Alloc,
          typename = typename std::iterator_traits<It>::iterator_category>
Queue(It, It, Alloc)
    -> Queue<typename std::iterator_traits<It>::value_type,
             Deque<typename std::iterator_traits<It>::value_type, Alloc>>;

template <typename T, typename Container>
bool operator==(const Queue<T, Container>& x, const Queue<T, Container>& y)
{
    return x.c == y.c;
}

template <typename T, typename Container>
bool operator!=(const Queue<T, Container>& x, const Queue<T, Container>& y)
{
    return x.c != y.c;
}

template <typename T, typename Container>
bool operator<(const Queue<T, Container>& x, const Queue<T, Container>& y)
{
    return x.c < y.c;
}

template <typename T, typename Container>
bool operator>(const Queue<T, Container>& x, const Queue<T, Container>& y)
{
    return x.c > y.c;
}

template <typename T, typename Container>
bool operator<=(const Queue<T, Container>& x, const Queue<T, Container>& y)
{
    return x.c <= y.c;
}

template <typename T, typename Container>
bool operator>=(const Queue<T, Container>& x, const Queue<T, Container>& y)
{
    return x.c >= y.c;
}