#include "pch.h"
#include "Dxgi.h"
#include <opencv2/core/directx.hpp>

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


Dxgi::Dxgi()
{
    mode = Capture::Mode_DirectX;
}

bool Dxgi::init()
{
    // 只需要定义一下，不会用上，唯一的作用是避免依赖d3d11.dll
    // 这背后大概有什么科学原理吧，可能
    static cv::VideoCapture Video;

	static auto d3dDevice = CreateD3DDevice();
    static auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
    static bool is_frist = true;
    if (is_frist)
    {
        d3dDevice->GetImmediateContext(m_d3dContext.put());
        // Set up 
        //auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
        m_device = CreateDirect3DDevice(dxgiDevice.get());
    }

	if(!giHandle)
    {
        err = { 10003,"句柄为空" };
        return false;
    }

        m_item = CreateCaptureItemForWindow(giHandle);

        // auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
        //d3dDevice->GetImmediateContext(m_d3dContext.put());
        
        auto size = m_item.Size();

        m_swapChain = CreateDXGISwapChain(
            d3dDevice,
            static_cast<uint32_t>(size.Width),
            static_cast<uint32_t>(size.Height),
            static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized),
            2);

        // Create framepool, define pixel format (DXGI_FORMAT_B8G8R8A8_UNORM), and frame size. 
        m_framePool = Direct3D11CaptureFramePool::Create(
            m_device,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            size);       
		
        m_session = m_framePool.CreateCaptureSession(m_item);
		
        auto f = [=] {
            try {
                return winrt::Windows::Foundation::Metadata::ApiInformation::
                    IsPropertyPresent(
                        L"Windows.Graphics.Capture.GraphicsCaptureSession",
                        L"IsBorderRequired");
            }
            catch (const winrt::hresult_error&) {
                return false;
            }
            catch (...) {
                return false;
            }
        };
        if (f())
        {
            winrt::Windows::Graphics::Capture::GraphicsCaptureAccess::
                RequestAccessAsync(
                    winrt::Windows::Graphics::Capture::
                    GraphicsCaptureAccessKind::Borderless)
                .get();
            m_session.IsBorderRequired(false);
            m_session.IsCursorCaptureEnabled(false);
        }

        m_lastSize = size;
        m_frameArrived = m_framePool.FrameArrived(auto_revoke, { this, &Dxgi::OnFrameArrived });

        StartCapture();
        is_need_init = false;
    return true;
}

bool Dxgi::uninit()
{
	//m_session.Close();
	return true;
}

bool Dxgi::capture(cv::Mat& frame)
{
    static D3D11_TEXTURE2D_DESC desc{
   0,0,1,1,DXGI_FORMAT_B8G8R8A8_UNORM,{1,0},D3D11_USAGE_STAGING,0,D3D11_CPU_ACCESS_READ,0
    };
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
	
    auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    d3dDevice->CreateTexture2D(&desc, nullptr, &bufferTexture);
    m_d3dContext->CopyResource(bufferTexture, frameSurface.get());
    if (bufferTexture == nullptr)
    {
        err = { 10005,"未能从GPU拷贝画面到CPU" };
        return false;
    }

    //cv::Mat test;
    //cv::directx::convertFromD3D11Texture2D(bufferTexture,test);
	
    D3D11_MAPPED_SUBRESOURCE mappedTex;
    m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);
	
	auto data = mappedTex.pData;
	auto pitch = mappedTex.RowPitch;
	// 将画面转换为OpenCV的Mat
	frame = cv::Mat(frame_size.Height, frame_size.Width, CV_8UC4, (void*)data, pitch);
	// 释放资源
    bufferTexture->Release();
	// new_frame.Close();
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
		//uninit();
        giHandle = handle;
		init();
    }
    return true;
}

// Start sending capture frames
void Dxgi::StartCapture()
{
    CheckClosed();
    m_session.StartCapture();
}

// Process captured frames
void Dxgi::Close()
{
    auto expected = false;
    if (m_closed.compare_exchange_strong(expected, true))
    {
        if (is_need_init)
            return;
        m_frameArrived.revoke();
        if (m_framePool != nullptr)
        {
            m_framePool.Close();
        }
        if (m_session != nullptr)
        {
            m_session.Close();
        }
		
        m_swapChain = nullptr;
        m_framePool = nullptr;
        m_session = nullptr;
        m_item = nullptr;
    }
}

void Dxgi::OnFrameArrived(
    Direct3D11CaptureFramePool const& sender,
    winrt::Windows::Foundation::IInspectable const&)
{
    auto newSize = false;
    static D3D11_TEXTURE2D_DESC desc{
       0,0,1,1,DXGI_FORMAT_B8G8R8A8_UNORM,{1,0},D3D11_USAGE_STAGING,0,D3D11_CPU_ACCESS_READ,0
    };
    {
        auto frame = sender.TryGetNextFrame();
        auto frameContentSize = frame.ContentSize();

        if (frameContentSize.Width != m_lastSize.Width ||
            frameContentSize.Height != m_lastSize.Height)
        {
            // The thing we have been capturing has changed size.
            // We need to resize our swap chain first, then blit the pixels.
            // After we do that, retire the frame and then recreate our frame pool.
            newSize = true;
            m_lastSize = frameContentSize;
            m_swapChain->ResizeBuffers(
                2,
                static_cast<uint32_t>(m_lastSize.Width),
                static_cast<uint32_t>(m_lastSize.Height),
                static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized),
                0);
            desc.Width = m_lastSize.Width;
            desc.Height = m_lastSize.Height;
        }

        //static ID3D11Texture2D* BufferTexture;
        {
    //        auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
			
    //        {
    //            D3D11_MAPPED_SUBRESOURCE mappedTex;
    //            {
    //                ID3D11Texture2D* bufferTexture;

    //                auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    //                d3dDevice->CreateTexture2D(&desc, nullptr, &bufferTexture);


    //                m_d3dContext->CopyResource(bufferTexture, frameSurface.get());


    //                m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);
    //                //m_d3dContext->Unmap(bufferTexture, 0);
				//	
    //                //bufferTexture->Release();
    //            }

    //            cv::Mat frameImage = cv::Mat(desc.Height, desc.Width, CV_8UC4, mappedTex.pData, mappedTex.RowPitch);
				////memcpy(frameImage.data, mappedTex.pData, frameImage.total() * frameImage.elemSize());

    //        }
        }
    }
	
    if (newSize)
    {
        m_framePool.Recreate(
            m_device,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            m_lastSize);
    }
}

