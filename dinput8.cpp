#include <windows.h>

typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
DirectInput8Create_t RealDirectInput8Create = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"Global\\GTASA_Launcher_Mutex");
        
        if (hMutex == NULL) {
            MessageBoxW(NULL, L"Necesitas ejecutar primero el launcher para jugar.", L"Acceso Denegado", MB_ICONERROR | MB_OK);
            ExitProcess(0);
        }
        CloseHandle(hMutex);

        wchar_t systemPath[MAX_PATH];
        GetSystemDirectoryW(systemPath, MAX_PATH);
        wcscat_s(systemPath, MAX_PATH, L"\\dinput8.dll");
        HMODULE hRealDll = LoadLibraryW(systemPath);
        
        if (hRealDll) {
            RealDirectInput8Create = (DirectInput8Create_t)GetProcAddress(hRealDll, "DirectInput8Create");
        }
    }
    return TRUE;
}

extern "C" __declspec(dllexport) HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) {
    if (RealDirectInput8Create) {
        return RealDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }
    return E_FAIL;
}
