// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

#include "framework.h"
// Opencv
#include <opencv2/opencv.hpp>

#include <Unknwn.h>
#include <inspectable.h>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <winrt/windows.foundation.metadata.h>

#include <windows.ui.composition.interop.h>
#include <DispatcherQueue.h>

#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")


// STL
#include <atomic>
#include <memory>
#include <chrono>
#include <format>
#include <fstream>
#include <filesystem>

// D3D
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <wincodec.h>

// Helpers
#include "capture/dxgi/include/composition.interop.h"
#include "capture/dxgi/include/d3dHelpers.h"
#include "capture/dxgi/include/direct3d11.interop.h"
#include "capture/dxgi/include/capture.interop.h"

#endif //PCH_H
