#include "pch.h"
#include "AutoTrack.h"

#include "resources/Resources.h"

#include "capture/dxgi/Dxgi.h"
#include "capture/bitblt/Bitblt.h"
#include "filter/kalman/Kalman.h"
#include "utils/Utils.h"

#include "algorithms/algorithms.direction.h"
#include "algorithms/algorithms.rotation.h"

#include "match/match.star.h"
#include "match/match.uid.h"

#include "genshin/genshin.h"

#include "version/Version.h"

Resources& res = Resources::getInstance();
using TianLi::Utils::logger;

AutoTrack::AutoTrack()
{
  genshin_avatar_position.target_map_world_center = res.map_relative_center;
  genshin_avatar_position.target_map_world_scale = res.map_relative_scale;

  genshin_handle.config.capture = std::make_shared<Bitblt>();
  genshin_handle.config.capture->init();
  genshin_avatar_position.config.pos_filter = std::make_shared<Kalman>();

  logger.info("自动追踪加载成功");
}

bool AutoTrack::init()
{
  if (!genshin_minimap.is_init_finish)
  {
    genshin_minimap.is_run_init_start = true;
    TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
    genshin_minimap.is_run_init_start = false;

    genshin_minimap.is_init_finish = true;
  }
  return genshin_minimap.is_init_finish;
}

bool AutoTrack::uninit()
{
  if (genshin_minimap.is_init_finish)
  {
    genshin_minimap.is_run_uninit_start = true;
    TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
    genshin_minimap.is_run_uninit_start = false;

    genshin_minimap.is_init_finish = false;
  }
  return !genshin_minimap.is_init_finish;
}

bool AutoTrack::SetUseBitbltCaptureMode()
{
  std::string logInfo = "自动追踪工作于Bitblt模式";
  if (genshin_handle.config.capture == nullptr)
  {
    genshin_handle.config.capture = std::make_shared<Bitblt>();
    logger.info(logInfo);
    return true;
  }
  if (genshin_handle.config.capture->mode == Capture::Bitblt)
  {
    return true;
    logger.info(logInfo);
  }
  genshin_handle.config.capture.reset();
  genshin_handle.config.capture = std::make_shared<Bitblt>();
  return true;
  logger.info(logInfo);
}

bool AutoTrack::SetUseDx11CaptureMode()
{
  std::string logInfo = "自动追踪工作于DirectX模式";
  if (genshin_handle.config.capture == nullptr)
  {
    genshin_handle.config.capture = std::make_shared<Dxgi>();
    logger.info(logInfo);
    return true;
  }
  if (genshin_handle.config.capture->mode == Capture::DirectX)
  {
    logger.info(logInfo);
    return true;
  }
  genshin_handle.config.capture.reset();
  genshin_handle.config.capture = std::make_shared<Dxgi>();
  logger.info(logInfo);
  return true;
}

bool AutoTrack::ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height)
{
  if (image_data_size != image_width * image_height * 4)
  {
    logger.error("传入图像的通道数不对应，需要传入BGRA四通道图像");
    return false;
  }
  auto map_block = cv::Mat(image_height, image_width, CV_8UC4, (void*)image_data, cv::Mat::AUTO_STEP);
  if (map_block.empty())
  {
    logger.error("传入图像为空");
    return false;
  }
  //res.set_map_block(id_x, id_y, map_block);
  UNREFERENCED_PARAMETER(id_x);
  UNREFERENCED_PARAMETER(id_y);

  return false;
}

bool AutoTrack::ImportMapBlockCenter(int x, int y)
{
  UNREFERENCED_PARAMETER(x);
  UNREFERENCED_PARAMETER(y);
  return false;
}

bool AutoTrack::ImportMapBlockCenterScale(int x, int y, double scale)
{
  UNREFERENCED_PARAMETER(x);
  UNREFERENCED_PARAMETER(y);
  UNREFERENCED_PARAMETER(scale);
  return false;
}

bool AutoTrack::SetHandle(long long int handle)
{
  if (handle == 0)
  {
    genshin_handle.config.is_auto_find_genshin = true;
    return true;
  }
  else
  {
    genshin_handle.config.is_auto_find_genshin = false;
    genshin_handle.handle = (HWND)handle;
  }
  bool isWin = IsWindow(genshin_handle.handle);
  if (isWin)
    logger.info("手动设置原神句柄成功");
  else
    logger.error(std::format("HWND:\"{0}\"不是窗口"));
}

