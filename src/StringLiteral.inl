// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr BasicStringLiteral<T, N>::BasicStringLiteral(const BasicStringLiteral<T, N1> &other) noexcept
    {
        Assign(other);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr BasicStringLiteral<T, N>::BasicStringLiteral(BasicStringLiteral<T, N1> &&other) noexcept
    {
        Assign(std::move(other));
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, N>::SizeType N1>
    constexpr BasicStringLiteral<T, N>::BasicStringLiteral(const T(&arr)[N1]) noexcept
    {
        Assign(arr);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::BasicStringLiteral(const T *const p) noexcept
    {
        Assign(p);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::BasicStringLiteral(const T *const p, const SizeType count) noexcept
    {
        Assign(p, count);
    }

    template <Character T, size_t N>
    template <std::contiguous_iterator It1, std::sized_sentinel_for<It1> It2>
        requires std::same_as<std::iter_value_t<It1>, T> && (!std::convertible_to<It2, typename BasicStringLiteral<T, N>::SizeType>)
    constexpr BasicStringLiteral<T, N>::BasicStringLiteral(const It1 first, const It2 last) noexcept
    {
        Assign(first, last);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::Assign(const BasicStringLiteral<T, N1> &other) noexcept
    {
        return Assign(other.Data);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::Assign(BasicStringLiteral<T, N1> &&other) noexcept
    {
        Assign(other.Data);
        other.Data[0] = '\0';
        return *this;
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, N>::SizeType N1>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::Assign(const T(&arr)[N1]) noexcept
    {
        return Assign(arr + 0);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::Assign(const T *const p) noexcept
    {
        auto i = 0uz;
        while (p[i] && i < N)
        {
            Data[i] = p[i];
            ++i;
        }
        Data[i] = '\0';
        return *this;
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::Assign(const T *const p, const SizeType count) noexcept
    {
        auto len = std::min(N, count);
        Data[len] = '\0';
        while(0 < len)
        {
            --len;
            Data[len] = p[len];
        }
        return *this;
    }

    template <Character T, size_t N>
    template <std::contiguous_iterator It1, std::sized_sentinel_for<It1> It2>
        requires std::same_as<std::iter_value_t<It1>, T> && (!std::convertible_to<It2, typename BasicStringLiteral<T, N>::SizeType>)
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::Assign(const It1 first, const It2 last) noexcept
    {
        return Assign(first, last - first);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr void BasicStringLiteral<T, N>::Swap(BasicStringLiteral<T, N1> &other) noexcept
    {
        std::bitset<2> foundNullTerminators;
        for (auto lhs = Data, rhs = other.Data; !foundNullTerminators.all(); ++lhs, ++rhs)
        {
            if (!foundNullTerminators[0])
            {
                foundNullTerminators[0] = lhs == '\0';
            }
            if (!foundNullTerminators[1])
            {
                foundNullTerminators[1] = rhs == '\0';
            }
            std::swap(*lhs, *rhs);
        }
    }

    template <Character T, size_t N>
    constexpr bool BasicStringLiteral<T, N>::Empty() const noexcept
    {
        return *Data == '\0';
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::SizeType BasicStringLiteral<T, N>::Length() const noexcept
    {
        return GetStringLength(Data);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr int BasicStringLiteral<T, N>::Compare(const BasicStringLiteral<T, N1> &other) const noexcept
    {
        return Compare(other.Data, std::max(N, N1));
    }

    template <Character T, size_t N>
    constexpr int BasicStringLiteral<T, N>::Compare(const T *const p) const noexcept
    {
        return Compare(p, std::max(N, GetStringLength(p)));
    }

    template <Character T, size_t N>
    constexpr int BasicStringLiteral<T, N>::Compare(const T *const p, const SizeType length) const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            return std::memcmp(Data, p, length * sizeof(T));
        }
        return std::char_traits<T>::compare(Data, p, length);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstIterator BasicStringLiteral<T, N>::Begin() const noexcept
    {
        return Begin(Data);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstIterator BasicStringLiteral<T, N>::CBegin() const noexcept
    {
        return CBegin(Data);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstIterator BasicStringLiteral<T, N>::End() const noexcept
    {
        return End(Data + 0); // Pass its address. In a constant-expression context, end-pos can be found at compile-time.
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstIterator BasicStringLiteral<T, N>::CEnd() const noexcept
    {
        return CEnd(Data + 0);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstReverseIterator BasicStringLiteral<T, N>::RBegin() const noexcept
    {
        return RBegin(Data + 0);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstReverseIterator BasicStringLiteral<T, N>::CRBegin() const noexcept
    {
        return CRBegin(Data + 0);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstReverseIterator BasicStringLiteral<T, N>::REnd() const noexcept
    {
        return REnd(Data);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstReverseIterator BasicStringLiteral<T, N>::CREnd() const noexcept
    {
        return CREnd(Data);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::operator=(const BasicStringLiteral<T, N1> &other) noexcept
    {
        return Assign(other);
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, 0>::SizeType N1>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::operator=(BasicStringLiteral<T, N1> &&other) noexcept
    {
        return Assign(std::move(other));
    }

    template <Character T, size_t N>
    template <BasicStringLiteral<T, N>::SizeType N1>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::operator=(const T(&arr)[N1]) noexcept
    {
        return Assign(arr);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N> &BasicStringLiteral<T, N>::operator=(const T *const p) noexcept
    {
        return Assign(p);
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::ConstReference BasicStringLiteral<T, N>::operator[](const SizeType i) const noexcept
    {
        static_assert(i < N);
        return Data[i];
    }

    template <Character T, size_t N>
    constexpr BasicStringLiteral<T, N>::operator const T *() const noexcept
    {
        return Data;
    }

    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    std::ostream &operator <<(std::ostream &os, const BasicStringLiteral<T, N> &str)
    {
        return os << str.Data;
    }

    template <Character T, BasicStringLiteral<T, 0>::SizeType N1, BasicStringLiteral<T, 0>::SizeType N2>
    constexpr bool operator==(const BasicStringLiteral<T, N1> &lhs, const BasicStringLiteral<T, N2> &rhs) noexcept
    {
        return Wcm::operator==(lhs, rhs.Data);
    }

    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    constexpr bool operator==(const BasicStringLiteral<T, N> &lhs, const T *const rhs) noexcept
    {
        return lhs.Compare(rhs, GetStringLength(rhs)) == 0;
    }

    template <Character T, BasicStringLiteral<T, 0>::SizeType N1, BasicStringLiteral<T, 0>::SizeType N2>
    constexpr std::strong_ordering operator<=>(const BasicStringLiteral<T, N1> &lhs, const BasicStringLiteral<T, N2> &rhs) noexcept
    {
        return Wcm::operator<=>(lhs, rhs.Data);
    }

    template <Character T, BasicStringLiteral<T, 0>::SizeType N>
    constexpr std::strong_ordering operator<=>(const BasicStringLiteral<T, N> &lhs, const T *const rhs) noexcept
    {
        const auto res = lhs.Compare(rhs, GetStringLength(rhs));
        return res == 0 ? std::strong_ordering::equal : res < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
    }
}

namespace std
{
    template<Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N1, Wcm::BasicStringLiteral<T, 0>::SizeType N2>
    void swap(Wcm::BasicStringLiteral<T, N1> &lhs, Wcm::BasicStringLiteral<T, N2> &rhs) noexcept(noexcept(lhs.Swap(rhs)))
    {
        lhs.Swap(rhs);
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto empty(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Empty()))
        -> decltype(s.Empty())
    {
        return s.Empty();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto size(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Length()))
        -> decltype(s.Length())
    {
        return s.Length();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto ssize(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Length()))
        -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(s.Length())>>
    {
        return static_cast<common_type_t<ptrdiff_t, std::make_signed_t<decltype(s.Length())>>>(s.Length());
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto begin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.Begin()))
        -> decltype(s.Begin())
    {
        return s.Begin();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto cbegin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CBegin()))
        -> decltype(s.CBegin())
    {
        return s.CBegin();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto end(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.End()))
        -> decltype(s.End())
    {
        return s.End();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto cend(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CEnd()))
        -> decltype(s.CEnd())
    {
        return s.CEnd();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto rbegin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.RBegin()))
        -> decltype(s.RBegin())
    {
        return s.RBegin();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto crbegin(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CRBegin()))
        -> decltype(s.CRBegin())
    {
        return s.CRBegin();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto rend(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.REnd()))
        -> decltype(s.REnd())
    {
        return s.REnd();
    }

    template <Wcm::Character T, Wcm::BasicStringLiteral<T, 0>::SizeType N>
    constexpr auto crend(const Wcm::BasicStringLiteral<T, N> &s) noexcept(noexcept(s.CREnd()))
        -> decltype(s.CREnd())
    {
        return s.CREnd();
    }
}
