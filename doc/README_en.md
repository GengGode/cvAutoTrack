这是英文，点击到[中文](https://github.com/GengGode/cvAutoTrack/blob/master/README.md)
# GenshinImpact AutoTrack DLL

DLL dynamic link library that obtains the character's position on the map from the Genshin Impact client through the image matching algorithm

[![GitHub version](https://badge.fury.io/gh/GengGode%2FcvAutoTrack.svg)](https://badge.fury.io/gh/GengGode%2FcvAutoTrack) [![Build status](https://ci.appveyor.com/api/projects/status/12csww8lg756kucu?svg=true)](https://ci.appveyor.com/project/GengGode/cvautotrack) ![convention](https://img.shields.io/badge/convention-__stdcall-orange.svg) ![platform](https://img.shields.io/badge/platform-Windows-blue.svg) ![](https://img.shields.io/badge/cpu-AMD64-purple.svg)

## Precautions

- This project uses window screenshots of the game screen to perform image processing algorithms to achieve all functions. It does not read or write the game memory, so there is no risk of account ban, but the effect also has certain limitations.
- The project has only been tested under limited conditions. If you need to troubleshoot errors, it is strongly recommended to configure the environment as described below.
   - Genshin Impact client > Paimon in the upper right corner > Settings > Anti-aliasing, set to `SMAA`
   - Genshin Impact client > Paimon in the upper right corner > Settings > Others > Minimap lock, set to `Lock direction`

# introduce 

- [x] [Kong Ying Tavern Genshin Impact Manual Update/Testing Tutorial] (doc/kyjg.md)
- [x] Contact QQ group [717355752](https://jq.qq.com/?_wv=1027&k=YvH6TsM4)

## New version plan 7.x

- [ ] Added angle and field of view recognition in the secret realm
- [x] Realize field of view recognition without transparent channel
- [x] Implement log controllable non-output
- [x] Add inertial acceleration navigation algorithm for positioning (see [giAlgorithmVerification](https://github.com/GengGode/giAlgorithmVerification) library)
- [ ] Add automatic calibration algorithm for positioning
- [ ] Support any map area
- [ ] Supports passing in maps instead of embedding them
- [x] Optional embedded precomputed results
- [x] Higher matching accuracy
- [x] Use binary files instead of xml serialization to track feature points
- [x] No longer embed precomputed results, the first run will generate cache xml in the run directory
- [x] Currently supports any resolution and controller mode, but the [Follow Perspective] of the minimap setting is not currently supported and only [Lock Direction] is supported.

# how to use

1. Download the compiled dynamic link library.
    - https://github.com/GengGode/cvAutoTrack/releases/
2. After loading the dynamic link library, call or encapsulate the relevant functions according to [Function Directory] (#Function Directory).
3. For calling some languages, please refer to the calling examples in the [impl](impl) folder.
4. Since the default interface outputs the coordinates of [Tianli Coordinate Model] (doc/天理坐标模型_en.md), the user needs to set the world center and scaling coefficient according to the mapping relationship between their own map coordinate system and the Tianli Coordinate Model. Or convert manually later.

# How to compile (will switch to cmake build soon)

1. Environmental requirements, [Visual Studio 2019 +](https://visualstudio.microsoft.com/zh-hans/vs/) 、[Opencv 4.5.0 static lib zip](https://github.com/GengGode/opencv450Release/releases/download/v1.0/Release.zip)
2. Install Visual Studio 2019+
3. Create environment variables for the Opencv library. If you do not use environment variables, you need to manually modify the `Additional Include Directory` and `Additional Library Directory` of the cvAutoTrack project.

     ``` $OpenCvDir = C:\projects\opencv\ ```
4. Download Opencv 4.5.0 static lib zip and extract it to any directory

     ``` 7z x ./*.zip -y -o%OpenCvDir% ```
5. Clone code

    ``` Git Clone https://github.com/GengGode/cvAutoTrack ```

6. Enter the project folder and extract the resource files

     ``` cd .\cvAutoTrack | 7z -x resource.zip ```
7. Compile the project

     ``` msbuild cvAutoTrack.sln ```

# For developers

## Project structure

- **cvAutoTrack**, dll project
- **doc**, documents and some illustrations
- **impl**, calling examples in some languages
     - **Cpp/TEST_cvAutoTrack_Cpp** C++ call
     - **CSharp/TEST_cvAutoTrack_CSharp** C# call
     - **Python** Python call


# [Function Directory](doc/函数目录_en.md)

## [Tianli coordinate model](doc/%E5%A4%A9%E7%90%86%E5%9D%90%E6%A0%87%E6%A8%A1%E5%9E%8B_en.md)

## Error handling
- ### [Runtime Error](doc/%E6%97%A0%E6%B3%95%E8%BF%90%E8%A1%8C.md)
- ### [Exceptions and errors during compilation, linking, and packaging]()

# Special thanks to 
  - [Opencv](https://opencv.org/)
  - [Kongying Tavern-Genshin Map](https://yuanshen.site/docs/)
