#pragma once
#include "../Capture.h"
class Dxgi : public Capture
{
public:
    Dxgi();
    ~Dxgi();
	
	bool init() override;
	bool uninit() override;
	bool capture(cv::Mat& frame) override;
	bool setHandle(HWND handle = 0) override;
	
private:
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
    winrt::com_ptr<ID3D11DeviceContext> m_d3dContext{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
    /// <summary>
    /// 存储应用程序捕获的帧。
    /// 尝试从帧池获取下一个捕获的帧。
    /// 捕获帧存储在帧池中时引发的事件。
    /// </summary>
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
    /// <summary>
    /// 允许应用程序进行屏幕捕获
    /// 启动捕获会话，允许应用程序捕获帧。
    /// </summary>
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
    winrt::Windows::Graphics::SizeInt32 m_lastSize;
    winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };

    std::atomic<bool> m_closed = false;
private:
    bool is_need_init = false;
	HWND giHandle = nullptr;
private:
    D3D11_TEXTURE2D_DESC desc_type{ 0,0,1,1,DXGI_FORMAT_B8G8R8A8_UNORM,{1,0},D3D11_USAGE_STAGING,0,D3D11_CPU_ACCESS_READ,0 };
};

