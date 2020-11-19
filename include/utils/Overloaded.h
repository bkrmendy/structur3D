//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_OVERLOADED_H
#define STRUCTUR3D_BASE_OVERLOADED_H

// https://en.cppreference.com/w/cpp/utility/variant/visit

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif //STRUCTUR3D_BASE_OVERLOADED_H
