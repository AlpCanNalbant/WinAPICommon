// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <bitset>
#include <iterator>
#include "Iterator.hpp"

namespace Wcm
{
    template <Character T, size_t N>
    struct BasicStringLiteral final
    {
        T Data[N]{};
        using Character = T;
        typedef BasicStringLiteral<T, N>           Type,                 type;
        typedef decltype(Data)                     DataType,             data_type;
        typedef T *                                Pointer,              pointer;
        typedef const T *                          ConstPointer,         const_pointer;
        typedef T &                                Reference,            reference;
        typedef const T &                          ConstReference,       const_reference;
        typedef ConstPointer                       ConstIterator,        const_iterator;
        typedef ConstIterator                      Iterator,             iterator;
        typedef ReverseIterator<ConstPointer>      ConstReverseIterator, const_reverse_iterator;
        typedef ConstReverseIterator               ReverseIterator,      reverse_iterator;
        typedef decltype(N)                        SizeType,             size_type;
        typedef ptrdiff_t                          DifferenceType,       difference_type;
        inline static constexpr SizeType MaxLength = std::conditional_t<N != 0, std::integral_constant<size_t, N - 1>, std::integral_constant<size_t, 0>>::value;

        constexpr BasicStringLiteral() noexcept = default;
        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr BasicStringLiteral(const BasicStringLiteral<T, N1> &other) noexcept;
        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr BasicStringLiteral(BasicStringLiteral<T, N1> &&other) noexcept;
        template <SizeType N1>
        constexpr BasicStringLiteral(const T(&arr)[N1]) noexcept;
        constexpr BasicStringLiteral(const T *const p) noexcept;
        constexpr BasicStringLiteral(std::nullptr_t) = delete;
        constexpr BasicStringLiteral(const T *const p, const SizeType count) noexcept;
        template <std::contiguous_iterator It1, std::sized_sentinel_for<It1> It2>
            requires std::same_as<std::iter_value_t<It1>, T> && (!std::convertible_to<It2, SizeType>)
        explicit constexpr BasicStringLiteral(const It1 first, const It2 last) noexcept;
        constexpr ~BasicStringLiteral() = default;

        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr BasicStringLiteral &Assign(const BasicStringLiteral<T, N1> &other) noexcept;
        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr BasicStringLiteral &Assign(BasicStringLiteral<T, N1> &&other) noexcept;
        template <SizeType N1>
        constexpr BasicStringLiteral &Assign(const T(&arr)[N1]) noexcept;
        constexpr BasicStringLiteral &Assign(const T *const p) noexcept;
        constexpr BasicStringLiteral &Assign(std::nullptr_t) = delete;
        constexpr BasicStringLiteral &Assign(const T *const p, const SizeType count) noexcept;
        template <std::contiguous_iterator It1, std::sized_sentinel_for<It1> It2>
            requires std::same_as<std::iter_value_t<It1>, T> && (!std::convertible_to<It2, SizeType>)
        constexpr BasicStringLiteral &Assign(const It1 first, const It2 last) noexcept;
        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr void Swap(BasicStringLiteral<T, N1> &other) noexcept;
        constexpr bool Empty() const noexcept;
        constexpr SizeType Length() const noexcept;
        template <BasicStringLiteral<T, 0>::SizeType N1>
        [[nodiscard]] constexpr int Compare(const BasicStringLiteral<T, N1> &other) const noexcept;
        [[nodiscard]] constexpr int Compare(const T *const p) const noexcept;
        [[nodiscard]] constexpr int Compare(const T *const p, const SizeType length) const noexcept;
        [[nodiscard]] constexpr ConstIterator Begin() const noexcept;
        [[nodiscard]] constexpr ConstIterator CBegin() const noexcept;
        [[nodiscard]] constexpr ConstIterator End() const noexcept;
        [[nodiscard]] constexpr ConstIterator CEnd() const noexcept;
        [[nodiscard]] constexpr ConstReverseIterator RBegin() const noexcept;
        [[nodiscard]] constexpr ConstReverseIterator CRBegin() const noexcept;
        [[nodiscard]] constexpr ConstReverseIterator REnd() const noexcept;
        [[nodiscard]] constexpr ConstReverseIterator CREnd() const noexcept;

        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr BasicStringLiteral &operator=(const BasicStringLiteral<T, N1> &other) noexcept;
        template <BasicStringLiteral<T, 0>::SizeType N1>
        constexpr BasicStringLiteral &operator=(BasicStringLiteral<T, N1> &&other) noexcept;
        template <SizeType N1>
        constexpr BasicStringLiteral &operator=(const T(&arr)[N1]) noexcept;
        constexpr BasicStringLiteral &operator=(const T *const p) noexcept;
        constexpr BasicStringLiteral &operator=(std::nullptr_t) = delete;
        [[nodiscard]] constexpr ConstReference operator[](const SizeType i) const noexcept;
        constexpr operator const T *() const noexcept;
    };

    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    BasicStringLiteral(const T(&)[N]) -> BasicStringLiteral<T, N>;

    template <BasicStringLiteral<char, 0>::SizeType N>
    using StringLiteral = BasicStringLiteral<char, N>;
    template <BasicStringLiteral<wchar_t, 0>::SizeType N>
    using WStringLiteral = BasicStringLiteral<wchar_t, N>;

    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    std::ostream &operator <<(std::ostream &os, const BasicStringLiteral<T, N> &str);

    template <Character T, BasicStringLiteral<T, 0>::SizeType N1, BasicStringLiteral<T, 0>::SizeType N2>
    [[nodiscard]] constexpr bool operator==(const BasicStringLiteral<T, N1> &lhs, const BasicStringLiteral<T, N2> &rhs) noexcept;
    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr bool operator==(const BasicStringLiteral<T, N> &lhs, const T *const rhs) noexcept;

    template <Character T, BasicStringLiteral<T, 0>::SizeType N1, BasicStringLiteral<T, 0>::SizeType N2>
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const BasicStringLiteral<T, N1> &lhs, const BasicStringLiteral<T, N2> &rhs) noexcept;
    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const BasicStringLiteral<T, N> &lhs, const T *const rhs) noexcept;
}

namespace std
{
    template<Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N1, Wcm::BasicStringLiteral<T, 0>::SizeType N2>
    void swap(Wcm::BasicStringLiteral<T, N1> &lhs, Wcm::BasicStringLiteral<T, N2> &rhs) noexcept(noexcept(lhs.Swap(rhs)));

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto empty(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Empty()))
        -> decltype(s.Empty());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto size(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Length()))
        -> decltype(s.Length());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto ssize(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Length()))
        -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(s.Length())>>;

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto begin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Begin()))
        -> decltype(s.Begin());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto cbegin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CBegin()))
        -> decltype(s.CBegin());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto end(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.End()))
        -> decltype(s.End());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto cend(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CEnd()))
        -> decltype(s.CEnd());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto rbegin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.RBegin()))
        -> decltype(s.RBegin());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto crbegin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CRBegin()))
        -> decltype(s.CRBegin());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto rend(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.REnd()))
        -> decltype(s.REnd());
    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    [[nodiscard]] constexpr auto crend(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CREnd()))
        -> decltype(s.CREnd());
}

#include "StringLiteral.inl"
