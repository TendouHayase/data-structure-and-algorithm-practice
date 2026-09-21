#pragma once
#include "other.h"

#include <cassert>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

#if defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#else
#define UNREACHABLE() ((void)0)
#endif

constexpr size_t BUF_SIZE = 512;

constexpr inline size_t deque_buf_size(size_t sz)
{
    return (sz < BUF_SIZE ? size_t(BUF_SIZE / sz) : size_t(1));
}

template <typename _Tp, typename _Ref, typename Ptr> struct DequeIterator
{
  private:
    template <typename CvTp>
    using iter =
        DequeIterator<_Tp, CvTp&,
                      typename std::pointer_traits<Ptr>::template rebind<CvTp>>;

  public:
    using iterator = iter<_Tp>;
    using const_iterator = iter<const _Tp>;
    using Elt_pointer = typename std::pointer_traits<Ptr>::template rebind<_Tp>;
    using Map_pointer =
        typename std::pointer_traits<Ptr>::template rebind<Elt_pointer>;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = _Tp;
    using pointer = Ptr;
    using reference = _Ref;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using Self = DequeIterator;

    Elt_pointer cur;
    Elt_pointer first;
    Elt_pointer last;
    Map_pointer node;

    DequeIterator(Elt_pointer x, Map_pointer y) noexcept
        : cur(x), first(*y), last(*y + deque_buf_size(sizeof(_Tp))), node(y)
    {
    }

    DequeIterator() noexcept : cur(), first(), last(), node() {}

    template <typename Iter,
              typename = _Require<std::is_same<Self, const_iterator>,
                                  std::is_same<Iter, iterator>>>
    DequeIterator(const Iter& x) noexcept
        : cur(x.cur), first(x.first), last(x.last), node(x.node)
    {
    }

    DequeIterator(const DequeIterator& x) noexcept
        : cur(x.cur), first(x.first), last(x.last), node(x.node)
    {
    }

    DequeIterator& operator=(const DequeIterator&) = default;

    [[nodiscard]] reference operator*() const noexcept { return *cur; }
    [[nodiscard]] pointer operator->() const noexcept { return cur; }
    Self& operator++() noexcept
    {
        ++cur;
        if (cur == last)
        {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    Self operator++(int) noexcept
    {
        Self tmp = *this;
        ++*this;
        return tmp;
    }

    Self& operator--() noexcept
    {
        if (cur == first)
        {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    Self& operator--(int) noexcept
    {
        Self tmp = *this;
        --*this;
        return tmp;
    }

    Self& operator+=(difference_type n) noexcept
    {
        const difference_type offset = n + (cur - first);
        if (offset >= 0 &&
            offset < difference_type(deque_buf_size(sizeof(_Tp))))
            cur += n;
        else
        {
            const difference_type node_offset =
                offset > 0
                    ? offset / difference_type(deque_buf_size(sizeof(_Tp)))
                    : -difference_type((-offset - 1) /
                                       deque_buf_size(sizeof(_Tp))) -
                          1;

            set_node(node + node_offset);
            cur = first +
                  (offset -
                   node_offset * difference_type(deque_buf_size(sizeof(_Tp))));
        }

        return *this;
    }

    Self& operator-=(difference_type n) noexcept { return *this += -n; }

    [[nodiscard]] reference operator[](difference_type n) const noexcept
    {
        return *(*this + n);
    }

    void set_node(Map_pointer new_node) noexcept
    {
        node = new_node;
        first = *new_node;
        last = first + difference_type(deque_buf_size(sizeof(_Tp)));
    }

    [[nodiscard]] friend bool operator==(const Self& x, const Self& y) noexcept
    {
        return x.cur == y.cur;
    }

    template <typename RefR, typename PtrR>
    [[nodiscard]] friend bool operator==(
        const Self& x, const DequeIterator<_Tp, RefR, PtrR>& y)
    {
        return x.cur == y.cur;
    }

    [[nodiscard]] friend bool operator!=(const Self& x, const Self& y) noexcept
    {
        return !(x == y);
    }

    template <typename RefR, typename PtrR>
    [[nodiscard]] friend bool operator!=(
        const Self& x, const DequeIterator<_Tp, RefR, PtrR>& y) noexcept
    {
        return !(x == y);
    }

    [[nodiscard]] friend bool operator<(const Self& x, const Self& y) noexcept
    {
        return (x.node == y.node) ? (x.cur < y.cur) : (x.node < y.node);
    }

    template <typename _RefR, typename _PtrR>
    [[nodiscard]] friend bool operator<(
        const Self& x, const DequeIterator<_Tp, _RefR, _PtrR>& y) noexcept
    {
        return (x.node == y.node) ? (x.cur < y.cur) : (x.node < y.node);
    }

    [[nodiscard]] friend bool operator>(const Self& x, const Self& y) noexcept
    {
        return y < x;
    }

    template <typename _RefR, typename _PtrR>
    [[nodiscard]] friend bool operator>(
        const Self& x, const DequeIterator<_Tp, _RefR, _PtrR>& y) noexcept
    {
        return y < x;
    }

    [[nodiscard]] friend difference_type operator-(const Self& x,
                                                   const Self& y) noexcept
    {
        return difference_type(deque_buf_size(sizeof(_Tp))) *
                   (x.node - y.node - difference_type(x.node != nullptr)) +
               (x.cur - x.first) + (y.last - y.cur);
    }

    template <typename _RefR, typename _PtrR>
    [[nodiscard]] friend difference_type operator-(
        const Self& x, const DequeIterator<_Tp, _RefR, _PtrR>& y) noexcept
    {
        return difference_type(deque_buf_size(sizeof(_Tp))) *
                   (x.node - y.node - bool(x.node)) +
               (x.cur - x.first) + (y.last - y.cur);
    }

    [[nodiscard]] friend Self operator+(const Self& x,
                                        difference_type n) noexcept
    {
        Self tmp = x;
        tmp += n;
        return tmp;
    }

    [[nodiscard]] friend Self operator-(const Self& x,
                                        difference_type n) noexcept
    {
        Self tmp = x;
        tmp -= n;
        return tmp;
    }

    [[nodiscard]] friend Self operator+(difference_type n,
                                        const Self& x) noexcept
    {
        return x + n;
    }
};

template <typename _Tp, typename _Alloc> class DequeBase
{

  protected:
    using TpAllocType =
        typename std::allocator_traits<_Alloc>::template rebind_alloc<_Tp>;
    using AllocTraits = std::allocator_traits<TpAllocType>;
    using Ptr = typename AllocTraits::pointer;
    using PtrConst = typename AllocTraits::const_pointer;

    using MapType = typename AllocTraits::template rebind_alloc<Ptr>;
    using MapTraits = std::allocator_traits<MapType>;
    using MapPointer = typename MapTraits::pointer;

    using allocator_type = _Alloc;

    allocator_type get_allocator() const noexcept
    {
        return allocator_type(get_tp_allocator());
    }

    using iterator = DequeIterator<_Tp, _Tp&, Ptr>;
    using const_iterator = DequeIterator<_Tp, const _Tp&, PtrConst>;

    DequeBase() : impl_() { initialize_map(0); }
    DequeBase(size_t num_elem) : impl_() { initialize_map(num_elem); }
    DequeBase(const allocator_type& a, size_t num_elem) : impl_(a)
    {
        initialize_map(num_elem);
    }
    DequeBase(const allocator_type& a) : impl_(a) { /* 호출자가 초기화함.*/ }
    DequeBase(DequeBase&& x) : impl_(std::move(x.get_tp_allocator()))
    {
        initialize_map(0);
        if (x.impl_.map)
            this->impl_.swap_data(x.impl_);
    }

    ~DequeBase() noexcept;

    struct DequeImplData
    {
        MapPointer map;
        size_t map_size;
        iterator start;
        iterator finish;

        DequeImplData() noexcept : map(), map_size(), start(), finish() {}

        DequeImplData(const DequeImplData&) = default;
        DequeImplData& operator=(const DequeImplData&) = default;
        DequeImplData(DequeImplData&& x) noexcept : DequeImplData(x)
        {
            x = DequeImplData();
        }

        void swap_data(DequeImplData& x) noexcept { std::swap(*this, x); }
    };

    struct DequeImpl : public TpAllocType, public DequeImplData
    {
        DequeImpl() noexcept(
            std::is_nothrow_default_constructible_v<TpAllocType>)
            : TpAllocType()
        {
        }

        DequeImpl(const TpAllocType& a) noexcept : TpAllocType(a) {}
        DequeImpl(DequeImpl&&) = default;
        DequeImpl(TpAllocType&& a) noexcept : TpAllocType(std::move(a)) {}
        DequeImpl(DequeImpl&& d, TpAllocType&& a)
            : TpAllocType(std::move(a)), DequeImplData(std::move(d))
        {
        }
    };

    TpAllocType& get_tp_allocator() noexcept { return this->impl_; }
    const TpAllocType& get_tp_allocator() const { return this->impl_; }

    MapType get_map_allocator() const noexcept
    {
        return MapType(get_tp_allocator());
    }

    Ptr allocate_node()
    {

        return std::allocator_traits<TpAllocType>::allocate(
            impl_, deque_buf_size(sizeof(_Tp)));
    }

    void deallocate_node(Ptr p) noexcept
    {
        std::allocator_traits<TpAllocType>::deallocate(
            impl_, p, deque_buf_size(sizeof(_Tp)));
    }

    MapPointer allocate_map(size_t n)
    {
        MapType map_alloc = get_map_allocator();
        return MapTraits::allocate(map_alloc, n);
    }

    void deallocate_map(MapPointer p, size_t n) noexcept
    {
        MapType map_alloc = get_map_allocator();
        MapTraits::deallocate(map_alloc, p, n);
    }

    void initialize_map(size_t);
    void create_nodes(MapPointer nstart, MapPointer nfinish);
    void destroy_nodes(MapPointer nstart, MapPointer nfinish) noexcept;
    enum
    {
        initial_map_size = 8
    };

    DequeImpl impl_;
};

template <typename _Tp, typename _Alloc>
DequeBase<_Tp, _Alloc>::~DequeBase() noexcept
{
    if (this->impl_.map)
    {
        destroy_nodes(this->impl_.start.node, this->impl_.finish.node + 1);
        deallocate_map(this->impl_.map, this->impl_.map_size);
    }
}

template <typename _Tp, typename _Alloc>
void DequeBase<_Tp, _Alloc>::initialize_map(size_t num_elem)
{
    const size_t num_nodes = (num_elem / deque_buf_size(sizeof(_Tp)) + 1);
    this->impl_.map_size =
        std::max((size_t)initial_map_size, size_t(num_nodes + 2));
    this->impl_.map = allocate_map(this->impl_.map_size);

    MapPointer nstart =
        (this->impl_.map + (this->impl_.map_size - num_nodes) / 2);
    MapPointer nfinish = nstart + num_nodes;

    try
    {
        create_nodes(nstart, nfinish);
    }
    catch (...)
    {
        deallocate_map(this->impl_.map, this->impl_.map_size);
        this->impl_.map = MapPointer();
        this->impl_.map_size = 0;
#if __cpp_exceptions // 예외 활성화시
        throw;
#endif
    }

    this->impl_.start.set_node(nstart);
    this->impl_.finish.set_node(nfinish - 1);
    this->impl_.start.cur = impl_.start.first;
    this->impl_.finish.cur =
        (this->impl_.finish.first + num_elem % deque_buf_size(sizeof(_Tp)));
}

template <typename _Tp, typename _Alloc>
void DequeBase<_Tp, _Alloc>::create_nodes(MapPointer nstart, MapPointer nfinish)
{
    MapPointer cur;
    try
    {
        for (cur = nstart; cur < nfinish; ++cur)
            *cur = this->allocate_node();
    }
    catch (...)
    {
        destroy_nodes(nstart, cur);
        throw;
    }
}

template <typename _Tp, typename _Alloc>
void DequeBase<_Tp, _Alloc>::destroy_nodes(MapPointer nstart,
                                           MapPointer nfinish) noexcept
{
    for (MapPointer n = nstart; n < nfinish; ++n)
        deallocate_node(*n);
}

template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
class Deque : protected DequeBase<_Tp, _Alloc>
{
    static_assert(std::is_same<typename std::remove_cv<_Tp>::type, _Tp>::value,
                  "std::deque must have a non-const, non-volatile value_type");

    using Base = DequeBase<_Tp, _Alloc>;
    using TpAllocType = typename Base::TpAllocType;
    using AllocTraits = typename Base::AllocTraits;
    using MapPointer = typename Base::MapPointer;

  public:
    using value_type = _Tp;
    using allocator_type = _Alloc;
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    // using reference = typename AllocTraits::reference; // 표준이 아닌 gcc에서
    // 자체 지원
    using reference = value_type&;
    // using const_reference = typename AllocTraits::const_reference; // 표준이
    // 아닌 gcc에서 자체 지원
    using const_reference = const value_type&;
    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

  private:
    using Base::allocate_map;
    using Base::allocate_node;
    using Base::create_nodes;
    using Base::deallocate_map;
    using Base::deallocate_node;
    using Base::destroy_nodes;
    using Base::get_tp_allocator;
    using Base::initialize_map;

    using Base::impl_;

  public:
    Deque() = default;
    explicit Deque(const allocator_type& a) : Base(a, 0) {}
    explicit Deque(size_t n, const allocator_type& a = allocator_type())
        : Base(a, check_init_len_static(n, a))
    {
        default_initialize();
    }

    Deque(size_t n, const value_type& value,
          const allocator_type& a = allocator_type())
        : Base(a, check_init_len_static(n, a))
    {
        fill_initialize(value);
    }

    Deque(const Deque& x)
        : Base(AllocTraits::select_on_container_copy_construction(
                   x.get_tp_allocator()),
               x.size())
    {
        uninitialized_copy_a(x.begin(), x.end(), this->impl_.start,
                             get_tp_allocator());
    }

    Deque(Deque&&) = default;

    Deque(const Deque& x, const type_identity_t<allocator_type>& a)
        : Base(a, x.size())
    {
        uninitialized_copy_a(x.begin(), x.end(), this->impl_.start,
                             get_tp_allocator());
    }

    Deque(Deque&& x, const type_identity_t<allocator_type>& a)
        : Deque(std::move(x), a, typename AllocTraits::is_always_equal{})
    {
    }

  private:
    Deque(Deque&& x, const allocator_type& a, std::true_type)
        : Base(std::move(x), a)
    {
    }
    Deque(Deque&& x, const allocator_type& a, std::false_type)
        : Base(std::move(x), a, x.size())
    {
        if (x.get_allocator() != a && !x.empty())
        {
            uninitialized_copy_a(x.begin(), x.end(), this->impl_.start,
                                 get_tp_allocator());
            x.clear();
        }
    }

  public:
    Deque(std::initializer_list<value_type> l,
          const allocator_type& a = allocator_type())
        : Base(a)
    {
        range_initialize(l.begin(), l.end(), std::random_access_iterator_tag());
    }

    template <typename InputIterator,
              typename = _RequireInputIter<InputIterator>>
    Deque(InputIterator first, InputIterator last,
          const allocator_type& a = allocator_type())
        : Base(a)
    {
        range_initialize(
            first, last,
            std::iterator_traits<InputIterator>::interator_category());
    }

    ~Deque() { destroy_data(begin(), end(), get_tp_allocator()); }

    Deque& operator=(const Deque& x)
    {
        if (std::addressof(x) != this)
        {

            if constexpr (AllocTraits::propagate_on_container_copy_assignment::
                              value)
            {
                if constexpr (!AllocTraits::is_always_equal::value &&
                              get_tp_allocator() != x.get_tp_allocator())
                {
                    replace_map(x, x.get_allocator());
                    get_tp_allocator() = x.get_tp_allocator();
                    return *this;
                }

                get_tp_allocator() = x.get_tp_allocator();
            }

            const size_type len = size();
            if (len >= x.size())
                erase_at_end(std::copy(x.begin(), x.end(), this->impl_.start));
            else
            {
                const_iterator mid = x.begin() + difference_type(len);
                std::copy(x.begin(), mid, this->impl_.start);
                range_insert_aux(this->impl_.finish, mid, x.end(),
                                 std::random_access_iterator_tag());
            }
        }
        return *this;
    }

    Deque& operator=(Deque&& x) noexcept(
        std::allocator_traits<_Alloc>::is_always_equal::value)
    {
        move_assign1(std::move(x),
                     typename std::allocator_traits<_Alloc>::is_always_equal{});
        return *this;
    }

    Deque& operator=(std::initializer_list<value_type> l)
    {
        assign_aux(l.begin(), l.end(), std::random_access_iterator_tag());
        return *this;
    }

    [[nodiscard]] allocator_type get_allocator() const noexcept
    {
        return Base::get_allocator;
    }

    [[nodiscard]] iterator begin() noexcept { return this->impl_.start; }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return this->impl_.start;
    }

    [[nodiscard]] iterator end() noexcept { return this->impl_.finish; }

    [[nodiscard]] const_iterator end() const noexcept
    {
        return this->impl_.finish;
    }

    [[nodiscard]] reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(this->impl_.finish);
    }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(this->impl_.finish);
    }

    [[nodiscard]] reverse_iterator rend() noexcept
    {
        return reverse_iterator(this->impl_.start);
    }

    [[nodiscard]] const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(this->impl_.start);
    }

    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return this->impl_.start;
    }

    [[nodiscard]] const_iterator cend() const noexcept
    {
        return this->impl_.finish;
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(this->impl_.finish);
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept
    {
        return reverse_iterator(this->impl_.start);
    }

    [[nodiscard]] size_type size() const noexcept
    {
        size_type sz = this->impl_.finish - this->impl_.start;
        if (sz >
            max_size()) // 컴파일러 최적화 힌트. 따라서 컴파일러에 따라 분기
            UNREACHABLE();

        return sz;
    }

    [[nodiscard]] size_type max_size() const noexcept
    {
        return max_size_static(get_tp_allocator());
    }

    [[nodiscard]] size_type capacity() const noexcept
    {
        const auto& s = this->impl_.start;
        const auto& f = this->impl_.finish;
        return size_type(f.node - s.node + 1) * deque_buf_size(sizeof(_Tp));
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return this->impl_.finish == this->impl_.start;
    }

  protected:
    void range_check(size_type n) const
    {
        if (n >= this->size())
            throw std::out_of_range("deque::_M_range_check: __n "
                                    "(which is %zu)>= this->size() "
                                    "(which is %zu)",
                                    n, this->size());
    }

  public:
    reference at(size_type n)
    {
        range_check(n);
        return (*this)[n];
    }

    const_reference at(size_type n) const
    {
        range_check(n);
        return (*this)[n];
    }

    [[nodiscard]] reference front() noexcept
    {
        if (this->empty())
            throw std::out_of_range("cannot dereference from an empty deque");
        return *begin();
    }

    [[nodiscard]] const_reference front() const noexcept
    {
        if (this->empty())
            throw std::out_of_range("cannot dereference from an empty deque");
        return *begin();
    }

    [[nodiscard]] reference back() noexcept
    {
        if (this->empty())
            throw std::out_of_range("cannot dereference from an empty deque");
        iterator tmp = end();
        --tmp;
        return *tmp;
    }

    void push_front(const value_type& x)
    {
        if (this->impl_.start.cur != this->impl_.start.first)
        {
            AllocTraits::construct(this->impl_, this->impl_.start.cur - 1, x);
            --this->impl_.start.cur;
        }
        else
            push_front_aux(x);
    }

    void push_front(value_type&& x) { emplace_front(std::move(x)); }

    void push_back(const value_type& x)
    {
        if (this->impl_.finish.cur != this->impl_.finish.last - 1)
        {
            AllocTraits::construct(this->impl_, this->impl_.finish.cur, x);
            ++this->impl_.finish.cur;
        }
        else
        {
            push_back_aux(x);
        }
    }

    template <typename... Args> reference emplace_front(Args&&... args)
    {
        if (this->impl_.start.cur != this->impl_.start.first)
        {
            AllocTraits::construct(this->impl_, this->impl_.start.cur - 1,
                                   std::forward<Args>(args)...);
            --this->impl_.start.cur;
        }
        else
            push_front_aux(std::forward<Args>(args)...);

        return front();
    }

    void push_back(value_type&& x) { emplace_back(std::move(x)); }

    template <typename... Args> reference emplace_back(Args&&... args)
    {
        if (this->impl_.finish.cur != this->impl_.finish.last - 1)
        {
            AllocTraits::construct(this->impl_, this->impl_.finish.cur,
                                   std::forward<Args>(args)...);
            ++this->impl_.finish.cur;
        }
        else
            push_back_aux(std::forward<Args>(args)...);
        return back();
    }

    void pop_front() noexcept
    {
        assert(!this->empty());
        if (this->impl_.start.cur != this->impl_.start.last - 1)
        {
            AllocTraits::destroy(get_tp_allocator(), this->impl_.start.cur);
            ++this->impl_.start.cur;
        }
        else
            pop_front_aux();
    }

    void pop_back() noexcept
    {
        assert(!this->empty());
        if (this->impl_.finish.cur != this->impl_.finish.first)
        {
            --this->impl_.finish.cur;
            AllocTraits::destroy(get_tp_allocator(), this->impl_.finish.cur);
        }
        else
            pop_back_aux();
    }

    template <typename... Args>
    iterator emplace(const_iterator position, Args&&... args);

    void swap(Deque& x) noexcept
    {
        assert(AllocTraits::propagate_on_container_swap::value ||
               get_tp_allocator() == x.get_tp_allocator());
        impl_.swap_data(x);
        if constexpr (AllocTraits::propagate_on_container_swap::value)
            swap(get_tp_allocator(), x.get_tp_allocator());
    }

    void clear() noexcept { erase_at_end(begin()); }

  protected:
    static size_t check_init_len_static(size_t n, const allocator_type& a)
    {
        if (n > max_size_static(a))
            throw std::length_error(
                "cannot create std::deque larger than max_size()");
        return n;
    }

    static size_type max_size_static(const TpAllocType& a) noexcept
    {
        const size_type diffmax = std::numeric_limits<ptrdiff_t>::max();
        const size_type allocmax = std::allocator_traits<_Alloc>::max_size(a);
        return (std::min)(diffmax, allocmax);
    }

    template <typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last,
                          std::input_iterator_tag)
    {
        this->initialize_map(0);
        try
        {
            for (; first != last; ++first)
                emplace_back(*first);
        }
        catch (...)
        {
            clear();
            throw;
        }
    }

    template <typename ForwardIterator>
    void range_initialize(ForwardIterator first, ForwardIterator last,
                          std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);
        this->initialize_map(check_init_len_static(n, get_tp_allocator()));

        MapPointer cur_node;
        try
        {
            for (cur_node = this->impl_.start.node;
                 cur_node < this->impl_.finish.node; ++cur_node)
            {
                if (n < deque_buf_size(sizeof(_Tp)))
                    UNREACHABLE();
                ForwardIterator mid = first;
                std::advance(mid, deque_buf_size(sizeof(_Tp)));
                uninitialized_copy_a(first, mid, *cur_node, get_tp_allocator());
                first = mid;
            }
            uninitialized_copy_a(first, last, this->impl_.finish.first,
                                 get_tp_allocator());
        }
        catch (...)
        {
            destroy_a(this->impl_.start, iterator(*cur_node, cur_node),
                      get_tp_allocator());
            throw;
        }
    }

    void fill_initialize(const value_type& value)
    {
        MapPointer cur;
        try
        {
            for (cur = this->impl_.start.node; cur < this->impl_.finish.node;
                 ++cur)
                uninitialized_fill_a(*cur, *cur + deque_buf_size(sizeof(_Tp)),
                                     value, get_tp_allocator());
            uninitialized_fill_a(this->impl_.finish.first,
                                 this->impl_.finish.cur, value,
                                 get_tp_allocator());
        }
        catch (...)
        {
            destroy_a(this->impl_.start, iterator(*cur, cur),
                      get_tp_allocator());
            throw;
        }
    }
    void default_initialize()
    {
        MapPointer cur;
        try
        {
            for (cur = this->impl_.start.node; cur < this->impl_.finish.node;
                 ++cur)
                uninitialized_default_a(*cur,
                                        *cur + deque_buf_size(sizeof(_Tp)),
                                        get_tp_allocator());
            uninitialized_default_a(this->impl_.finish.first,
                                    this->impl_.finish.cur, get_tp_allocator());
        }
        catch (...)
        {
            destroy_a(this->impl_.start, iterator(*cur, cur),
                      get_tp_allocator());
            throw;
        }
    }

    template <typename InputIterator>
    void assign_aux(InputIterator first, InputIterator last,
                    std::input_iterator_tag);

    template <typename ForwardIterator>
    void assign_aux(ForwardIterator first, ForwardIterator last,
                    std::forward_iterator_tag)
    {
        const size_type len = std::distance(first, last);
        if (len > size())
        {
            ForwardIterator mid = first;
            std::advance(mid, size());
            std::copy(first, mid, begin());
            range_insert_aux(end(), mid, last, iterator_category(first));
        }
        else
        {
            erase_at_end(std::copy(first, last, begin()));
        }
    }

    void fill_assign(size_type n, const value_type& val)
    {
        if (n > size())
        {
            std::fill(begin(), end(), val);
            fill_insert(end(), n - size(), val);
        }
        else
        {
            erase_at_end(begin() + difference_type(n));
            std::fill(begin(), end(), val);
        }
    }

    template <typename... Args> void push_back_aux(Args&&... args)
    {
        if (size() == max_size())
            throw std::length_error(
                "cannot create std::deque larger than max_size()");

        reserve_map_at_back();
        *(this->impl_.finish.node + 1) = this->allocate_node();
        try
        {
            AllocTraits::construct(this->impl_, this->impl_.finish.cur,
                                   std::forward<Args>(args)...);
            this->impl_.finish.set_node(this->impl_.finish.node + 1);
            this->impl_.finish.cur = this->impl_.finish.first;
        }
        catch (...)
        {
            deallocate_node(*(this->impl_.finish.node + 1));
            throw;
        }
    }

    void pop_front_aux()
    {
        AllocTraits::destroy(get_tp_allocator(), this->impl_.start.cur);
        deallocate_node(this->impl_.start.first);
        this->impl_.start.set_node(this->impl_.start.node + 1);
        this->impl_.start.cur = this->impl_.start.first;
    }

    void pop_back_aux()
    {
        deallocate_node(this->impl_.finish.first);
        this->impl_.finish.set_node(this->impl_.finish.node - 1);
        this->impl_finish.cur = this->impl_.finish.last - 1;
        AllocTraits::destroy(get_tp_allocator(), this->impl_.finish.cur);
    }

    template <typename InputIterator, typename Sentinel>
    void range_prepend(InputIterator first, Sentinel last, size_type n)
    {
        iterator new_start = reserve_elements_at_front(n);
        try
        {
            uninitialized_copy_a(std::move(first), last, new_start,
                                 get_tp_allocator());
            this->impl_.start = new_start;
        }
        catch (...)
        {
            destroy_nodes(new_start.node, this->impl_.start.node);
            throw;
        }
    }

    template <typename InputIterator, typename Sentinel>
    void range_append(InputIterator first, Sentinel last, size_type n)
    {
        iterator new_finish = reserve_elements_at_front(n);
        try
        {
            uninitialized_copy_a(std::move(first), last, this->impl_.finish,
                                 get_tp_allocator());
            this->impl_.finish = new_finish;
        }
        catch (...)
        {
            destroy_nodes(this->impl_.finish.node + 1, new_finish.node + 1);
            throw;
        }
    }

    template <typename InputIterator>
    void range_insert_aux(iterator pos, InputIterator first, InputIterator last,
                          std::input_iterator_tag)
    {
        std::copy(first, last, std::inserter(*this, pos));
        // std::inserter는 컨테이너와 인덱스를 받아 그 인덱스에 삽입(size + 1)
        // 할 수 있는 반복자를 반환
    }
    template <typename ForwardIterator>
    void range_insert_aux(iterator pos, ForwardIterator first,
                          ForwardIterator last, std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);
        if (n == 0) [[unlikely]]
            return;
        if (pos.cur == this->impl_.start.cur)
            range_prepend(first, last, n);
        else if (pos.cur == this->impl_.finish.cur)
            range_append(first, last, n);
        else
            insert_aux(pos, first, last, n);
    }

    // 앞으로 할당할때 비용이 적을지 뒤로 할당할때가 비용이 적을지 판단하여 할당
    template <typename... Args>
    iterator emplace_aux(iterator pos, Args&&... args)
    {
        value_type x_copy(std::forward<Args>(args)...);

        difference_type index = pos - this->impl_.start;
        if (static_cast<size_type>(index) < size() / 2)
        {
            push_front(std::move(front()));
            iterator front1 = this->impl_.start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = this->impl_.start + index;
            iterator pos1 = pos;
            ++pos1;
            std::move(front2, pos1, front1);
        }
        else
        {
            push_back(std::move(back()));
            iterator back1 = this->impl_.finish;
            --back1;
            iterator back2 = back1;
            --back2;
            pos = this->impl_.start + index;
            std::move(pos, back2, back1);
        }
        *pos = std::move(x_copy);
        return pos;
    }

    template <typename ForwardIterator>
    void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last,
                    size_type n)
    {
        const difference_type elem_before = pos - this->impl_.start;
        const size_type length = this->size();

        if (static_cast<size_type>(elem_before) < length / 2)
        {
            iterator new_start = reserve_elements_at_front(n);
            iterator old_start = this->impl_.start + elem_before;
            try
            {
                if (elem_before >= difference_type(n))
                {
                    iterator start_n = (this->impl_.start + difference_type(n));
                    uninitialized_move_a(this->impl_.start, start_n, new_start,
                                         get_tp_allocator());
                    this->impl_.start = new_start;
                    std::move(start_n, pos, old_start);
                    std::copy(first, last, pos - difference_type(n));
                }
                else
                {
                    ForwardIterator mid = first;
                    std::advance(mid, difference_type(n) - elem_before);
                    uninitialized_move_copy(this->impl_.start, pos, first, mid,
                                            new_start, get_tp_allocator());
                    this->impl_.start = new_start;
                    std::copy(mid, last, old_start);
                }
            }
            catch (...)
            {
                destroy_nodes(new_start.node, this->impl_.start.node);
                throw;
            }
        }
        else
        {
            iterator new_finish = reserve_elements_at_back(n);
            iterator old_finish = this->impl_.finish;
            const difference_type elems_after =
                difference_type(length) - elem_before;
            pos = this->impl_.finish - elems_after;
            try
            {
                if (elems_after > difference_type(n))
                {
                    iterator finish_n =
                        (this->impl_.finish - difference_type(n));
                    uninitialized_move_a(finish_n, this->impl_.finish,
                                         this->impl_.finish,
                                         get_tp_allocator());
                    this->impl_.finish = new_finish;
                    std::move(pos, finish_n, old_finish);
                    std::copy(first, last, pos);
                }
                else
                {
                    ForwardIterator mid = first;
                    std::advance(mid, elems_after);
                    uninitialized_copy_move(mid, last, pos, this->impl_.finish,
                                            this->impl_.finish,
                                            get_tp_allocator());
                    this->impl_.finish = new_finish;
                    std::copy(first, mid, pos);
                }
            }
            catch (...)
            {
                destroy_nodes(this->impl_.finish.node + 1, new_finish.node + 1);
                throw;
            }
        }
    }

    void destroy_data_aux(iterator first, iterator last)
    {
        for (MapPointer node = first.node + 1; node < last.node; ++node)
            destroy_a(*node, *node + deque_buf_size(sizeof(_Tp)),
                      get_tp_allocator());

        if (first.node != last.node)
        {
            destroy_a(first.cur, first.last, get_tp_allocator());
            destroy_a(last.first, last.cur, get_tp_allocator());
        }
        else
        {
            destroy_a(first.cur, last.cur, get_tp_allocator());
        }
    }

    template <typename _Alloc1>
    void destroy_data(iterator first, iterator last, const _Alloc1&)
    {
        destroy_data_aux(first, last);
    }

    void destroy_data(iterator first, iterator last, const std::allocator<_Tp>&)
    {
        if (!std::is_trivially_destructible_v<value_type>)
            destroy_data_aux(first, last);
    }

    void erase_at_end(iterator pos)
    {
        destroy_data(pos, end(), get_tp_allocator());
        destroy_nodes(pos.node + 1, this->impl_.finish.node + 1);
        this->impl_.finish = pos;
    }

    iterator reserve_elements_at_front(size_type n)
    {
        const size_type vacancies =
            this->impl_.start.cur - this->impl_.start.first;
        if (n > vacancies)
            new_elements_at_front(n - vacancies);
        return this->impl_.start - difference_type(n);
    }

    iterator reserve_elements_at_back(size_type n)
    {
        const size_type vacancies =
            (this->impl_.finish.last - this->impl_.finish.cur) - 1;
        if (n > vacancies)
            new_elements_at_back(n - vacancies);
        return this->impl_.finish + difference_type(n);
    }

    void new_elements_at_front(size_type new_elems)
    {
        if (this->max_size() - this->size() < new_elems)
            throw std::length_error("deque::new_elements_at_front");
        const size_type new_nodes =
            (new_elems + deque_buf_size(sizeof(_Tp)) - 1) /
            deque_buf_size(sizeof(_Tp));
        reserve_map_at_front(new_nodes);
        size_type i;
        try
        {
            for (i = 1; i <= new_nodes; ++i)
                *(this->impl_.start.node - i) = this->allocate_node();
        }
        catch (...)
        {
            for (size_type j = 1; j < i; ++j)
                deallocate_node(*(this->impl_.start.node - j));
            throw;
        }
    }

    void new_elements_at_back(size_type new_elems)
    {
        if (this->max_size() - this->size() < new_elems)
            throw std::length_error("deque::new_elements_at_back");
        const size_type new_nodes =
            ((new_elems + deque_buf_size(sizeof(_Tp)) - 1) /
             deque_buf_size(sizeof(_Tp)));
        reserve_map_at_back(new_nodes);
        size_type i;
        try
        {
            for (i = 1; i <= new_nodes; ++i)
                *(this->impl_.finish.node - i) = this->allocate_node();
        }
        catch (...)
        {
            for (size_type j = 1; j < i; ++j)
                deallocate_node(*(this->impl_.finish.node - j));
            throw;
        }
    }

    void reserve_map_at_back(size_type nodes_to_add = 1)
    {
        if (nodes_to_add + 1 >
            this->impl_.map_size - (this->impl_.finish.node - this->impl_.map))
            reallocate_map(nodes_to_add, false);
    }

    void reserve_map_at_front(size_type nodes_to_add = 1)
    {
        if (nodes_to_add > size_type(this->impl_.start.node - this->impl_.map))
            reallocate_map(nodes_to_add, true);
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front)
    {
        const size_type old_num_nodes =
            this->impl_.finish.node - this->impl_.start.node + 1;
        const size_type new_num_nodes = old_num_nodes + nodes_to_add;

        MapPointer new_nstart;
        if (this->impl_.map_size > 2 * new_num_nodes)
        {
            new_nstart = this->impl_.map +
                         (this->impl_.map_size - new_num_nodes) / 2 +
                         (add_at_front ? nodes_to_add : 0);
            if (new_nstart < this->impl_.start.node)
                std::copy(this->impl_.start.node, this->impl_.finish.node + 1,
                          new_nstart);
            else
                std::copy_backward(this->impl_.start.node,
                                   this->impl_.finish.node + 1,
                                   new_nstart + old_num_nodes);
        }
        else
        {
            size_type new_map_size =
                this->impl_.map_size +
                std::max(this->impl_.map_size, nodes_to_add) + 2;
            const size_t bufsz = deque_buf_size(sizeof(_Tp));
            if (new_map_size > ((max_size() + bufsz - 1) / bufsz) * 2)
                UNREACHABLE();

            MapPointer _new_map = this->allocate_map(new_map_size);
            new_nstart = _new_map + (new_map_size - new_num_nodes) / 2 +
                         (add_at_front ? nodes_to_add : 0);
            std::copy(this->impl_.start.node, this->impl_.finish.node + 1,
                      new_nstart);
            deallocate_map(this->impl_.map, this->impl_.map_size);

            this->impl_.map = _new_map;
            this->impl_.map_size = new_map_size;
        }

        this->impl_.start.set_node(new_nstart);
        this->impl_.finish.set_node(new_nstart + old_num_nodes - 1);
    }

    void move_assign1(Deque&& x, std::true_type) noexcept
    {
        this->impl_.swap_data(x.impl_);
        if constexpr (AllocTraits::propagate_on_container_move_assignment::
                          value)
            get_tp_allocator() = x.get_tp_allocator();
    }

    void move_assign1(Deque&& x, std::false_type)
    {
        if (get_tp_allocator() == x.get_tp_allocator())
            return move_assign1(std::move(x), std::true_type());
        constexpr bool move_storage =
            AllocTraits::propagate_on_container_move_assignment::value;
        move_assign2(std::move(x), std::bool_constant<move_storage>());
    }

    template <typename... Args> void replace_map(Args&&... args)
    {
        Deque newobj(std::forward<Args>(args)...);
        clear();
        deallocate_node(*begin().node);
        deallocate_map(this->impl_.map, this->impl_.map_size);
        this->impl_.map = nullptr;
        this->impl_.map_size = 0;
        this->impl_.swap_data(newobj.impl_);
    }

    void move_assign2(Deque&& x, std::true_type)
    {
        auto alloc = x.get_tp_allocator();
        replace_map(std::move(x));
        get_tp_allocator() = std::move(alloc);
    }

    void move_assign2(Deque&& x, /* propagate: */ std::false_type)
    {
        if (x.get_tp_allocator() == this->get_tp_allocator())
        {

            replace_map(std::move(x), x.get_allocator());
        }
        else
        {

            _M_assign_aux(std::make_move_iterator(x.begin()),
                          std::make_move_iterator(x.end()),
                          std::random_access_iterator_tag());
            x.clear();
        }
    }
};

template <
    typename _InputIterator,
    typename _ValT = typename std::iterator_traits<_InputIterator>::value_type,
    typename _Allocator = std::allocator<_ValT>,
    typename = _RequireInputIter<_InputIterator>,
    typename = IsAllocator<_Allocator>>
Deque(_InputIterator, _InputIterator, _Allocator = _Allocator())
    -> Deque<_ValT, _Allocator>;

template <typename _Tp, typename _Alloc>
[[nodiscard]] inline bool operator==(const Deque<_Tp, _Alloc>& x,
                                     const Deque<_Tp, _Alloc>& y)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename _Tp, typename _Alloc>
inline void swap(Deque<_Tp, _Alloc>& x,
                 Deque<_Tp, _Alloc>& y) noexcept(noexcept(x.swap(y)))
{
    x.swap(y);
}