bool AutoTrack::SetWorldCenter(double x, double y)
{
  genshin_avatar_position.target_map_world_center.x = x;
  genshin_avatar_position.target_map_world_center.y = y;
  logger.info(std::format("世界中心被重新设置为：({0},{1})", x, y));
  return true;
}

bool AutoTrack::SetWorldScale(double scale)
{
  genshin_avatar_position.target_map_world_scale = scale;
  logger.info(std::format("世界尺度被重新设置为：({0})", scale));
  return true;
}

bool AutoTrack::startServe()
{
  return false;
}

bool AutoTrack::stopServe()
{
  return false;
}

bool AutoTrack::setLogLevel(char* level)
{
  if (level == nullptr || strlen(level) == 0)
    return false;

  std::map<std::string, TianLi::Utils::LogLevel> logLevelMapper
  {
    std::make_pair("ALL", TianLi::Utils::LogLevel::ALL),
      std::make_pair("TRACE", TianLi::Utils::LogLevel::TRACE),
      std::make_pair("DEBUG", TianLi::Utils::LogLevel::DEBUG),
      std::make_pair("INFO", TianLi::Utils::LogLevel::INFO),
      std::make_pair("WARN", TianLi::Utils::LogLevel::WARN),
      std::make_pair("ERROR", TianLi::Utils::LogLevel::ERROR),
      std::make_pair("FATAL", TianLi::Utils::LogLevel::FATAL),
      std::make_pair("OFF", TianLi::Utils::LogLevel::OFF)
  };

  if (logLevelMapper.find(level) != logLevelMapper.end())
  {
    logger.setLogLevel(logLevelMapper[level]);
    return true;
  }
  return false;
}

bool AutoTrack::GetVersion(char* version_buff, int buff_size)
{
  if (version_buff == NULL || buff_size < 1)
  {
    logger.error(std::format("获取版本号({0})失败，缓冲区参数为空", TianLi::Version::build_version));
    return false;
  }
  if (TianLi::Version::build_version.size() > buff_size)
  {
    logger.error(std::format("获取版本号({0})失败，缓冲区大小不足", TianLi::Version::build_version));
    return false;
  }
  strcpy_s(version_buff, buff_size, TianLi::Version::build_version.c_str());
  logger.info(std::format("获取版本号({0})成功", TianLi::Version::build_version));
  return true;
}

bool AutoTrack::GetCompileTime(char* time_buff, int buff_size)
{
  if (time_buff == NULL || buff_size < 1)
  {
    logger.error(std::format("获取编译时间({0})失败，缓冲区参数为空", TianLi::Version::build_time));
    return false;
  }
  if (TianLi::Version::build_time.size() > buff_size)
  {
    logger.error(std::format("获取编译时间({0})失败，缓冲区大小不足", TianLi::Version::build_time));
    return false;
  }
  strcpy_s(time_buff, buff_size, TianLi::Version::build_time.c_str());
  logger.info(std::format("获取编译时间({0})成功", TianLi::Version::build_time));
  return true;
}

//目前这里始终返回true
bool AutoTrack::GetMapIsEmbedded()
{
  return res.map_is_embedded();
}

bool AutoTrack::DebugCapture()
{
  return DebugCapturePath("Capture.png", 12);
}

