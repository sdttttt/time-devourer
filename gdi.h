#pragma once
#include <windows.h>
#include <type_traits>

// �Զ���װGDI�����ڶ����������ڽ���ʱ�Զ��ͷ�
namespace IGDI {

    template <typename GDIObj>
    struct is_gdi_type : std::false_type {};
    template <>
    struct is_gdi_type<HFONT> : std::true_type {};

    template <typename GDIObj>
    class AutoGDI {

        static_assert( is_gdi_type<GDIObj>::value, "T must be a GDI object" );
        GDIObj obj;
    public:
        explicit AutoGDI(GDIObj obj) : obj(obj) {}
        ~AutoGDI() {
            if ( obj ) {
                DeleteObject(obj);
                obj = nullptr;
            }
        }

        operator GDIObj() const { return obj; }

        GDIObj get() const { return obj; }

        GDIObj release() {
            GDIObj tmp = obj;
            obj = nullptr;
            return tmp;
        }
    };

}