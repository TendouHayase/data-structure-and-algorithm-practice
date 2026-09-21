#pragma once
#include <iterator>
#include <memory>
#include <type_traits>

template <typename... _Cond>
using _Require = std::enable_if_t<std::conjunction_v<_Cond...>>;

template <class It>
using _RequireInputIter = std::enable_if_t<
    std::is_convertible_v<typename std::iterator_traits<It>::iterator_category,
                          std::input_iterator_tag>>;

template <class T> struct is_std_allocator : std::false_type
{
};
template <class T> struct is_std_allocator<std::allocator<T>> : std::true_type
{
};

template <class T> struct type_identity
{
    using type = T;
};

template <class T> using type_identity_t = typename type_identity<T>::type;

template <class InputIt, class ForwardIt, class Alloc>
ForwardIt uninitialized_copy_a(InputIt first, InputIt last, ForwardIt result,
                               Alloc& alloc)
{
    if constexpr (is_std_allocator<Alloc>::value)
    {
        return std::uninitialized_copy(first, last, result);
    }
    else
    {
        using Traits = std::allocator_traits<Alloc>;
        ForwardIt cur = result;
        try
        {
            for (; first != last; ++first, (void)++cur)
                Traits::construct(alloc, std::addressof(*cur), *first);
            return cur;
        }
        catch (...)
        {
            for (; result != cur; ++result)
                Traits::destroy(alloc, std::addressof(*result));
            throw;
        }
    }
}

template <class InputIt, class ForwardIt, class Alloc>
ForwardIt uninitialized_move_a(InputIt first, InputIt last, ForwardIt result,
                               Alloc& a)
{
    return uninitialized_copy_a(std::make_move_iterator(first),
                                std::make_move_iterator(last), result, a);
}

template <class InputIt1, class InputIt2, class ForwardIt, class Alloc>
ForwardIt uninitialized_move_copy(InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2,
                                  ForwardIt result, Alloc& a)
{
    ForwardIt mid = uninitialized_move_a(first1, last1, result, a);
    try
    {
        return uninitialized_copy_a(first2, last2, mid, a);
    }
    catch (...)
    {
        for (; result != mid; ++result)
            std::allocator_traits<Alloc>::destroy(a, std::addressof(*result));
        throw;
    }
}

template <class InputIt1, class InputIt2, class ForwardIt, class Alloc>
ForwardIt uninitialized_copy_move(InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2,
                                  ForwardIt result, Alloc& a)
{
    ForwardIt mid = uninitialized_copy_a(first1, last1, result, a);
    try
    {
        return uninitialized_move_a(first2, last2, mid, a);
    }
    catch (...)
    {
        for (; result != mid; ++result)
            std::allocator_traits<Alloc>::destroy(a, std::addressof(*result));
        throw;
    }
}

template <class ForwardIt, class Alloc>
void uninitialized_default_a(ForwardIt first, ForwardIt last, Alloc& a)
{
    using Traits = std::allocator_traits<Alloc>;
    ForwardIt cur = first;
    try
    {
        for (; cur != last; ++cur)
            Traits::construct(a, std::addressof(*cur)); // 인자 없음 → T()
    }
    catch (...)
    {
        for (; first != cur; ++first)
            Traits::destroy(a, std::addressof(*first));
        throw;
    }
}

template <class ForwardIt, class T, class Alloc>
void uninitialized_fill_a(ForwardIt first, ForwardIt last, const T& value,
                          Alloc& a)
{
    using Traits = std::allocator_traits<Alloc>;
    ForwardIt cur = first;
    try
    {
        for (; cur != last; ++cur)
            Traits::construct(a, std::addressof(*cur), value); // 복사 생성
    }
    catch (...)
    {
        for (; first != cur; ++first)
            Traits::destroy(a, std::addressof(*first));
        throw;
    }
}

template <class ForwardIt, class Alloc>
void destroy_a(ForwardIt first, ForwardIt last, Alloc& a) noexcept
{
    for (; first != last; ++first)
        std::allocator_traits<Alloc>::destroy(a, std::addressof(*first));
}

template <class It>
constexpr typename std::iterator_traits<It>::iterator_category
iterator_category(const It&)
{
    return typename std::iterator_traits<It>::iterator_category{};
}

template <class A, class = void> struct IsAllocator : std::false_type
{
};

template <class A>
struct IsAllocator<
    A, std::void_t<typename A::value_type,
                   decltype(std::declval<A&>().allocate(std::size_t{}))>>
    : std::true_type
{
};

template <class A>
using require_allocator = std::enable_if_t<IsAllocator<A>::value, A>;