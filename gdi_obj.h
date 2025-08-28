#pragma once
#include <windows.h>
#include <type_traits>

// 自动包装GDI对象，在对象生命周期结束时自动释放
namespace IGDI {
    template<typename T>
    struct is_gdi_type {
        static constexpr bool value = false;
    };

    template<>
    struct is_gdi_type<HFONT> {
        static constexpr bool value = true;
    };

    template<>
    struct is_gdi_type<HBRUSH> {
        static constexpr bool value = true;
    };

    template<>
    struct is_gdi_type<HPEN> {
        static constexpr bool value = true;
    };

    template<typename T>
    class AutoGDI {
        static_assert(is_gdi_type<T>::value == true, "T must be a GDI object");

        T obj;

    public:
        explicit AutoGDI(T obj) : obj(obj) {
        }

        ~AutoGDI() {
            if (obj) {
                DeleteObject(obj);
                obj = nullptr;
            }
        }

        operator T() const { return obj; }

        T get() const { return obj; }

        T release() {
            T tmp = obj;
            obj = nullptr;
            return tmp;
        }
    };
}
