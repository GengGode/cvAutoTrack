#pragma once
#include <dxgi.h>
#include <d2d1_1.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <inspectable.h>
#include <winrt/base.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include <windows.ui.composition.interop.h>

namespace tianli::frame::capture::utils::window_graphics
{

    struct __declspec(uuid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1"))
        IDirect3DDxgiInterfaceAccess : ::IUnknown
    {
        virtual HRESULT __stdcall GetInterface(GUID const &id, void **object) = 0;
    };

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
            this->dxgi_device = this->d3d_device.as<IDXGIDevice>();
            this->is_inited = true;
        }

    public:
        static graphics_global &get_instance()
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

    inline auto CreateDXGISwapChain(winrt::com_ptr<ID3D11Device> const &device, const DXGI_SWAP_CHAIN_DESC1 *desc)
    {
        auto dxgiDevice = device.as<IDXGIDevice2>();
        winrt::com_ptr<IDXGIAdapter> adapter;
        winrt::check_hresult(dxgiDevice->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
        winrt::com_ptr<IDXGIFactory2> factory;
        winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));

        winrt::com_ptr<IDXGISwapChain1> swapchain;
        winrt::check_hresult(factory->CreateSwapChainForComposition(
            device.get(),
            desc,
            nullptr,
            swapchain.put()));

        return swapchain;
    }

    inline auto CreateDXGISwapChain(winrt::com_ptr<ID3D11Device> const &device, uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t bufferCount)
    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.Format = format;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferCount = bufferCount;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        return CreateDXGISwapChain(device, &desc);
    }

    bool get_client_box(HWND window, uint32_t width, uint32_t height,
                        D3D11_BOX *client_box)
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
        if (client_box_available)
        {
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
            if (width > left)
            {
                texture_width =
                    min(width - left, (uint32_t)client_rect.right);
            }

            uint32_t texture_height = 1;
            if (height > top)
            {
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
} // namespace tianli::frame::capture::utils::window_graphics
