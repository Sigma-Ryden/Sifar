#ifndef SERIALIZATION_REGISTRY_HPP
#define SERIALIZATION_REGISTRY_HPP

#include <Serialization/Access.hpp>
#include <Serialization/Detail/Meta.hpp>

#define SERIALIZATION_IMPLEMENT_CLASS_INFO(...)                                                         \
    static constexpr std::size_t static_key() noexcept {                                                \
        return ::serialization::static_hash(#__VA_ARGS__);                                              \
    }                                                                                                   \
    virtual std::size_t dynamic_key() const noexcept {                                                  \
        return static_key();                                                                            \
    }

#define SERIALIZATION_IMPLEMENT_CLASS_TPL_INFO(...)                                                     \
    template<>                                                                                          \
    constexpr std::size_t __VA_ARGS__::static_key() noexcept {                                          \
        return ::serialization::static_hash(#__VA_ARGS__);                                              \
    }                                                                                                   \
    template<>                                                                                          \
    std::size_t __VA_ARGS__::dynamic_key() const noexcept {                                             \
        return static_key();                                                                            \
    }

namespace serialization
{

template <typename Base, class Archive, typename Derived,
    meta::require<meta::is_base_of<Base, Derived>()> = 0>
void base(Archive& archive, Derived& derived)
{
    archive & static_cast<Base&>(derived);
}

template <typename Base, class Archive, typename Derived,
    meta::require<meta::is_virtual_base_of<Base, Derived>()> = 0>
void virtual_base(Archive& archive, Derived& derived)
{
    if (derived.dynamic_key() == Derived::static_key())
        static_cast<Base&>(derived);

#ifdef SERIALIZATION_DEBUG
    else throw "the srializable object must serialize the virtual base object.";
#endif
}

template <class... Tn>
class Registry
{
public:
    template <class T>
    static auto key(T& object) noexcept -> decltype(Access::dynamic_key(object))
    {
        return Access::dynamic_key(object);
    }

    template <class T>
    static constexpr auto key() noexcept -> decltype(Access::static_key<T>())
    {
        return Access::static_key<T>();
    }

    template <class Archive, class P,
              typename T = meta::deref<P>,
              typename key_type = decltype(Access::static_key<T>())>
    static void save(Archive& archive, P& pointer, key_type id)
    {
        return save_impl<T, Tn..., T>(archive, pointer, id);
    }

    template <class Archive, class P,
              typename T = meta::deref<P>,
              typename key_type = decltype(Access::static_key<T>())>
    static void load(Archive& archive, P& pointer, key_type id)
    {
        return load_impl<T, Tn..., T>(archive, pointer, id);
    }

private:
    template <class Derived, class Archive, class Base,
              meta::require<meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& archive, Base& pointer)
    {
        archive & dynamic_cast<Derived&>(*pointer);
    }

    template <class Derived, class Archive, class Base,
              meta::require<not meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& /*archive*/, Base& /*pointer*/) noexcept {}

    template <class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n, class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        if (id == key<Derived>())
            return save_if_derived_of<Derived>(archive, pointer);

        return save_impl<Derived_n...>(archive, pointer, id);
    }

    template <class Derived, class Archive, class Base,
              meta::require<not meta::is_abstract<Derived>() and
                            meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void load_if_derived_of(Archive& archive, Base& pointer)
    {
        if (pointer != nullptr)
            throw "the read pointer must be initialized to nullptr.";

        pointer = new Derived;

        archive & (dynamic_cast<Derived&>(*pointer));
    }

    template <class Derived, class Archive, class Base,
              typename B = meta::deref<Base>,
              typename key_type = decltype(Access::static_key<B>()),
              meta::require<meta::is_abstract<Derived>() or
                            not meta::is_base_of<B, Derived>()> = 0>
    static void load_if_derived_of(Archive& /*archive*/, Base& /*pointer*/) noexcept {}

    template <class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n, class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        if (id == key<Derived>())
            return load_if_derived_of<Derived>(archive, pointer);

        return load_impl<Derived_n...>(archive, pointer, id);
    }
};

} // namespace serialization

#endif // SERIALIZATION_REGISTRY_HPP
