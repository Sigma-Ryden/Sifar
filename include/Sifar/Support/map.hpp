#ifndef SIFAR_SUPPORT_MAP_HPP
#define SIFAR_SUPPORT_MAP_HPP

#include <type_traits> // true_type, false_type

#include <map> // map, multimap

#include <utility> // move

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Utility.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_map : std::false_type {};
template <typename Key, typename Type, typename Compare, typename Alloc>
struct is_std_map<std::map<Key, Type, Compare, Alloc>> : std::true_type {};

template <typename> struct is_std_multimap : std::false_type {};
template <typename Key, typename Type, typename Compare, typename Alloc>
struct is_std_multimap<std::multimap<Key, Type, Compare, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(map, meta::is_std_map<T>::value or
                             meta::is_std_multimap<T>::value)
{
    let::u64 size = map.size();
    archive & size;

    for (const auto& item : map)
        archive & item;

    return archive;
}

SERIALIZATION_LOAD_DATA(map, meta::is_std_map<T>::value or
                             meta::is_std_multimap<T>::value)
{
    using value_type = typename T::value_type;

    let::u64 size;
    archive & size;

    map.clear();

    auto hint = map.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = map.emplace_hint(hint, std::move(item));
    }

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_map<T>::value)
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_multimap<T>::value)

#endif // SIFAR_SUPPORT_MAP_HPP
