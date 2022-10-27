#include "pch.h"
#include "Dxgi.h"
//#include <opencv2/core/directx.hpp>
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
    auto expected = false;
    if (m_closed.compare_exchange_strong(expected, true))
    {
        if (is_need_init)
        {
            return;
        }
		
        try {
            if (m_session != nullptr) 
                m_session.Close();
        }
        catch (...) {
			//
        }

        try {
        if(m_framePool!= nullptr)    
            m_framePool.Close();
        }
        catch (...) {
			//
        }
		
        m_session = nullptr;
        m_framePool = nullptr;
        m_swapChain = nullptr;
        m_item = nullptr;
    }
}

bool Dxgi::init()
{
    // 只需要定义一下，不会用上，唯一的作用是避免依赖d3d11.dll
    // 这背后大概有什么科学原理吧，可能
    static cv::VideoCapture Video;
	
    static bool is_frist = true;
    if (is_frist)
    {
        m_device = CreateDirect3DDevice(TianLi::DirectX::dxgiDevice.get());
        is_frist = false;
    }
	
    if (is_need_init == false)
    {
        return true;
    }

	if(!giHandle)
    {
        err = { 10003,"句柄为空" };
        return false;
    }
	
    TianLi::DirectX::d3dDevice->GetImmediateContext(m_d3dContext.put());

    m_item = CreateCaptureItemForWindow(giHandle);

    auto size = m_item.Size();

    m_swapChain = CreateDXGISwapChain(
        TianLi::DirectX::d3dDevice,
        static_cast<uint32_t>(size.Width),
        static_cast<uint32_t>(size.Height),
        static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized),
        2);
    try {

        //auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
		
        auto fun_get_frame_pool = [=]()->auto {
            auto device= CreateDirect3DDevice(TianLi::DirectX::dxgiDevice.get());
            return winrt::Windows::Graphics::Capture::
                Direct3D11CaptureFramePool::Create(device, DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, size);
        };
		
		// 另一个线程运行 get_frame_pool
        std::future<Direct3D11CaptureFramePool> f_frame_pool = std::async(std::launch::async, fun_get_frame_pool);
        auto frame_pool = f_frame_pool.get();
		

        /*const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool
            frame_pool = winrt::Windows::Graphics::Capture::
            Direct3D11CaptureFramePool::Create(
                m_device,
                DirectXPixelFormat::B8G8R8A8UIntNormalized,
                2,
                size);*/
        const winrt::Windows::Graphics::Capture::GraphicsCaptureSession session =
            frame_pool.CreateCaptureSession(m_item);

        m_framePool = frame_pool;
        m_session = session;
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
    catch (const winrt::hresult_error&)
    {
        // Ignore any errors
    }
    catch (...)
    {
        // Ignore any errors
    }
#endif

    m_lastSize = size;

    if (m_closed.load() == true)
    {
        throw winrt::hresult_error(RO_E_CLOSED);
    }

    m_session.StartCapture();


    is_need_init = false;
    return true;
}

bool Dxgi::uninit()
{
    is_need_init = true;
	return true;
}

bool Dxgi::capture(cv::Mat& frame)
{
    static ID3D11Texture2D* bufferTexture;

	// 获取新的画面
	auto new_frame = m_framePool.TryGetNextFrame();
    if (new_frame == nullptr)
    {
        err = { 10004,"未能获取到新一帧画面" };
        return false;
    }
	auto frame_size = new_frame.ContentSize();
    if(desc.Width != static_cast<UINT>(m_lastSize.Width) || desc.Height != static_cast<UINT>(m_lastSize.Height))
    {
        desc.Width = m_lastSize.Width;
        desc.Height = m_lastSize.Height;
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
    TianLi::DirectX::d3dDevice->CreateTexture2D(&desc, nullptr, &bufferTexture);
    m_d3dContext->CopyResource(bufferTexture, frameSurface.get());
    if (bufferTexture == nullptr)
    {
        err = { 10005,"未能从GPU拷贝画面到CPU" };
        return false;
    }
	
    D3D11_MAPPED_SUBRESOURCE mappedTex;
    m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);
	
	auto data = mappedTex.pData;
	auto pitch = mappedTex.RowPitch;
	
	// 将画面转换为OpenCV的Mat
	frame = cv::Mat(frame_size.Height, frame_size.Width, CV_8UC4, (void*)data, pitch);
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
