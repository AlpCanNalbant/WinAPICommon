// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    consteval auto GetCount(auto &&arr) noexcept
    {
        if constexpr (Array<decltype(arr)>)
        {
            return CountOf<decltype(arr)>;
        }
        else
        {
            return -1;
        }
    }
}
