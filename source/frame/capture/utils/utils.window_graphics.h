#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <inspectable.h>
#include <winrt/base.h>
#include <windows.graphics.directx.direct3d11.interop.h>

namespace tianli::frame::capture::utils::window_graphics
{
    extern "C"
    {
        HRESULT __stdcall CreateDirect3D11DeviceFromDXGIDevice(::IDXGIDevice *dxgiDevice,
                                                               ::IInspectable **graphicsDevice);

        HRESULT __stdcall CreateDirect3D11SurfaceFromDXGISurface(::IDXGISurface *dgxiSurface,
                                                                 ::IInspectable **graphicsSurface);
    }

    struct __declspec(uuid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1"))
        IDirect3DDxgiInterfaceAccess : ::IUnknown
    {
        virtual HRESULT __stdcall GetInterface(GUID const &id, void **object) = 0;
    };

    inline auto CreateD3DDevice(D3D_DRIVER_TYPE const type, winrt::com_ptr<ID3D11Device> &device)
    {
        WINRT_ASSERT(!device);
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        // #ifdef _DEBUG
        //	flags |= D3D11_CREATE_DEVICE_DEBUG;
        // #endif
        return D3D11CreateDevice(nullptr, type, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, device.put(), nullptr, nullptr);
    }

    //************************************************************************************************************
    winrt::com_ptr<ID3D11Device> init_d3d11_device()
    {
        winrt::com_ptr<ID3D11Device> device;
        HRESULT hr = CreateD3DDevice(D3D_DRIVER_TYPE_HARDWARE, device);

        if (DXGI_ERROR_UNSUPPORTED == hr)
        {
            hr = CreateD3DDevice(D3D_DRIVER_TYPE_WARP, device);
        }

        winrt::check_hresult(hr);
        return device;
    }

    winrt::impl::com_ref<IDXGIDevice> init_dxgi_device(winrt::com_ptr<ID3D11Device> device)
    {
        return device.as<IDXGIDevice>();
    }

    class graphics_global
    {
        inline auto creat_d3d_device(D3D_DRIVER_TYPE const type, winrt::com_ptr<ID3D11Device> &device)
        {
            WINRT_ASSERT(!device);
            UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
            // #ifdef _DEBUG
            //	flags |= D3D11_CREATE_DEVICE_DEBUG;
            // #endif
            return D3D11CreateDevice(nullptr, type, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, device.put(), nullptr, nullptr);
        }

        graphics_global()
        {
            HRESULT hr = creat_d3d_device(D3D_DRIVER_TYPE_HARDWARE, this->d3d_device);
            if (DXGI_ERROR_UNSUPPORTED == hr)
            {
                hr = creat_d3d_device(D3D_DRIVER_TYPE_WARP, this->d3d_device);
            }
            winrt::check_hresult(hr);
            
            dxgi_device = utils::window_graphics::init_dxgi_device(d3d_device);
            this->is_inited = true;
        }

    public:
        graphics_global &get_instance()
        {
            static graphics_global gg;
            return gg;
        }

    public:
        bool is_inited = false;
        winrt::com_ptr<ID3D11Device> d3d_device;
        winrt::impl::com_ref<IDXGIDevice> dxgi_device;
        D3D11_TEXTURE2D_DESC desc_type{
            0, 0, 1, 1, DXGI_FORMAT_B8G8R8A8_UNORM, {1, 0}, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0};
    };

    inline auto CreateDirect3DDevice(IDXGIDevice *dxgi_device)
    {
        winrt::com_ptr<::IInspectable> d3d_device;
        winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgi_device, d3d_device.put()));
        return d3d_device;
    }

    inline auto CreateDirect3DSurface(IDXGISurface *dxgi_surface)
    {
        winrt::com_ptr<::IInspectable> d3d_surface;
        winrt::check_hresult(CreateDirect3D11SurfaceFromDXGISurface(dxgi_surface, d3d_surface.put()));
        return d3d_surface;
    }

    template <typename T>
    auto GetDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const &object)
    {
        auto access = object.as<IDirect3DDxgiInterfaceAccess>();
        winrt::com_ptr<T> result;
        winrt::check_hresult(access->GetInterface(winrt::guid_of<T>(), result.put_void()));
        return result;
    }


    auto get_window_handle()
    {
        auto activation_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
        auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = {nullptr};
        interop_factory->CreateForWindow(hwnd, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), reinterpret_cast<void **>(winrt::put_abi(item)));
    }

} // namespace tianli::frame::capture::utils::window_graphics
