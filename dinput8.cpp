#include <windows.h>

typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
DirectInput8Create_t RealDirectInput8Create = nullptr;
HMODULE hRealDll = nullptr;

void LoadRealDinput() {
    if (hRealDll) return;
    
    wchar_t systemPath[MAX_PATH];
    GetSystemDirectoryW(systemPath, MAX_PATH);
    wcscat_s(systemPath, MAX_PATH, L"\\dinput8.dll");
    hRealDll = LoadLibraryW(systemPath);
    
    if (!hRealDll) {
        GetSystemWow64DirectoryW(systemPath, MAX_PATH);
        wcscat_s(systemPath, MAX_PATH, L"\\dinput8.dll");
        hRealDll = LoadLibraryW(systemPath);
    }
    
    if (hRealDll) {
        RealDirectInput8Create = (DirectInput8Create_t)GetProcAddress(hRealDll, "DirectInput8Create");
    }
}

extern "C" __declspec(dllexport) HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) {

    HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"GTASA_Launcher_Mutex");
    
    if (hMutex == NULL) {
        ExitProcess(0);
    }
    CloseHandle(hMutex);

    LoadRealDinput();
    
    if (RealDirectInput8Create) {
        return RealDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }
    return E_FAIL;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
