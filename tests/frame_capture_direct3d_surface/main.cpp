#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include <frame/frame.include.h>
#include <frame/capture/capture.include.h>
#include <frame/capture/capture.bitblt.h>
#include <utils/convect.string.h>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// 获取屏幕截图
IDirect3DSurface9 *GetScreenShot(IDirect3DDevice9 *pDevice)
{
    IDirect3DSurface9 *pSurface = nullptr;
    D3DDISPLAYMODE displayMode;
    // 检索显示模式的空间分辨率、颜色分辨率和刷新频率。
    pDevice->GetDisplayMode(0, &displayMode);
    std::cout << displayMode.Width << displayMode.Height << std::endl;
    // 创建屏幕外图面。
    HRESULT hr = pDevice->CreateOffscreenPlainSurface(displayMode.Width, displayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
    if (FAILED(hr))
    {
        return nullptr;
    }
    // 生成设备的前缓冲区的副本，并将该副本放置在应用程序提供的系统内存缓冲区中。
    hr = pDevice->GetFrontBufferData(0, pSurface);
    if (FAILED(hr))
    {
        return nullptr;
    }
    return pSurface;
}


int main()
{
    // 1.初始化 Direct3D 设备
    LPDIRECT3D9 pD3D = nullptr; // Direct3D 对象是图形应用程序需要创建的第一个 Direct3D COM 对象，也是应用程序需要释放的最后一个对象
    if (nullptr == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    { // 创建 IDirect3D9 对象并返回一个接口。
        return E_FAIL;
    }
    // 2.创建 Direct3D 设备
    LPDIRECT3DDEVICE9 pDevice = nullptr;
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;                 // 如果应用程序运行窗口，则为 TRUE;如果应用程序全屏运行，则为 FALSE。
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY; // 运行时将保证有关缓冲区交换行为的隐式语义;
    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, FindWindow(NULL, L"原神"),
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice)))
    { // 创建一个设备来表示显示适配器。指定默认适配器、硬件抽象层 (HAL) 设备和软件顶点处理。
        return E_FAIL;
    }

    // 3.获取屏幕截图
    IDirect3DSurface9 *pSrcSurface = GetScreenShot(pDevice);

    // 4.转为 Mat
    D3DSURFACE_DESC desc;
    pSrcSurface->GetDesc(&desc);
    cv::Mat frame(desc.Height, desc.Width, CV_8UC4);
    D3DLOCKED_RECT lockedRect;
    pSrcSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
    memcpy(frame.data, lockedRect.pBits, desc.Height * desc.Width * 4);
    pSrcSurface->UnlockRect();

    // 6.释放资源
    pDestSurface->Release();
    pSrcSurface->Release();
    pDevice->Release();
    pD3D->Release();

    return 0;
}