#include "startup.h"

bool AddToStartup(const wchar_t* appName, const wchar_t* exePath) {
    HKEY hKey;
    // 打开当前用户的 Run 注册表项（可写）
    LSTATUS status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );
    if (status != ERROR_SUCCESS) {
        return false;
    }

    // 确保路径带引号（防止路径含空格出错）
    wchar_t quotedPath[MAX_PATH * 2] = {0};
    wcsncpy_s(quotedPath, exePath, _countof(quotedPath) - 1);
    PathQuoteSpacesW(quotedPath); // 自动加引号

    // 写入注册表：appName 作为键名，quotedPath 作为值
    status = RegSetValueExW(
        hKey,
        appName, // 键名
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(quotedPath),
        (wcslen(quotedPath) + 1) * sizeof(wchar_t)
    );

    RegCloseKey(hKey);
    return (status == ERROR_SUCCESS);
}

bool RemoveFromStartup(const wchar_t* appName) {
    HKEY hKey;
    LSTATUS status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );
    if (status != ERROR_SUCCESS) {
        return false;
    }

    status = RegDeleteValueW(hKey, appName);
    RegCloseKey(hKey);
    return (status == ERROR_SUCCESS || status == ERROR_FILE_NOT_FOUND);
}


bool IsInStartup(const wchar_t* appName) {
    HKEY hKey;
    LSTATUS status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_QUERY_VALUE, // 只读权限，更安全
        &hKey
    );
    if (status != ERROR_SUCCESS) {
        return false;
    }

    // 查询值的大小（先不读内容）
    DWORD valueType = 0;
    DWORD valueSize = 0;
    status = RegQueryValueExW(
        hKey,
        appName,              // 要查询的键名（如 L"TimeDevourer"）
        nullptr,
        &valueType,
        nullptr,              // 先传 nullptr 获取大小
        &valueSize
    );

    RegCloseKey(hKey);

    // 如果查询成功且类型是 REG_SZ，则说明存在
    return (status == ERROR_SUCCESS && valueType == REG_SZ);
}
