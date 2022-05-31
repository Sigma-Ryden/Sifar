#ifndef SERIALIZATION_DETAIL_META_HPP
#define SERIALIZATION_DETAIL_META_HPP

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#include <Serialization/Detail/MacroScope.hpp>

namespace serialization
{

namespace meta
{

template <bool condition, typename T = void>
using when = typename std::enable_if<condition, T>::type;

template <bool condition>
using require = when<condition, int>;

template <typename... Args>
using void_t = void;

template <typename T>
using remove_ptr = typename std::remove_pointer<T>::type;

template <typename T>
using remove_cv = typename std::remove_cv<T>::type;

template <typename T>
using remove_ref = typename std::remove_reference<T>::type;

namespace detail
{

template <typename T, typename = void_t<>>
struct deref_impl { using type = T; };

template <typename T>
struct deref_impl<T, void_t<decltype(*std::declval<T>())>>
{
    using type = remove_ref<decltype(*std::declval<T>())>;
};

} // namespace detail

template <typename It>
using deref = typename detail::deref_impl<It>::type;

template <bool condition, typename if_true, typename if_false>
using if_ = typename std::conditional<condition, if_true, if_false>::type;

template <class...> struct and_ : std::true_type {};
template <class B1> struct and_<B1> : B1 {};
template <class B1, class... Bn>
struct and_<B1, Bn...>
    : if_<bool(B1::value), and_<Bn...>, B1> {};

namespace detail
{

template <typename, std::size_t N>
struct remove_pointer_impl;

template <typename T>
struct remove_pointer_impl<T*, 1>
{
    using type = T;
};

template <typename T, std::size_t N>
struct remove_pointer_impl<T*, N>
{
    using type = typename remove_pointer_impl<T, N - 1>::type;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using remove_pointer = typename detail::remove_pointer_impl<T, N>::type;

namespace detail
{

template <typename>
struct is_character_impl : std::false_type {};

template <> struct is_character_impl<char> : std::true_type {};
template <> struct is_character_impl<signed char> : std::true_type {};
template <> struct is_character_impl<unsigned char> : std::true_type {};

template <> struct is_character_impl<wchar_t> : std::true_type {};
template <> struct is_character_impl<char16_t> : std::true_type {};
template <> struct is_character_impl<char32_t> : std::true_type {};

template <typename T>
struct is_character : is_character_impl<typename std::remove_cv<T>::type> {};

template <class T, class... Tn>
struct is_same_all: and_<std::is_same<T, Tn>...> {};

} // namespace detail

template <std::size_t...>
struct index_sequence {};

namespace detail
{

template <std::size_t I, std::size_t... In>
struct index_sequence_helper : public index_sequence_helper<I - 1, I - 1, In...> {};

template <std::size_t... In>
struct index_sequence_helper<0, In...>
{
    using type = index_sequence<In...>;
};

} // namespace detail

template <std::size_t N>
using make_index_sequence = typename detail::index_sequence_helper<N>::type;

template <class Base, class Derived> constexpr bool is_base_of() noexcept
{
    return std::is_base_of<Base, Derived>::value;
}

template <typename T> constexpr bool is_abstract() noexcept
{
    return std::is_abstract<T>::value;
}

template <typename T> constexpr bool is_polymorphic() noexcept
{
    return std::is_polymorphic<T>::value;
}

template <typename T, typename... Tn> constexpr bool is_same_all() noexcept
{
    return detail::is_same_all<T, Tn...>::value;
}

template <typename T> constexpr bool is_character() noexcept
{
    return detail::is_character<T>::value;
}

template <typename T> constexpr bool is_arithmetic() noexcept
{
    return std::is_arithmetic<T>::value;
}

template <typename T> constexpr bool is_enum() noexcept
{
    return std::is_enum<T>::value;
}

template <typename T> constexpr bool is_array() noexcept
{
    return std::is_array<T>::value;
}

template <typename T> constexpr bool is_pointer() noexcept
{
    return std::is_pointer<T>::value;
}

template <typename T> constexpr bool is_abstract_pointer() noexcept
{
    return is_pointer<T>() and std::is_abstract<meta::deref<T>>::value;
}

template <typename T> constexpr bool is_polymorphic_pointer() noexcept
{
    return is_pointer<T>() and std::is_polymorphic<meta::deref<T>>::value;
}

template <typename T> constexpr bool is_pod_pointer() noexcept
{
    return is_pointer<T>()
           and not is_abstract_pointer<T>()
           and not is_polymorphic_pointer<T>();
}

} // namespace meta

} // namespace serialization

#include "MacroUnscope.hpp"

#endif // SERIALIZATION_DETAIL_META_HPP