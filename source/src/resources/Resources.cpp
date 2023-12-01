#include "pch.h"
#include "Resources.h"
#include "resource.h"
#include "resources/import/resources.import.h"
#include "utils/convect.string.h"
#include "resources/trackCache.h"
#include "version/Version.h"
#include "resources.load.h"

Resources::Resources()
{
    // load_library cvAutoTrack.resources.dll
    resource_lib_handle = LoadLibraryW(L"cvAutoTrack.resources.dll");
    if (resource_lib_handle == NULL)
        throw "Load cvAutoTrack.resources.dll Fail!";

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

Resources::~Resources()
{
    FreeLibrary(resource_lib_handle);
}

Resources &Resources::getInstance()
{
    static Resources instance;
    return instance;
}

bool Resources::map_is_embedded()
{
    return true;
}

bool load_cache(std::shared_ptr<trackCache::CacheInfo> cacheInfo)
{
    std::string file_name = "cvAutoTrack_Cache.dat";
    // get module path
    wchar_t module_path[MAX_PATH];
    GetModuleFileNameW(NULL, module_path, MAX_PATH);
    std::wstring module_path_wstr(module_path);
    std::string module_path_str = utils::to_string(module_path_wstr);
    std::filesystem::path module_dir = std::filesystem::path(module_path_str).parent_path();
    auto cache_file_path = module_dir / file_name;

    if (std::filesystem::exists(cache_file_path) == false)
    {
        return false;
    }
    cacheInfo = trackCache::Deserialize(cache_file_path.string());

    return true;
}

struct QDTree
{
    std::unique_ptr<QDTree> tl;    // 左上角指针
    std::unique_ptr<QDTree> tr;    // 右上角指针
    std::unique_ptr<QDTree> bl;    // 左下角指针
    std::unique_ptr<QDTree> br;    // 右下角指针
    bool isGeode = false;          // 是否为叶子节点
    std::vector<cv::KeyPoint> pts; // 叶节点中存储的关键点
};

/**
 * @brief 从关键点构造四叉树
 * @param in kp 关键点数组
 * @param out rootNode 构造好的四叉树节点指针
 * @param minSize 分裂后的最小尺寸，为0则不限制
 * @param minPoints 分裂后每个节点至少包含的关键点个数，最小为1
 * @param maxDepth 分裂的最大深度，为0则不限制
 * @return
 */
bool buildQuadTree(const std::vector<cv::KeyPoint> &kp, std::unique_ptr<QDTree> &rootNode, float minSize = 0.0, int minPoints = 1, int depth = 0)
{
    return false;
}