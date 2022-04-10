#pragma once

template<typename T, typename U>
struct _is_same
{
    static const bool value = false;
};

template<typename T>
struct _is_same<T, T>
{
    static const bool value = true;
};

template<typename T, typename U>
constexpr bool is_equal_type() { return _is_same<T, U>::value; }