这是英文，点击查看[中文](https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/blob/master/README.md) 

This content is from Google Translate.

# GenshinImpact AutoTrack DLL


Through the image matching algorithm, the DLL dynamic link library for obtaining the position of the character on the map from the Genshin Impact client

[![GitHub version](https://badge.fury.io/gh/GengGode%2FGenshinImpact_AutoTrack_DLL.svg)](https://badge.fury.io/gh/GengGode%2FGenshinImpact_AutoTrack_DLL) [![Build status](https://ci.appveyor.com/api/projects/status/1q2jfn373bc15raa?svg=true)](https://ci.appveyor.com/project/GengGode/genshinimpact-autotrack-dll) ![convention](https://img.shields.io/badge/convention-__stdcall-orange.svg) ![platform](https://img.shields.io/badge/platform-Windows-blue.svg) ![](https://img.shields.io/badge/cpu-AMD64-purple.svg)

# introduce 

### Currently supports any resolution and handle mode, but does not support the [Following View] of the minimap setting, only supports [Lock Direction]
### The map currently supports regions

| Genshin Impact Version | Map Area | dll Version |
| -------- | ------ | ------ |
| 1.0 | Mond | - |
| - | Snow Mountain | - |
| - | Liyue | - |
| - | Inawa I | - |
| - | Inawife II | - |
| - | Inatsu III (Crane View) | - |
| - | Inatsu IIII (Fuchixia Palace) | - |
| - | Liyue II (Layer Rock) | - |
| - | Liyue III (Underground Rock) | - |
| 3.0 | Xumi | 6.1.51 |
| 3.1 | Sumeru Desert | 6.4.17 |

## ~~Support NVIDIA GPU graphics computing acceleration~~ (remote compilation does not support cuda, so I sent it)

### Functions supported by each version

| Version | Keyboard Mode | Gamepad Mode | Resolution Support List |
| -------- | ------ | ------ | ------ |
| 6.4.x | √ | × | 1920x1080 |
| 6.5.39 | √ | × | 1920x1080, 2560x1440, 2560x1080 |
| 6.5.92 | √ | √(Not supported in DX mode) | 16:9 series (but not 4K and above), 21:9 series, and all resolutions below 1920*1080 |

## New version planned 7.0.0

- [ ] Added inertial acceleration navigation algorithm for positioning
- [ ] Added smooth transition algorithm for positioning
- [ ] Added automatic calibration algorithm for positioning
- [ ] Fully support controller mode
- [ ] Fully supports any resolution, 4K and above
- [ ] supports any map area
- [ ] support incoming map instead of inline
- [ ] optional embedded precomputed result

### Interface modification scheme

- [ ] Refactor the interface to uniformly use double as floating-point parameters
    - `bool GetTransform(float &x, float &y, float &a)` -> `bool GetTransform(double &x, double &y, double &a)`
- [ ] Refactored the interface to uniformly use int as the return value of the interface
    - `bool GetPosition(double &x, double &y)` -> `int GetPosition(double &x, double &y)`
- `bool GetDirection(double &a)` -> `int GetDirection(double &a)`

# how to use

1. Download the compiled dynamic link library.
   - https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/releases/
2. After loading the dynamic link library, call or encapsulate the related functions according to the [Function Manual] (#Function Manual).
3. For the invocation of some languages, please refer to the invocation example in the `impl` folder.
4. Since the default interface outputs the coordinates of [Tianli Coordinate Model] (Tianli Coordinate Model), users need to set the world center and scaling factor according to the mapping relationship between their own map coordinate system and the Tianli coordinate model, or manually convert them later

# How to compile

1. Environment requirements, [Visual Studio 2019+](https://visualstudio.microsoft.com/zh-hans/vs/), [Opencv 4.5.0 static lib zip](https://github.com/GengGode/ opencv450Release/releases/download/v1.0/Release.zip)
2. Install Visual Studio 2019+
3. Create the environment variables of the Opencv library. If you do not use the environment variables, you need to manually modify the `additional include directory` and `additional library directory` of the cvAutoTrack project

    ``` $OpenCvDir = C:\projects\opencv\ ```
4. Download Opencv 4.5.0 static lib zip and extract it to any directory

    ``` 7z x ./*.zip -y -o%OpenCvDir% ```
5. Clone code

   ``` Git Clone https://github.com/GengGode/GenshinImpact_AutoTrack_DLL ```

6. Go to the project folder and unzip the resource file

    ``` cd .\cvAutoTrack | 7z -x resource.zip ```
7. Compile the project

    ``` msbuild cvAutoTrack.sln ```

## Precautions

- Currently only the dynamic link library (.dll) on the Windows x64 platform is compiled. For Windows x86 or other platforms, such as the Linux platform (.so), you need to download the source code and compile it separately.
- Windows stipulates that 64-bit process/DLL and 32-bit process/DLL cannot call/load each other, so before calling dynamic link library, please make sure that the calling process is **64-bit process** .
- This project uses the window screenshots of the game screen to perform image processing algorithms to achieve all functions. It does not read and write the game memory, so there is no risk of being banned, but the effect has certain limitations.
- The project has only been tested under limited conditions. To troubleshoot errors, it is strongly recommended to configure the environment as described below.
  - Genshin Impact Client > Paimon in the upper right corner > Settings > Antialiasing, set to `SMAA`
  - Genshin Impact Client > Paimon in the upper right corner > Settings > Others > Small Map Lock, set to `Lock Direction`
  
# For developers

- Warehouse on Code Cloud: [https://gitee.com/Yu_Sui_Xian/yuanshen-auto-tracking-dll](https://gitee.com/Yu_Sui_Xian/yuanshen-auto-tracking-dll)
- Repository on Git: [https://github.com/GengGode/GenshinImpact_AutoTrack_DLL](https://github.com/GengGode/GenshinImpact_AutoTrack_DLL)

## project structure

- **cvAutoTrack**, dll works
- **impl**, calling examples for some languages
    - **Cpp/TEST_cvAutoTrack_Cpp** C++ call
    - **CSharp/TEST_cvAutoTrack_CSharp** C# call
    - **Python** Python call


# function directory

| Interface Name | Interface Function |
| ----------------------------------------- | ------------------------------------------------------------------------- |
| `verison`                  | Get the Dll version.                                                                                                                            |
| `init`                     | Initializes the runtime environment.                                                                                                            |
| `uninit`                   | Unload the memory occupied by initialization.                                                                                                   |
| `startServe`               | (Incomplete) Start the service, start the loop detection thread.                                                                                |
| `stopServe`                | (Not done) Stops the service, stops the loop detection thread.                                                                                  |
| ... | ... |                                                                                                                                                                  |
| `GetGpuCount`              | (To be removed) Gets the number of GPU devices available to the machine.                                                                        |
| `SetGpuDevice`             | (To be removed) Sets the enabled GPU device.                                                                                                    |
| ... | ... |                                                                                                                                                                  |
| `SetUseBitbltCaptureMode`  | Set to use Bitblt capture, enabled by default, faster and better, but some systems do not support it.                                           |
| `SetUseDx11CaptureMode`    | Set to use DirectX capture, the effect is poor, but it supports win11 and above systems.                                                        |
| `SetHandle`                | Set the window handle of Genshin Impact client.                                                                                                 |
| `SetWorldCenter`           | Set the coordinates in the celestial coordinate model where the origin center of the mapping target map coordinate system is located.           |
| `SetWorldScale`            | Sets the ratio of the zoom factor of the map coordinate system of the mapping target to the zoom factor of the celestial coordinate model.      |
| ... | ... |                                                                                                                                                                  |
| `DebugCapture`             | Get the UID, current character position and angle (arrow direction) in the local video and save it to the text.                                 |
| ... | ... |                                                                                                                                                                  |
| `GetTransform`             | (to be deleted) Get the current character's position and angle (the direction of the arrow).                                                    |
| `GetTransformOfMap`        | Get the current character's position, angle (direction of the arrow) and the map area and remap the coordinate area.                            |
| `GetPosition`              | (to be deleted) Get the current position of the character.                                                                                      |
| `GetPositionOfMap`         | Get the location map area of ​​the location and remap the coordinate area.                                                                        |
| `GetDirection`             | Get the current angle (the direction of the arrow).                                                                                             |
| `GetRotation`              | Get the current view orientation (view orientation).                                                                                            |
| `GetUID`                   | Get the player's UID displayed in the bottom right corner of the screen.                                                                        |
| ... | ... |                                                                                                                                                                  |
| `GetInfoLoadPicture`       | Get the UID in the local picture, the current character's position and the angle (the direction of the arrow).                                  |
| `GetInfoLoadVideo`         | Get the UID, current character position and angle (arrow direction) in the local video and save it to the text.                                 |
| ... | ... |                                                                                                                                                                  |
| `GetLastErr`               | Get the last set error code.                                                                                                                    |
| `GetLastErrStr`            | (To be removed) Get the last set error message.                                                                                                 |
| `GetLastErrMsg`            | Get the last set error message.                                                                                                                 |
                          

## Tianli coordinate model                                                                                                                                                     

Since the map in the Genshin Impact game will continue to expand, and there are differences in the resolution of map textures corresponding to different physical blocks, it is not advisable to directly base on the pixel coordinate system of the texture, which is not good for future expansion.

Therefore, based on the existing map blocks of Genshin Impact 2.0, a more reasonable and easy-to-expandable map model has been planned, called the Tianli coordinate model.

After that, all interfaces related to map coordinates will be output in this coordinate system by default.

At present, the model is scaled with the map of the town location in the game as the standard scale size, that is, there is no zoom-in or zoom-out size.

According to this size, the other textures are scaled to the same scale, and finally combined into a general image, the scaling factor at this time is the scaling factor of the celestial coordinate model.

Under this zoom factor, in Genshin Impact 2.1 version, the total image size is
![Coordinate Range](https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/blob/master/doc/page1.svg)
After that, select the center of the altar of the rite of worship in Liyue as the origin of the world, namely `( 0 , 0 )`.

Afterwards, in order to facilitate the coordinate calculation of Opencv, the map is set to be from Mond to Liyue, which is the positive direction of the abscissa.

The map is from Liyue to Inatsuma, and the ordinate is in the positive direction.

which is

![Coordinate Range](https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/blob/master/doc/page2.svg)

### Fuchigo Palace and Underground Rocks

This part of the area will be redone the coordinate mapping in the future. For the time being, only a brief description will be given, that is, the upper right corner of the map is the origin, and the rest will be as usual.

# Coming soon

> `bool getAllInfo(double &x,double &y,double &a,int &uid);`

Get all the information, get the latest status data