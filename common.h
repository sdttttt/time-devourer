#pragma once


namespace Debug {
    inline void ReportError(LPCWSTR err)
    {
        MessageBox(NULL, err, L"Error", MB_ICONERROR);
    }

    inline void TryLastErrorMessageBox() {
#if _DEBUG
        DWORD err = GetLastError();
        if (err == 0) return;

        LPWSTR errMsg = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL, err, 0, (LPWSTR) &errMsg, 0, NULL);
        ReportError(errMsg);
        LocalFree(errMsg);
#endif
    }
}