bool AutoTrack::DebugCapturePath(const char* path_buff, int buff_size)
{
  if (path_buff == NULL || buff_size < 1)
  {
    logger.error("设置追踪截图路径失败，路径缓冲区为空");
    return false;
  }

  if (genshin_screen.img_screen.empty())
  {
    logger.error("保存追踪截图失败，画面为空");
    return false;
  }
  cv::Mat out_info_img = genshin_screen.img_screen.clone();
  switch (genshin_handle.config.capture->mode)
  {
  case Capture::Bitblt:
  {
    // 绘制paimon Rect
    cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
    // 绘制miniMap Rect		      
    cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
    cv::Rect Avatar = genshin_minimap.rect_avatar;
    Avatar.x += genshin_minimap.rect_minimap.x;
    Avatar.y += genshin_minimap.rect_minimap.y;

    // 绘制avatar Rect
    cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
    // 绘制UID Rect
    cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
    break;
  }
  case Capture::DirectX:
  {
    // 绘制paimon Rect
    cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
    // 绘制miniMap Rect			      
    cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
    cv::Rect Avatar = genshin_minimap.rect_avatar;
    Avatar.x += genshin_minimap.rect_minimap.x;
    Avatar.y += genshin_minimap.rect_minimap.y;

    // 绘制avatar Rect
    cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
    // 绘制UID Rect
    cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
  }
  }

  auto last_time_stream = std::stringstream();
  last_time_stream << genshin_screen.last_time;
  std::string last_time_str = last_time_stream.str();

  cv::putText(out_info_img, last_time_str, cv::Point(out_info_img.cols / 2, out_info_img.rows / 2), 1, 1, cv::Scalar(128, 128, 128, 255), 1, 16, 0);
  //
  //auto err_msg_str = err.toJson();
  //cv::putText(out_info_img, err_msg_str, cv::Point(0, out_info_img.rows / 2 - 100), 1, 1, cv::Scalar(128, 128, 128, 128), 1, 16, 0);

  bool rel = cv::imwrite(path_buff, out_info_img);

  if (!rel)
  {
    logger.error(std::format("保存画面失败，请检查文件路径是否合法({0})", path_buff));
    return false;
  }

  logger.info("追踪截图保存成功");
  return true;
}

bool AutoTrack::GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
  double x2 = 0, y2 = 0, a2 = 0;
  int mapId2 = 0;
  if (!genshin_minimap.is_init_finish)
  {
    init();//初始化
  }

  if (!GetPositionOfMap(x2, y2, mapId2))
  {
    return false;
  }

  GetDirection(a2);
  x = x2;
  y = y2;
  a = a2;
  mapId = mapId2;
  return true;
}

bool AutoTrack::GetPosition(double& x, double& y)
{
  if (try_get_genshin_windows() == false)
  {
    //日志在try_get_genshin_windows打过了。
    return false;
  }
  if (!genshin_minimap.is_init_finish)
  {
    init();
  }
  if (getMiniMapRefMat() == false)
  {
    //日志移交到getMiniMapRefMat
    return false;
  }

  if (genshin_minimap.img_minimap.empty())
  {
    logger.error("坐标获取失败，画面为空");
    return false;
  }
  genshin_minimap.config.is_find_paimon = true;

  TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);

  cv::Point2d pos = genshin_avatar_position.position;

  x = pos.x;
  y = pos.y;
  return true;
}

bool AutoTrack::GetPositionOfMap(double& x, double& y, int& mapId)
{
  mapId = 0;
  bool isSuccess = GetPosition(x, y);
  if (isSuccess != true)
  {
    return false;
  }

  auto raw_pos = TianLi::Utils::ConvertSpecialMapsPosition(x, y);
  mapId = raw_pos.second;
  if (mapId == 0)
  {
    auto user_Pos = TianLi::Utils::TransferAxes(raw_pos.first, genshin_avatar_position.target_map_world_center, genshin_avatar_position.target_map_world_scale);
    x = user_Pos.x;
    y = user_Pos.y;
  }
  else
  {
    x = raw_pos.first.x;
    y = raw_pos.first.y;
  }
  logger.info("坐标获取成功");
  return true;
}

bool AutoTrack::GetDirection(double& a)
{
  if (try_get_genshin_windows() == false)
  {
    return false;
  }
  if (getMiniMapRefMat() == false)
  {
    //日志移交到getMiniMapRefMat
    return false;
  }
  if (genshin_minimap.rect_avatar.empty())
  {
    logger.error("玩家朝向获取失败，没有截取到小箭头");
    return false;
  }

  //待重写，设法把日志移动到AutoTrack
  bool isSuccess = direction_calculation(genshin_minimap.img_avatar, a);
  return isSuccess;
}

