#include <windows.h>
#include <string>
#include <unordered_map>
#include <tchar.h>
#include "font.h"
#include "gdi.h"

namespace Font {

	namespace {
		HFONT CreateScaledFont(HDC hdc, const RECT& rect, const std::wstring& str) {
			// ���������С
			int font_size_height = rect.bottom - rect.top;
			int font_size_width = rect.right - rect.left;

			int new_font_size = min(font_size_height, font_size_width);

			return CreateFont(
				new_font_size,                 // �߶ȣ���ֵ��ʾ�ַ��߶ȣ�
				0,                          // ��ȣ�0��ʾĬ�Ͽ�߱ȣ�
				0,                          // ��б�Ƕ�
				0,                          // ����
				FW_BOLD,                    // ��ϸ��FW_BOLD=700��
				FALSE,                      // б��
				FALSE,                      // �»���
				FALSE,                      // ɾ����
				DEFAULT_CHARSET,            // �ַ���
				OUT_DEFAULT_PRECIS,         // �������
				CLIP_DEFAULT_PRECIS,        // �ü�����
				CLEARTYPE_QUALITY,          // �������
				DEFAULT_PITCH | FF_DONTCARE,// ����ϵ��
				FONT_FAMILY                    // ��������
			);
		}
	}

	void DrawScaledText(HDC hdc, const RECT& rect, const std::wstring& str) {
		IGDI::AutoGDI<HFONT> h_font(CreateScaledFont(hdc, rect, str));
		
		HFONT h_old_font = (HFONT)SelectObject(hdc, h_font);

		DrawText(
			hdc,
			str.c_str(),
			-1,
			const_cast<LPRECT>(&rect),
			DT_SINGLELINE
			| DT_CENTER
			| DT_VCENTER);

		SelectObject(hdc, h_old_font);
	}
}


