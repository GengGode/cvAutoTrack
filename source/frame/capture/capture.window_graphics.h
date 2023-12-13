#pragma once
#include "global/global.include.h"
#include "capture.include.h"
#include "utils/utils.window_scale.h"
#include "utils/utils.window_graphics.h"

#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>

#include <winrt/Windows.Graphics.Capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.capture.h>

#include <winrt/windows.foundation.metadata.h>

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d2d1_3.h>

#include <functional>
#include <future>

namespace tianli::frame::capture
{
    class capture_window_graphics : public capture_source
    {
        // winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice
        winrt::com_ptr<::IInspectable> m_device{nullptr};
        winrt::com_ptr<ID3D11DeviceContext> m_d3dContext{nullptr};
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{nullptr};
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{nullptr};
        winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{nullptr};
        winrt::Windows::Graphics::SizeInt32 m_lastSize;
        winrt::com_ptr<IDXGISwapChain1> m_swapChain{nullptr};
        std::atomic<bool> m_closed = false;

        winrt::Windows::Graphics::Capture::GraphicsCaptureItem get_window_handle_from_itme(HWND handle)
        {
            auto activation_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
            auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();
            winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = {nullptr};
            interop_factory->CreateForWindow(handle, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), reinterpret_cast<void **>(winrt::put_abi(item)));
            return item;
        }

    public:
        capture_window_graphics(std::shared_ptr<global::logger> logger = nullptr) : capture_source(logger)
        {
            this->type = source_type::window_graphics;

            auto init_global = utils::window_graphics::graphics_global::get_instance();
        }
        ~capture_window_graphics() = default;

    public:
        bool initialization() override
        {
            if (IsWindow(this->source_handle) == false)
                return false;

            m_device = utils::window_graphics::CreateDirect3DDevice(utils::window_graphics::graphics_global::get_instance().dxgi_device.get());
            m_item = get_window_handle_from_itme(this->source_handle);
            if (m_item == nullptr)
                return false;
            auto size = m_item.Size();
            m_swapChain = utils::window_graphics::CreateDXGISwapChain(utils::window_graphics::graphics_global::get_instance().d3d_device, static_cast<uint32_t>(size.Width), static_cast<uint32_t>(size.Height), DXGI_FORMAT_B8G8R8X8_UNORM, 2);

            if (size.Width < 480 || size.Height < 360)
            {
                return false;
            }

            utils::window_graphics::graphics_global::get_instance().d3d_device->GetImmediateContext(m_d3dContext.put());

            try
            {
                auto fun_get_frame_pool = [=]() -> auto
                {
                    auto device = utils::window_graphics::CreateDirect3DDevice(utils::window_graphics::graphics_global::get_instance().dxgi_device.get());
                    return winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(), static_cast<winrt::Windows::Graphics::DirectX::DirectXPixelFormat>(87), 2, size);
                };
                // 另一个线程运行 get_frame_pool
                std::future<winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool> f_frame_pool = std::async(std::launch::async, fun_get_frame_pool);

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

            return true;
        }
        bool uninitialized() override
        {
            return true;
        }
        bool set_handle(HWND handle = 0) override
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
        bool get_frame(cv::Mat &frame) override
        {
            auto handle = this->source_handle;
            if (this->is_callback)
                handle = this->source_handle_callback();
            if (handle == nullptr)
                return false;
            if (IsWindow(handle) == false)
                return false;
            RECT rect = {0, 0, 0, 0};
            if (GetWindowRect(handle, &rect) == false)
                return false;
            RECT client_rect = {0, 0, 0, 0};
            if (GetClientRect(handle, &client_rect) == false)
                return false;
            double screen_scale = utils::window_scale::get_screen_scale(handle);
            cv::Size client_size = {0, 0};

            static ID3D11Texture2D *bufferTexture;
            if (m_framePool == nullptr)
            {
                uninitialized();
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
                bool res = initialization();
                if (res == false)
                {
                    return false;
                }
            }

            // 判断是否可以获取新的的画面
            winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame new_frame{nullptr};
            // 获取最新的画面
            try
            {
                // Debug下每次都获取最新的画面
#ifdef _DEBUG
                Direct3D11CaptureFrame new_frame_null{nullptr};
                do
                {
                    new_frame = m_framePool.TryGetNextFrame();
                    if (new_frame == nullptr)
                    {
                        err = {10002, "获取新的画面失败"};
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
                return false;
            }
            if (new_frame == nullptr)
            {
                return false;
            }
            auto frame_size = new_frame.ContentSize();
            auto &desc = utils::window_graphics::graphics_global::get_instance().desc_type;
            if (desc.Width != static_cast<UINT>(m_lastSize.Width) || desc.Height != static_cast<UINT>(m_lastSize.Height))
            {
                desc.Width = m_lastSize.Width;
                desc.Height = m_lastSize.Height;
            }
            if (frame_size.Width != m_lastSize.Width || frame_size.Height != m_lastSize.Height)
            {
                m_framePool.Recreate(
                    m_device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(),
                    winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                    2,
                    frame_size);
                m_lastSize = frame_size;

                m_swapChain->ResizeBuffers(
                    2,
                    static_cast<uint32_t>(m_lastSize.Width),
                    static_cast<uint32_t>(m_lastSize.Height),
                    static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
                    0);
            }
            auto frameSurface = utils::window_graphics::GetDXGIInterfaceFromObject<ID3D11Texture2D>(new_frame.Surface());

            // auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
            utils::window_graphics::graphics_global::get_instance().d3d_device->CreateTexture2D(&desc, nullptr, &bufferTexture);
            D3D11_BOX client_box;
            bool client_box_available = utils::window_graphics::get_client_box(this->source_handle, desc.Width, desc.Height, &client_box);

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
                return false;
            }

            D3D11_MAPPED_SUBRESOURCE mappedTex;
            m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);

            auto data = mappedTex.pData;
            auto pitch = mappedTex.RowPitch;
            if (data == nullptr)
            {
                return false;
            }

            // 将画面转换为OpenCV的Mat（疑似会崩溃）
            try
            {
                frame = cv::Mat(frame_size.Height, frame_size.Width, CV_8UC4, (void *)data, pitch);
            }
            catch (std::exception e)
            {
                return false;
            }
            if (client_box_available)
            {
                if (client_box.right - client_box.left > frame_size.Width || client_box.bottom - client_box.top > frame_size.Height)
                {
                    return false;
                }
                frame = frame(cv::Rect(0, 0, client_box.right - client_box.left, client_box.bottom - client_box.top));
            }
            // 释放资源
            bufferTexture->Release();
            return true;

            if (this->source_frame.empty())
                return false;
            if (this->source_frame.cols < 480 || this->source_frame.rows < 360)
                return false;
            frame = this->source_frame;
            return true;
        }

    private:
        RECT source_rect = {0, 0, 0, 0};
        RECT source_client_rect = {0, 0, 0, 0};
        cv::Size source_client_size;
    };

} // namespace tianli::capture
