#include <windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

// 添加到开机启动
bool AddToStartup(const wchar_t* appName, const wchar_t* exePath);
// 从开机启动移除
bool RemoveFromStartup(const wchar_t* appName);
// 检查 appName 是否已在当前用户的开机启动项中
bool IsInStartup(const wchar_t* appName);
