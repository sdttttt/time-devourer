#include "common.h"
#include <VersionHelpers.h>
#include "time_window.h"


int WINAPI WinMain(
    _In_ const HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow
) {
    const auto ret = CreateTimeClassAndWindow(hInstance, nCmdShow);
    if (ret > 0) {
        return ret;
    }

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
};
