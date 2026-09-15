#include "QueueIterator.h"

#include <memory>


template <typename T, typename Alloc, size_t N> struct Block
{
    Block* front = nullptr;
    Block* back = nullptr;
    T elem[N];

    Block() = delete;

    Block(const Block&) = delete;
    Block(Block&&) noexcept = delete;
    Block& operator=(const Block&) = delete;
    Block& operator=(Block&&) = delete;

    explicit Block(Block* front, Block* back) : front(front), back(back)
    {
        void* ptr = ::operator new(sizeof(T) * N, alignof(T));
    }

    void change_back(Block* next) noexcept { back = next; }
    void change_front(Block* before) noexcept { front = before; }
};

// msvc 기준 vector 헤더에 class vector 내부를 보면서 F12(Go to Definition)을
// 계속 하다보면 메모리의 바이트 단위 할당부터 vector 구성까지 전부 볼 수 있음.
// 아래 구현은 msvc의 구현을 보고 참고해서 구현.
// 참고로 bool 자료형은 템플릿 특수화로 따로 구현되어있음.
// 실제 STL의 스택은 deque<T, Alloc = std::allocator<T>>를 사용하지만 편의를
// 위해 vector를 참고해 구현.
template <typename T, typename Alloc = std::allocator<T>> class Deque
{
    T* start_ = nullptr;
    T* end_ = nullptr;
    size_t sz_ = 0;
    size_t capacity_ = 0;
    static const size_t BLOCK_SIZE = sizeof(T) < 128 ? 512 / sizeof(T) : 4;

    using Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<
        T>; // allocator_traits은 Alloc이 기본 할당자가 있는지 검사하고
    using Alty_traits = std::allocator_traits<Alty>;

  public:
    using value_type = T;
    using allocator_type = Alloc;
    using pointer = typename Alty_traits::pointer;
    using const_pointer = typename Alty_traits::const_pointer;
    using reference = T&;
    using const_reference = const T&;
    using size_type = typename Alty_traits::size_type; // size_t
    using difference_type =
        typename Alty_traits::difference_type; // std::ptrdiff_t

    using iterator = QueueIterator<T>;
    using const_iterator = QueueIterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  private:
    using map_pointer = pointer*;

    static constexpr difference_type buf = iterator::buffer_size();
    static constexpr size_type initial_map_size = 8;

  public:
    Deque() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value)
        : Deque(Alloc())
    {
    }
    explicit Deque(const allocator_type& a) {}

  private:
    allocator_type get_allocator() const noexcept;
};
