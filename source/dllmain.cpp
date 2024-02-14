#include "pch.h"
#include <cvAutoTrack.h>

static HMODULE g_library_module = NULL;
static std::string core_cache_path = "";

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

bool SetCoreCachePath(const char* path_buff)
{
    core_cache_path = path_buff;
    return true;
}

#include "utils/convect.string.h"
bool GetCoreCachePath(char* path_buff, int buff_size)
{
    std::string file_name = "cvAutoTrack_Cache.dat";
    // get module path
    wchar_t applicate_path[MAX_PATH];
    GetModuleFileNameW(NULL, applicate_path, MAX_PATH);
    std::string applicate_path_str = utils::to_string(applicate_path);
    std::filesystem::path applicate_dir = std::filesystem::path(applicate_path_str).parent_path();
    char module_path[MAX_PATH];
    GetCoreModulePath(module_path, MAX_PATH);
    std::string module_path_str = module_path;
    std::filesystem::path module_dir = std::filesystem::path(module_path_str).parent_path();

    std::vector<std::filesystem::path> files = { core_cache_path, applicate_dir / file_name, module_dir / file_name, module_dir / "resource" / file_name };
    std::filesystem::path cache_file_path;

    static auto select_exists_file = [](const std::vector<std::filesystem::path>& files, std::filesystem::path& result) -> bool
    {
        for (auto& file : files)
        {
            if (std::filesystem::exists(file))
            {
                result = file;
                return true;
            }
        }
        return false;
    };

    if (select_exists_file(files, cache_file_path) == false)
    {
        return false;
    }

    strcpy_s(path_buff, buff_size, cache_file_path.string().c_str());
    return true;
}