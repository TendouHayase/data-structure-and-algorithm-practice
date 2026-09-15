#include <iterator>

template <typename T> class QueueIterator
{
  public:
    using iterator_category = std::forward_iterator_tag;

    using value_type = typename T::value_type;
    using difference_type = typename T::difference_type;
    using pointer = typename T::pointer;
    using reference = value_type&;
    using map_pointer = pointer*;

    static constexpr difference_type buffer_size() noexcept
    {
        return sizeof(T) < 512 ? static_cast<difference_type>(512 / sizeof(T))
                               : 1;
    }

    T* cur = nullptr;
    T* first = nullptr;
    T* last = nullptr;
    map_pointer node = nullptr;

    QueueIterator() = default;
    QueueIterator(pointer p, map_pointer n) noexcept
        : cur(p), first(*n), last(*n + buffer_size()), node(n)
    {
    }
};