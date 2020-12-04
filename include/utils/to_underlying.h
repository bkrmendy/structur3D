//
// Created by Berci on 2020. 12. 04..
//

#ifndef STRUCTUR3D_BASE_TO_UNDERLYING_H
#define STRUCTUR3D_BASE_TO_UNDERLYING_H

#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

#endif //STRUCTUR3D_BASE_TO_UNDERLYING_H
