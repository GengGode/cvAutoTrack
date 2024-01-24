#pragma once
#include "global/global.genshin.h"

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

    bool GetUID(int& uid);
    bool GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid);

    bool startServe();
    bool stopServe();

    bool SetDisableFileLog();
    bool SetEnableFileLog();

    bool DebugCapture();
    bool DebugCapturePath(const char* path_buff, int buff_size);
#pragma endregion
private:
    tianli::global::GenshinHandle genshin_handle;
    tianli::global::GenshinScreen genshin_screen;
    tianli::global::GenshinPaimon genshin_paimon;
    tianli::global::GenshinMinimap genshin_minimap;
    tianli::global::GenshinAvatarPosition genshin_avatar_position;

public:
    std::shared_ptr<tianli::frame::frame_source>& get_source() { return genshin_handle.config.frame_source; }

private:
    bool try_get_genshin_windows();
    bool getGengshinImpactWnd();
    bool getGengshinImpactScreen();
    bool getMiniMapRefMat();
};
