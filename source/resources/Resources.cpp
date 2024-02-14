#include "pch.h"
#include <cvAutoTrack.h>

#include "Resources.h"
#include "resources.load.h"
#include "resources/import/resources.import.h"
#include "resources/trackCache.h"
#include "utils/convect.string.h"
#include "resource/version.h"

Resources::Resources()
{
    PaimonTemplate = TianLi::Resources::Load::load_image("paimon");
    StarTemplate = TianLi::Resources::Load::load_image("star");
    MinimapCailbTemplate = TianLi::Resources::Load::load_image("cailb");
    UID = TianLi::Resources::Load::load_image("uid_");
    UIDnumber[0] = TianLi::Resources::Load::load_image("uid0");
    UIDnumber[1] = TianLi::Resources::Load::load_image("uid1");
    UIDnumber[2] = TianLi::Resources::Load::load_image("uid2");
    UIDnumber[3] = TianLi::Resources::Load::load_image("uid3");
    UIDnumber[4] = TianLi::Resources::Load::load_image("uid4");
    UIDnumber[5] = TianLi::Resources::Load::load_image("uid5");
    UIDnumber[6] = TianLi::Resources::Load::load_image("uid6");
    UIDnumber[7] = TianLi::Resources::Load::load_image("uid7");
    UIDnumber[8] = TianLi::Resources::Load::load_image("uid8");
    UIDnumber[9] = TianLi::Resources::Load::load_image("uid9");

    cv::cvtColor(StarTemplate, StarTemplate, cv::COLOR_RGB2GRAY);
    cv::cvtColor(UID, UID, cv::COLOR_RGB2GRAY);
    for (int i = 0; i < 10; i++)
    {
        cv::cvtColor(UIDnumber[i], UIDnumber[i], cv::COLOR_RGB2GRAY);
    }
}

Resources& Resources::getInstance()
{
    static Resources instance;
    return instance;
}

bool select_exists_file(const std::vector<std::filesystem::path>& files, std::filesystem::path& result)
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
}

bool load_cache(std::shared_ptr<trackCache::CacheInfo>& cacheInfo)
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

    std::vector<std::filesystem::path> files = { applicate_dir / file_name, module_dir / file_name, module_dir / "resource" / file_name };
    std::filesystem::path cache_file_path;
    if (select_exists_file(files, cache_file_path) == false)
    {
        return false;
    }

    try
    {
        cacheInfo = trackCache::Deserialize(cache_file_path.string());
    }
    catch (...)
    {
        return false;
    }
    return true;
}
