#pragma once
#include <array>

#include "CommonTypes.h"

namespace WEngine
{
    /**
     * This has a bunch of items, where the first item is not always the first in the container
     * When getting an item, the belt position gets taken into account.
     * @tparam T Type of the Values.
     * @tparam count Number of items on the belt.
     */
    template<typename T, uint64 count>
    class ValueBelt
    {
    public:
        ValueBelt() : beltPosition(0) {}

        void Cycle()
        {
            beltPosition++;
            if (beltPosition >= count)
                beltPosition = 0;
        }

        T operator[](uint64 index) const
        {
            uint64 pos = beltPosition + index;
            if (pos >= count)
                pos -= count;
            return belt[pos];
        }

        T& operator[](uint64 index)
        {
            uint64 pos = beltPosition + index;
            if (pos >= count)
                pos -= count;
            return belt[pos];
        }

    private:
        uint64 beltPosition;
        std::array<T, count> belt;
    };
}
