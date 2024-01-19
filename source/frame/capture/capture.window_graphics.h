#pragma once
#include "capture.include.h"
#include "utils/utils.window_graphics.h"
#include "utils/utils.window_scale.h"
#include <global/global.include.h>
#include <utils/convect.string.h>

namespace tianli::frame::capture
{
    class capture_window_graphics : public capture_source
    {
        winrt::com_ptr<IInspectable> m_device{ nullptr };
        winrt::com_ptr<ID3D11DeviceContext> m_d3dContext{ nullptr };
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
        winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
        winrt::Windows::Graphics::SizeInt32 m_lastSize;
        winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };

    public:
        capture_window_graphics(std::shared_ptr<global::logger> logger = nullptr) : capture_source(logger)
        {
            this->type = source_type::window_graphics;
            auto init_global = utils::window_graphics::graphics_global::get_instance();
        }
        ~capture_window_graphics() override { uninitialized(); }

    public:
        bool initialization() override
        {
            if (this->is_initialized)
                return true;

            if (this->is_callback)
                this->source_handle = this->source_handle_callback();
            if (IsWindow(this->source_handle) == false)
                return false;

            m_device = utils::window_graphics::CreateDirect3DDevice(utils::window_graphics::graphics_global::get_instance().dxgi_device.get());
            m_item = utils::window_graphics::CreateCaptureItemForWindow(this->source_handle);

            if (m_item == nullptr)
                return false;

            m_lastSize = m_item.Size();
            m_swapChain = utils::window_graphics::CreateDXGISwapChain(utils::window_graphics::graphics_global::get_instance().d3d_device, static_cast<uint32_t>(m_lastSize.Width),
                                                                      static_cast<uint32_t>(m_lastSize.Height), DXGI_FORMAT_B8G8R8A8_UNORM, 2);

            utils::window_graphics::graphics_global::get_instance().d3d_device->GetImmediateContext(m_d3dContext.put());

            m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(m_device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(),
                                                                                                static_cast<winrt::Windows::Graphics::DirectX::DirectXPixelFormat>(87), 2, m_lastSize);
            m_session = m_framePool.CreateCaptureSession(m_item);

            utils::window_graphics::set_capture_session_property(m_session);

            m_session.StartCapture();

            this->is_initialized = true;
            return true;
        }

        bool uninitialized() override
        {
            if (this->is_initialized == false)
                return true;

            if (m_session != nullptr)
                m_session.Close();
            if (m_framePool != nullptr)
                m_framePool.Close();
            m_session = nullptr;
            m_framePool = nullptr;
            m_swapChain = nullptr;
            m_item = nullptr;

            this->is_initialized = false;
            return true;
        }

        bool set_capture_handle(HWND handle = 0) override
        {
            if (handle == nullptr)
                return false;
            if (handle == this->source_handle)
                return true;
            if (uninitialized() == false)
                return false;
            this->source_handle = handle;
            if (initialization() == false)
                return false;
            this->is_callback = false;
            return true;
        }

        bool set_source_handle_callback(std::function<HWND()> callback) override
        {
            if (callback == nullptr)
                return false;
            if (uninitialized() == false)
                return false;
            this->source_handle_callback = callback;
            if (initialization() == false)
                return false;
            this->is_callback = true;
            return true;
        }

        bool get_frame(cv::Mat& frame) override
        {
            static ID3D11Texture2D* bufferTexture;

            if (this->is_callback)
                set_capture_handle(this->source_handle_callback());

            if (m_framePool == nullptr)
            {
                uninitialized();
                if (initialization() == false)
                    return false;
            }

            winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame new_frame{ nullptr };

            new_frame = m_framePool.TryGetNextFrame();
            if (new_frame == nullptr)
                return false;

            auto frame_size = new_frame.ContentSize();
            auto& desc = utils::window_graphics::graphics_global::get_instance().desc_type;
            if (desc.Width != static_cast<UINT>(m_lastSize.Width) || desc.Height != static_cast<UINT>(m_lastSize.Height))
            {
                desc.Width = m_lastSize.Width;
                desc.Height = m_lastSize.Height;
            }

            if (frame_size.Width != m_lastSize.Width || frame_size.Height != m_lastSize.Height)
            {
                m_framePool.Recreate(m_device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(), winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2,
                                     frame_size);
                m_lastSize = frame_size;

                m_swapChain->ResizeBuffers(2, static_cast<uint32_t>(m_lastSize.Width), static_cast<uint32_t>(m_lastSize.Height),
                                           static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized), 0);
            }
            auto frameSurface = utils::window_graphics::GetDXGIInterfaceFromObject<ID3D11Texture2D>(new_frame.Surface());

            // auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
            utils::window_graphics::graphics_global::get_instance().d3d_device->CreateTexture2D(&desc, nullptr, &bufferTexture);
            D3D11_BOX client_box;
            bool client_box_available = utils::window_graphics::get_client_box(this->source_handle, desc.Width, desc.Height, &client_box);

            if (client_box_available)
            {
                m_d3dContext->CopySubresourceRegion(bufferTexture, 0, 0, 0, 0, frameSurface.get(), 0, &client_box);
            }
            else
            {
                m_d3dContext->CopyResource(bufferTexture, frameSurface.get());
            }

            if (bufferTexture == nullptr)
            {
                return false;
            }

            D3D11_MAPPED_SUBRESOURCE mappedTex;
            m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);

            auto data = mappedTex.pData;
            auto pitch = mappedTex.RowPitch;
            if (data == nullptr)
                return false;

            frame = cv::Mat(frame_size.Height, frame_size.Width, CV_8UC4, data, pitch);
            if (client_box_available)
            {
                if (client_box.right - client_box.left > frame_size.Width || client_box.bottom - client_box.top > frame_size.Height)
                    return false;
                this->source_frame = frame(cv::Rect(0, 0, client_box.right - client_box.left, client_box.bottom - client_box.top));
            }
            // 释放资源
            bufferTexture->Release();

            if (this->source_frame.empty())
                return false;
            if (this->source_frame.cols < 480 || this->source_frame.rows < 360)
                return false;
            frame = this->source_frame;
            return true;
        }

    private:
        RECT source_rect = { 0, 0, 0, 0 };
        RECT source_client_rect = { 0, 0, 0, 0 };
        cv::Size source_client_size;
    };

} // namespace tianli::frame::capture
