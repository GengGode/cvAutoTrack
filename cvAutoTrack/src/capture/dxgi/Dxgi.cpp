#include "pch.h"
#include "Dxgi.h"

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


Dxgi::Dxgi(
    IDirect3DDevice const& device,
    GraphicsCaptureItem const& item)
{
    m_item = item;
    m_device = device;

    // Set up 
    auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    d3dDevice->GetImmediateContext(m_d3dContext.put());

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
    m_lastSize = size;
    m_frameArrived = m_framePool.FrameArrived(auto_revoke, { this, &Dxgi::OnFrameArrived });

    //d3dDevice->CreateTexture2D(&desc, nullptr, &m_pTexture);
}

// Start sending capture frames
void Dxgi::StartCapture()
{
    CheckClosed();
    m_session.StartCapture();
}

ICompositionSurface Dxgi::CreateSurface(
    Compositor const& compositor)
{
    CheckClosed();
    return CreateCompositionSurfaceForSwapChain(compositor, m_swapChain.get());
}

// Process captured frames
void Dxgi::Close()
{
    auto expected = false;
    if (m_closed.compare_exchange_strong(expected, true))
    {
        m_frameArrived.revoke();
        m_framePool.Close();
        m_session.Close();

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
	static ID3D11Texture2D* BufferTexture;
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

            desc.Width = m_lastSize.Width;
            desc.Height = m_lastSize.Height;

            m_swapChain->ResizeBuffers(
                2,
                static_cast<uint32_t>(m_lastSize.Width),
                static_cast<uint32_t>(m_lastSize.Height),
                static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized),
                0);
        }

        {
            auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
			
            {
                D3D11_MAPPED_SUBRESOURCE mappedTex;
                {
                    ID3D11Texture2D* bufferTexture;

                    auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
                    d3dDevice->CreateTexture2D(&desc, nullptr, &bufferTexture);


                    m_d3dContext->CopyResource(bufferTexture, frameSurface.get());


                    m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);
                    //m_d3dContext->Unmap(bufferTexture, 0);
					
                    bufferTexture->Release();
                }

                cv::Mat frameImage = cv::Mat::zeros(m_lastSize.Height, m_lastSize.Width, CV_8UC4);// = cv::Mat(desc.Height, desc.Width, CV_8UC4, mappedTex.pData, mappedTex.RowPitch);
				memcpy(frameImage.data, mappedTex.pData, frameImage.total() * frameImage.elemSize());

            }
        }
    }

    DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
    m_swapChain->Present1(1, 0, &presentParameters);

    if (newSize)
    {
        m_framePool.Recreate(
            m_device,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            m_lastSize);
    }
}

