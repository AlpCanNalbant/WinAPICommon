// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Range T>
    constexpr Iterator<T> BeginT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::begin(std::forward<T>(t));
        }
        else
        {
            return t;
        }
    }
    template <Range T>
    constexpr bool BeginT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::begin(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr ConstIterator<T> CBeginT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::cbegin(std::forward<T>(t));
        }
        else
        {
            return t;
        }
    }
    template <Range T>
    constexpr bool CBeginT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::cbegin(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr Sentinel<T> EndT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::end(std::forward<T>(t));
        }
        else
        {
            return t + GetStringLength(t);
        }
    }
    template <Range T>
    constexpr bool EndT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::end(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr ConstSentinel<T> CEndT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::cend(std::forward<T>(t));
        }
        else
        {
            return t + GetStringLength(t);
        }
    }
    template <Range T>
    constexpr bool CEndT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::cend(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr ReverseIterator<Iterator<T>> RBeginT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::rbegin(std::forward<T>(t));
        }
        else
        {
            return std::make_reverse_iterator(t + GetStringLength(t));
        }
    }
    template <Range T>
    constexpr bool RBeginT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::rbegin(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr ReverseIterator<ConstIterator<T>> CRBeginT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::crbegin(std::forward<T>(t));
        }
        else
        {
            return std::make_reverse_iterator(t + GetStringLength(t));
        }
    }
    template <Range T>
    constexpr bool CRBeginT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::crbegin(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr ReverseIterator<Sentinel<T>> REndT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::rend(std::forward<T>(t));
        }
        else
        {
            return std::make_reverse_iterator(t);
        }
    }
    template <Range T>
    constexpr bool REndT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::rend(std::declval<T&>()));
        }
    }

    template <Range T>
    constexpr ReverseIterator<ConstSentinel<T>> CREndT::operator()(T &&t) const noexcept(IsNoexcept<T>())
    {
        if constexpr (!CharacterPointer<T>)
        {
            return std::ranges::crend(std::forward<T>(t));
        }
        else
        {
            return std::make_reverse_iterator(t);
        }
    }
    template <Range T>
    constexpr bool CREndT::IsNoexcept() noexcept
    {
        if constexpr (CharacterPointer<T>)
        {
            return true;
        }
        else
        {
            return noexcept(std::ranges::crend(std::declval<T&>()));
        }
    }
}
