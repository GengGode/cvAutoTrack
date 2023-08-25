#include "pch.h"
#include "Dxgi.h"
#include <future>

using namespace winrt;
using namespace Windows;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::Graphics;
using namespace Windows::Graphics::Capture;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Composition;

namespace TianLi::DirectX
{
    static bool is_init_d3d = false;
    static winrt::com_ptr<ID3D11Device> d3dDevice;// = CreateD3DDevice();
    static winrt::impl::com_ref<IDXGIDevice> dxgiDevice;// = d3dDevice.as<IDXGIDevice>();
}

Dxgi::Dxgi()
{
    mode = Capture::DirectX;
    if (TianLi::DirectX::is_init_d3d == false)
    {
        TianLi::DirectX::is_init_d3d = true;
        TianLi::DirectX::d3dDevice = CreateD3DDevice();
        TianLi::DirectX::dxgiDevice = TianLi::DirectX::d3dDevice.as<IDXGIDevice>();
    }
}

Dxgi::~Dxgi()
{
    try
    {
        if (m_session != nullptr)
            m_session.Close();
        if (m_framePool != nullptr)
            m_framePool.Close();
    }
    catch (...)
    {
        //
    }
    m_session = nullptr;
    m_framePool = nullptr;
    m_swapChain = nullptr;
    m_item = nullptr;
}

bool Dxgi::init()
{
    // 只需要定义一下，不会用上，唯一的作用是避免依赖d3d11.dll
    // 这背后大概有什么科学原理吧，可能
    static cv::VideoCapture Video;

    if (is_need_init == false)
    {
        return true;
    }

    if (!giHandle)
    {
        err = { 10003,"句柄为空" };
        return false;
    }

    // TODO: 中断时重开原神，进入这里
    m_device = CreateDirect3DDevice(TianLi::DirectX::dxgiDevice.get());
    m_item = CreateCaptureItemForWindow(giHandle);
    if (m_item == nullptr)
    {
        err = { 10013,"未能捕获窗口" };
        return false;
    }
    auto size = m_item.Size();
    m_swapChain = CreateDXGISwapChain(TianLi::DirectX::d3dDevice, static_cast<uint32_t>(size.Width), static_cast<uint32_t>(size.Height), static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized), 2);

    if (size.Width < 480 || size.Height < 360)
    {
        err = { 14, "窗口画面大小小于480x360，无法使用" };
        return false;
    }

    TianLi::DirectX::d3dDevice->GetImmediateContext(m_d3dContext.put());

    try
    {
        auto fun_get_frame_pool = [=]()->auto {
            auto device = CreateDirect3DDevice(TianLi::DirectX::dxgiDevice.get());
            return winrt::Windows::Graphics::Capture::
                Direct3D11CaptureFramePool::Create(device, DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, size);
        };
        // 另一个线程运行 get_frame_pool
        std::future<Direct3D11CaptureFramePool> f_frame_pool = std::async(std::launch::async, fun_get_frame_pool);

        m_framePool = f_frame_pool.get();
        m_session = m_framePool.CreateCaptureSession(m_item);
    }
    catch (...)
    {
        // Ignore any errors
    }

    // 判断 WindowsSDK 版本大于等于 10.0.22000.0
#if (WINVER >= _WIN32_WINNT_WIN10_21H1)
    try
    {
        if (winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.Graphics.Capture.GraphicsCaptureSession", L"IsBorderRequired"))
        {

            winrt::Windows::Graphics::Capture::GraphicsCaptureAccess::
                RequestAccessAsync(
                    winrt::Windows::Graphics::Capture::
                    GraphicsCaptureAccessKind::Borderless)
                .get();
            m_session.IsBorderRequired(false);
            m_session.IsCursorCaptureEnabled(false);
        }
    }
    catch (...)
    {
        // Ignore any errors
    }
#endif

    m_lastSize = size;

    m_session.StartCapture();

    is_need_init = false;
    return true;
}

bool Dxgi::uninit()
{
    is_need_init = true;
    return true;
}
bool get_client_box(HWND window, uint32_t width, uint32_t height,
    D3D11_BOX* client_box)
{
    RECT client_rect{}, window_rect{};
    POINT upper_left{};

    /* check iconic (minimized) twice, ABA is very unlikely */
    bool client_box_available =
        !IsIconic(window) && GetClientRect(window, &client_rect) &&
        !IsIconic(window) && (client_rect.right > 0) &&
        (client_rect.bottom > 0) &&
        (DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS,
            &window_rect,
            sizeof(window_rect)) == S_OK) &&
        ClientToScreen(window, &upper_left);
    if (client_box_available) {
        const uint32_t left =
            (upper_left.x > window_rect.left)
            ? (upper_left.x - window_rect.left)
            : 0;
        client_box->left = left;

        const uint32_t top = (upper_left.y > window_rect.top)
            ? (upper_left.y - window_rect.top)
            : 0;
        client_box->top = top;

        uint32_t texture_width = 1;
        if (width > left) {
            texture_width =
                min(width - left, (uint32_t)client_rect.right);
        }

        uint32_t texture_height = 1;
        if (height > top) {
            texture_height =
                min(height - top, (uint32_t)client_rect.bottom);
        }

        client_box->right = left + texture_width;
        client_box->bottom = top + texture_height;

        client_box->front = 0;
        client_box->back = 1;

        client_box_available = (client_box->right <= width) &&
            (client_box->bottom <= height);
    }

    return client_box_available;
}

