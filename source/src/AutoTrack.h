#pragma once
#include "match/type/MatchType.h"

class AutoTrack
{
public:
    AutoTrack();
#pragma region 外部接口
    bool init();
    bool uninit();

    bool SetUseBitbltCaptureMode();
    bool SetUseDx11CaptureMode();

    bool ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height);
    bool ImportMapBlockCenter(int x, int y);
    bool ImportMapBlockCenterScale(int x, int y, double scale);

    bool SetHandle(long long int handle = 0);
    bool SetWorldCenter(double x, double y);
    bool SetWorldScale(double scale);
    bool GetTransformOfMap(double& x, double& y, double& a, int& mapId);
    bool GetPosition(double& x, double& y);
    bool GetPositionOfMap(double& x, double& y, int& mapId);
    bool GetDirection(double& a);
    bool GetRotation(double& a);
    // 获取发现的神瞳坐标,isEnd为真则为当前画面中的最后一个神瞳
    bool GetStar(double& x, double& y, bool& isEnd);
    // 获取发现的神瞳坐标，以json字符串格式
    bool GetStarJson(char* jsonBuff);
    bool GetUID(int& uid);
    bool GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid);
    /*********/
    bool GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a);
    bool GetInfoLoadVideo(char* path, char* pathOutFile);
    /*********/
    int GetLastError();
    int GetLastErrMsg(char* msg_buff, int buff_size);
    int GetLastErrJson(char* json_buff, int buff_size);

    bool startServe();
    bool stopServe();

    bool SetDisableFileLog();
    bool SetEnableFileLog();

    bool GetVersion(char* version_buff, int buff_size);
    bool GetCompileTime(char* time_buff, int buff_size);
    bool GetMapIsEmbedded();

    bool DebugCapture();
    bool DebugCapturePath(const char* path_buff, int buff_size);
#pragma endregion
private:
    GenshinHandle genshin_handle;
    GenshinScreen genshin_screen;
    GenshinPaimon genshin_paimon;
    GenshinMinimap genshin_minimap;
    GenshinAvatarPosition genshin_avatar_position;

private:
    bool try_get_genshin_windows();
    bool getGengshinImpactWnd();
    bool getGengshinImpactScreen();
    bool getMiniMapRefMat();

#ifdef _DEBUG
    void showMatchResult(float x, float y, int mapId, float angle, float rotate);
#endif // DEBUG
};