bool AutoTrack::GetRotation(double& a)
{
  if (try_get_genshin_windows() == false)
  {
    return false;
  }
  if (getMiniMapRefMat() == false)
  {
    //err = { 3001, "获取视角朝向时，没有识别到paimon" };
    return false;
  }

  rotation_calculation_config config;
  rotation_calculation(genshin_minimap.img_minimap, a, config);
  if (config.error)
  {
    err = config.err;
    return false;
  }

  return true;
}

bool AutoTrack::GetStar(double& x, double& y, bool& isEnd)
{
  static bool isNotSee = false;
  static std::vector<cv::Point2d> pos;
  static int seeId = 0;
  static bool isStarVisible = false;

  int MAXLOOP = 0;
  bool isLoopMatch = false;
  cv::Mat tmp;
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  double scale = 1.3;
  bool isOnCity = false;

  if (isNotSee)
  {
    if (isOnCity)
    {
      scale = 0.8667;
    }
    x = pos[seeId].x * scale;
    y = pos[seeId].y * scale;
    seeId++;
    if (seeId == pos.size())
    {
      isEnd = true;
      isNotSee = false;
    }
    else
    {
      isEnd = false;
      isNotSee = true;
    }
    return true();
  }
  else
  {
    pos.clear();
    seeId = 0;

    if (try_get_genshin_windows() == false)
    {
      return false;
    }

    if (getMiniMapRefMat() == false)
    {
      //err = { 4001, "获取神瞳时，没有识别到paimon" };
      return false;
    }

    if (genshin_minimap.img_minimap.empty())
    {
      err = { 5, "原神小地图区域为空" };
      return false;
    }
    cv::Mat giStarRef;

    cv::cvtColor(genshin_minimap.img_minimap(cv::Rect(36, 36, genshin_minimap.img_minimap.cols - 72, genshin_minimap.img_minimap.rows - 72)),
      giStarRef, cv::COLOR_RGBA2GRAY);


    matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
    minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
    //cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
    if (maxVal < 0.66)
    {
      isStarVisible = false;
    }
    else
    {
      isLoopMatch = true;
      isStarVisible = true;
      pos.emplace_back(cv::Point2d(maxLoc) -
        cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
        cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
    }

    while (isLoopMatch)
    {
      giStarRef(cv::Rect(maxLoc.x, maxLoc.y, res.StarTemplate.cols, res.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
      matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
      minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
      //cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif

      if (maxVal < 0.66)
      {
        isLoopMatch = false;
      }
      else
      {
        pos.emplace_back(cv::Point2d(maxLoc) -
          cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
          cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
      }

      MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
    }


    if (isStarVisible == true)
    {
      if (isOnCity)
      {
        scale = 0.8667;
      }
      x = pos[seeId].x * scale;
      y = pos[seeId].y * scale;
      seeId++;
      if (seeId == pos.size())
      {
        isEnd = true;
        isNotSee = false;
      }
      else
      {
        isEnd = false;
        isNotSee = true;
      }
      return clear_error_logs();
    }
    err = { 601,"获取神瞳失败，未确定原因" };
    return false;
  }
}

bool AutoTrack::GetStarJson(char* jsonBuff)
{
  if (try_get_genshin_windows() == false)
  {
    return false;
  }

  if (getMiniMapRefMat() == false)
  {
    //err = { 4001, "获取神瞳时，没有识别到paimon" };
    return false;
  }

  if (genshin_minimap.img_minimap.empty())
  {
    err = { 5, "原神小地图区域为空" };
    return false;
  }
  cv::Mat giStarRef;

  //一个bug 未开游戏而先开应用，开游戏时触发
  cv::cvtColor(genshin_minimap.img_minimap(cv::Rect(36, 36, genshin_minimap.img_minimap.cols - 72, genshin_minimap.img_minimap.rows - 72)),
    giStarRef, cv::COLOR_RGBA2GRAY);

  star_calculation_config config;
  config.is_on_city = genshin_avatar_position.config.is_on_city;
  star_calculation(giStarRef, jsonBuff, config);
  if (config.error)
  {
    err = config.err;
    return false;
  }

  return clear_error_logs();
}

bool AutoTrack::GetUID(int& uid)
{
  if (try_get_genshin_windows() == false)
  {
    return false;
  }

  cv::Mat& giUIDRef = genshin_screen.img_uid;

  std::vector<cv::Mat> channels;

  split(giUIDRef, channels);

  if (genshin_handle.config.capture->mode == Capture::DirectX)
  {
    cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
  }
  else
  {
    giUIDRef = channels[3];
  }

  uid_calculation_config config;
  uid_calculation(giUIDRef, uid, config);
  if (config.error)
  {
    err = config.err;
    return false;
  }

  return clear_error_logs();
}

bool AutoTrack::GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
  if (try_get_genshin_windows() == false)
  {
    return false;
  }
  if (!genshin_minimap.is_init_finish)
  {
    init();
  }
  if (getMiniMapRefMat() == false)
  {
    //err = { 1001, "获取所有信息时，没有识别到paimon" };
    return false;
  }
  if (genshin_minimap.img_minimap.empty())
  {
    err = { 5, "原神小地图区域为空" };
    return false;
  }
  if (genshin_minimap.rect_avatar.empty())
  {
    err = { 11,"原神角色小箭头区域为空" };
    return false;
  }

  // x,y,mapId
  {
    genshin_minimap.config.is_find_paimon = true;
    GetPositionOfMap(x, y, mapId);
  }


  // a
  {
    direction_calculation_config  config;
    direction_calculation(genshin_minimap.img_avatar, a, config);
    if (config.error)
    {
      err = config.err;
    }
  }
  // r
  {
    rotation_calculation_config config;
    rotation_calculation(genshin_minimap.img_minimap, r, config);
    if (config.error)
    {
      err = config.err;
    }
  }
  cv::Mat& giUIDRef = genshin_screen.img_uid;
  // uid
  {
    std::vector<cv::Mat> channels;

    cv::split(giUIDRef, channels);

    if (genshin_handle.config.capture->mode == Capture::DirectX)
    {
      cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
    }
    else
    {
      giUIDRef = channels[3];
    }

    uid_calculation_config config;
    uid_calculation(giUIDRef, uid, config);
    if (config.error)
    {
      err = config.err;
    }
  }

#ifdef _DEBUG
  showMatchResult(x, y, mapId, a, r);
#endif // _DEBUG
  return clear_error_logs();
}

bool AutoTrack::GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
{
  UNREFERENCED_PARAMETER(path);
  UNREFERENCED_PARAMETER(uid);
  UNREFERENCED_PARAMETER(x);
  UNREFERENCED_PARAMETER(y);
  UNREFERENCED_PARAMETER(a);
  return false;
}

bool AutoTrack::GetInfoLoadVideo(char* path, char* pathOutFile)
{
  UNREFERENCED_PARAMETER(path);
  UNREFERENCED_PARAMETER(pathOutFile);
  return false;
}

int AutoTrack::GetLastError()
{
#ifdef _DEBUG
  std::cout << err;
#endif
  return err;
}

int AutoTrack::GetLastErrMsg(char* msg_buff, int buff_size)
{
  if (msg_buff == NULL || buff_size < 1)
  {
    err = { 291,"缓存区为空指针或是缓存区大小为小于1" };
    return false;
  }
  std::string msg = err.getLastErrorMsg();
  if (msg.size() > buff_size)
  {
    err = { 292,"缓存区大小不足" };
    return false;
  }
  strcpy_s(msg_buff, buff_size, msg.c_str());
  return true;
}

int AutoTrack::GetLastErrJson(char* json_buff, int buff_size)
{
  if (json_buff == NULL || buff_size < 1)
  {
    err = { 291,"缓存区为空指针或是缓存区大小为小于1" };
    return false;
  }
  std::string msg = err.toJson();
  if (msg.size() > buff_size)
  {
    err = { 292,"缓存区大小不足" };
    return false;
  }
  strcpy_s(json_buff, buff_size, msg.c_str());
  return true;
}

bool AutoTrack::try_get_genshin_windows()
{
  if (!getGengshinImpactWnd())
  {
    logger.error("未能找到原神进程");
    return false;
  }
  if (!getGengshinImpactScreen())
  {
    logger.error("原神进程在运行中，但无法获取到画面");
    return false;
  }
  return true;
}

bool AutoTrack::getGengshinImpactWnd()
{
  TianLi::Genshin::get_genshin_handle(genshin_handle);
  if (genshin_handle.handle == NULL)
  {
    return false;
  }

  genshin_handle.config.capture->setHandle(genshin_handle.handle);

  return true;
}

bool AutoTrack::getGengshinImpactScreen()
{
  TianLi::Genshin::get_genshin_screen(genshin_handle, genshin_screen);
  if (genshin_screen.img_screen.empty())
  {
    return false;
  }
  return true;
}

bool AutoTrack::getMiniMapRefMat()
{
  genshin_minimap.img_minimap = genshin_screen.img_screen(genshin_minimap.rect_minimap);


  if (genshin_handle.config.capture->mode == Capture::DirectX || genshin_handle.config.is_force_used_no_alpha)
  {
    genshin_screen.config.is_used_alpha = false;
  }
  else
  {
    genshin_screen.config.is_used_alpha = true;
  }

  // 检测派蒙 -> 直接计算小地图坐标

  if (TianLi::Genshin::Check::check_paimon(genshin_screen, genshin_paimon) == false)
  {
    if (genshin_handle.config.capture->mode == Capture::Bitblt)
    {
      //err = { 40101,"Bitblt模式下检测派蒙失败" };
      return false;
    }
    else if (genshin_handle.config.capture->mode == Capture::DirectX)
    {
      //err = { 40201,"DirectX模式下检测派蒙失败" };
      return false;
    }
  }
  if (genshin_paimon.is_visial == false)
  {
    if (genshin_handle.config.capture->mode == Capture::Bitblt)
    {
      //err = { 40102,"Bitblt模式下没有检测到派蒙" };
      return false;
    }
    else if (genshin_handle.config.capture->mode == Capture::DirectX)
    {
      //err = { 40202,"DirectX模式下没有检测到派蒙" };
      return false;
    }
  }

  genshin_screen.config.rect_paimon = genshin_paimon.rect_paimon;
  genshin_screen.config.is_handle_mode = genshin_paimon.is_handle_mode;
  genshin_screen.config.is_search_mode = genshin_paimon.is_search_mode;

  if (TianLi::Genshin::Cailb::cailb_minimap(genshin_screen, genshin_minimap) == false)
  {
    if (genshin_handle.config.capture->mode == Capture::Bitblt)
    {
      //err = { 40105,"Bitblt模式下计算小地图区域失败" };
      return false;
    }
    else if (genshin_handle.config.capture->mode == Capture::DirectX)
    {

      //err = { 40205,"DirectX模式下计算小地图区域失败" };
      return false;
    }
  }

#ifdef _DEBUG
  cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
  cv::imshow("MiniMap", genshin_minimap.img_minimap);
  cv::waitKey(1);
#endif
  return true;
}

#ifdef _DEBUG
Resources* resource = &Resources::getInstance();
inline void AutoTrack::showMatchResult(float x, float y, int mapId, float angle, float rotate)
{
  cv::Point2d pos(x, y);
  //转换到绝对坐标
  if (mapId == 0)
    pos = TianLi::Utils::TransferAxes_inv(pos, genshin_avatar_position.target_map_world_center, genshin_avatar_position.target_map_world_scale);

  //获取附近的地图
  cv::Mat gi_map = resource->MapTemplate;
  cv::Mat subMap = TianLi::Utils::get_some_map(gi_map, pos, 150).clone();

  cv::Point2i center(subMap.size[1] / 2, subMap.size[0] / 2);

  //绘制扇形
  cv::Mat sectorMask = subMap.clone();
  cv::ellipse(sectorMask, center, cv::Size2i(100, 100), -rotate - 135, 0, 90, cv::Scalar(255, 255, 255, 100), -1, cv::LINE_AA);
  cv::addWeighted(subMap, 0.75, sectorMask, 0.25, 0, subMap);

  //绘制玩家方向
  cv::Point2i direct(0, 0);
  direct.x = -(30 * sin((angle / 180.0) * _Pi)) + center.x;
  direct.y = -(30 * cos((angle / 180.0) * _Pi)) + center.y;
  cv::arrowedLine(subMap, center, direct, cv::Scalar(255, 255, 0), 5, cv::LINE_AA, 0, 0.5);

  //在图中显示坐标信息
  cv::imshow("Visual Debug", subMap);
}
#endif