bool Dxgi::capture(cv::Mat& frame)
{
    static ID3D11Texture2D* bufferTexture;
    if (m_framePool == nullptr)
    {
        uninit();
        try
        {
            if (m_session != nullptr)
                m_session.Close();
            if (m_framePool != nullptr)
                m_framePool.Close();
        }
        catch (...)
        {
            //
        }
        m_session = nullptr;
        m_framePool = nullptr;
        m_swapChain = nullptr;
        m_item = nullptr;
        bool res = init();
        if (res == false)
        {
            err = { 10001,"重新初始化捕获池" };
            return false;
        }
    }

    // 判断是否可以获取新的的画面
    Direct3D11CaptureFrame new_frame{ nullptr };
    // 获取最新的画面
    try
    {
        // Debug下每次都获取最新的画面
#ifdef _DEBUG
        Direct3D11CaptureFrame new_frame_null{ nullptr };
        do
        {
            new_frame = m_framePool.TryGetNextFrame();
            if (new_frame == nullptr)
            {
                err = { 10002,"获取新的画面失败" };
                return false;
            }
            new_frame_null = m_framePool.TryGetNextFrame();
            if (new_frame_null != nullptr)
            {
                new_frame.Close();
                new_frame = new_frame_null;
            }
        } while (new_frame_null == nullptr);
#else
        new_frame = m_framePool.TryGetNextFrame();
#endif // _DEBUG
    }
    catch (...)
    {
        // Ignore any errors
        err = { 10003,"获取下一帧画面失败" };
        return false;
    }
    if (new_frame == nullptr)
    {
        err = { 10004,"未能获取到新一帧画面" };
        return false;
    }
    auto frame_size = new_frame.ContentSize();
    if (desc_type.Width != static_cast<UINT>(m_lastSize.Width) || desc_type.Height != static_cast<UINT>(m_lastSize.Height))
    {
        desc_type.Width = m_lastSize.Width;
        desc_type.Height = m_lastSize.Height;
    }
    if (frame_size.Width != m_lastSize.Width || frame_size.Height != m_lastSize.Height)
    {
        m_framePool.Recreate(
            m_device,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            frame_size);
        m_lastSize = frame_size;

        m_swapChain->ResizeBuffers(
            2,
            static_cast<uint32_t>(m_lastSize.Width),
            static_cast<uint32_t>(m_lastSize.Height),
            static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized),
            0);

    }
    auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(new_frame.Surface());

    //auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    TianLi::DirectX::d3dDevice->CreateTexture2D(&desc_type, nullptr, &bufferTexture);
    D3D11_BOX client_box;
    bool client_box_available = get_client_box(giHandle, desc_type.Width, desc_type.Height, &client_box);

    if (client_box_available)
    {
        m_d3dContext->CopySubresourceRegion(bufferTexture,
            0, 0, 0, 0, frameSurface.get(),
            0, &client_box);
    }
    else
    {
        m_d3dContext->CopyResource(bufferTexture, frameSurface.get());
    }
    if (bufferTexture == nullptr)
    {
        err = { 10005,"未能从GPU拷贝画面到CPU" };
        return false;
    }

    D3D11_MAPPED_SUBRESOURCE mappedTex;
    m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);



    auto data = mappedTex.pData;
    auto pitch = mappedTex.RowPitch;
    if (data == nullptr)
    {
        err = { 10006,"指针指向为空" };
        return false;
    }

    // 将画面转换为OpenCV的Mat（疑似会崩溃）
    try {
        frame = cv::Mat(frame_size.Height, frame_size.Width, CV_8UC4, (void*)data, pitch);
    }
    catch(std::exception e){
        err = { 504, std::format("FATAL!! 出现了致命的错误，已自动跳过，原因:{}",e.what())};
        return false;
    }
    if (frame_size.Width < 480 || frame_size.Height < 360)
    {
        err = { 14, "窗口画面大小小于480x360，无法使用" };
        return false;
    }

    if (client_box_available)
    {
        if (client_box.right - client_box.left > frame_size.Width || client_box.bottom - client_box.top > frame_size.Height)
        {
            err = { 14, "窗口画面小于裁剪框，截图失败" };
            return false;
        }
        frame = frame(cv::Rect(0, 0, client_box.right - client_box.left, client_box.bottom - client_box.top));
    }

    frame = frame.clone();
    // 释放资源
    bufferTexture->Release();
    return true;
}

bool Dxgi::setHandle(HWND handle)
{
    if (giHandle != handle)
    {
        if (handle == nullptr)
        {
            err = { 10006,"设置的句柄为空" };
            return false;
        }
        uninit();
        giHandle = handle;
        init();
    }
    return true;
}
