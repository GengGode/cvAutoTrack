#include "pch.h"
#include <cvAutoTrack.h>

static HMODULE g_library_module = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/
)
{
    g_library_module = hModule;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}

bool GetCoreModulePath(char* path_buff, int buff_size)
{
    if (g_library_module == NULL)
    {
        return false;
    }
    char module_path_char[MAX_PATH] = { 0 };
    GetModuleFileNameA(g_library_module, module_path_char, MAX_PATH);
    strcpy_s(path_buff, buff_size, module_path_char);
    return true;
}