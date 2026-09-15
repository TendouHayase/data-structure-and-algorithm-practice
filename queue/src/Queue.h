#include "Deque.h"

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
    Container c_;

  public:
    constexpr Queue() : Queue(Container()) {}
    constexpr explicit Queue(const Container& c) {}
    constexpr explicit Queue(Container&& c) {}
    template <typename InputIter>
    constexpr Queue(InputIter first, InputIter last);
    template <typename Alloc> constexpr explicit Queue(const Alloc& alloc);
    template <typename Alloc>
    constexpr Queue(const Container& c, const Alloc& alloc);
    template <typename Alloc>
    constexpr Queue(Container&& c, const Alloc& alloc);
    template <typename Alloc>
    constexpr Queue(const Queue& q, const Alloc& alloc);
    template <typename Alloc> constexpr Queue(Queue&& q, const Alloc& alloc);
    template <typename InputIter, typename Alloc>
    Queue(InputIter first, InputIter last, const Alloc& alloc);

    constexpr bool empty() const { return c_.empty(); }
    constexpr size_type size() const { return c_.size(); }
    constexpr reference front() { return c_.front(); }
    constexpr const_reference front() const { return c_.front(); }
    constexpr reference back() { return c_.back(); }
    constexpr const_reference back() const { return c_.back(); }
    constexpr void push(const value_type& x) { c_.push_back(x); }
    constexpr void push(value_type&& x) { c_.push_back(std::move(x)); }
    template <typename... Args> constexpr decltype(auto) emplace(Args&&... args)
    {
        return c_.emplace_back(std::forward<Args>(args)...);
    }
    constexpr void pop() { c_.pop_front(); }
    constexpr void swap(Queue& q) noexcept(
        std::is_nothrow_swappable_v<Container>)
    {
        using std::swap;
        swap(c_, q.c_);
    }
};

template <typename T, typename Container, typename Alloc>
struct std::uses_allocator<Queue<T, Container>, Alloc>
    : std::uses_allocator<Container, Alloc>::type
{
};
