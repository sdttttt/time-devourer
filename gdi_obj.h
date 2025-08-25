#pragma once
#include <windows.h>
#include <type_traits>

// �Զ���װGDI�����ڶ����������ڽ���ʱ�Զ��ͷ�
namespace IGDI {

    template <typename T>
    struct is_gdi_type : std::false_type {};
    template <>
    struct is_gdi_type<HFONT> : std::true_type {};
    template <>
    struct is_gdi_type<HBRUSH> : std::true_type {};
    template <>
    struct is_gdi_type<HPEN> : std::true_type {};

    template <typename T>
    class AutoGDI {

        static_assert( is_gdi_type<T>::value, "T must be a GDI object" );
        T obj;
    public:
        explicit AutoGDI(T obj) : obj(obj) {}
        ~AutoGDI() {
            if ( obj ) {
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