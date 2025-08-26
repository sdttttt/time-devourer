#pragma once

#include <windows.h>

namespace Debug {

    inline void TryLastErrorMessageBox() {
#if _DEBUG
        DWORD err = GetLastError();
        if ( err == 0 ) return;

        LPWSTR errMsg = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, err, 0, ( LPWSTR ) &errMsg, 0, NULL);
        MessageBox(NULL, errMsg, L"Error", MB_ICONERROR);
        LocalFree(errMsg);
#endif
    }
}

