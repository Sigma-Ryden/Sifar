#ifndef SERIALIZATION_ACCESS_HPP
#define SERIALIZATION_ACCESS_HPP

#include "Detail/Meta.hpp"

#include "Detail/MacroScope.hpp"

#define SERIALIZATION_ARCHIVE_ACCESS(...)                                                               \
    friend class serialization::Access;

namespace serialization
{

class Access
{
private:
    // Special type for has_function_tpl_helper meta
    struct dummy_type;

    SERIALIZATION_HAS_FUNCTION_TPL_HELPER(serialize);

    SERIALIZATION_HAS_FUNCTION_TPL_HELPER(save);
    SERIALIZATION_HAS_FUNCTION_TPL_HELPER(load);

public:
    template <class T>
    static constexpr bool is_save_load_class() noexcept
    {
        return has_save<T>::value and has_load<T>::value;
    }

    template <class T>
    static constexpr bool is_serialize_class() noexcept
    {
        return has_serialize<T>::value;
    }

public:
    template <class Archive, class T,
              meta::require<is_serialize_class<T>()> = 0>
    static void serialize(Archive& archive, T& object) noexcept
    {
        object.serialize(archive);
    }

    template <class Archive, class T,
              meta::require<is_save_load_class<T>()> = 0>
    static void save(Archive& archive, T& object) noexcept
    {
        object.save(archive);
    }

    template <class Archive, class T,
              meta::require<is_save_load_class<T>()> = 0>
    static void load(Archive& archive, T& object) noexcept
    {
        object.load(archive);
    }
};

} // namespace serialization

#include "Detail/MacroUnscope.hpp"

#endif // SERIALIZATION_ACCESS_HPP
