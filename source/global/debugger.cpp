#include "pch.h"
#include "debugger.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#define WINDLE_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct window_opengl
{
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;

    WNDCLASSEXW wc;
    thread_local static inline int width = 0;
    thread_local static inline int height = 0;
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND create(int width, int height);
    void shutdown();
};

LRESULT WINAPI window_opengl::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED)
            {
                window_opengl::width = LOWORD(lParam);
                window_opengl::height = HIWORD(lParam);
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY: ::PostQuitMessage(0); return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

HWND window_opengl::create(int width, int height)
{
    this->width = width;
    this->height = height;

    hwnd = nullptr;
    hdc = nullptr;
    hglrc = nullptr;

    wc = { sizeof(WNDCLASSEXW), CS_OWNDC, window_opengl::WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"DebuggerClass", nullptr };
    auto register_res = ::RegisterClassExW(&this->wc);
    if (register_res == 0)
        return nullptr;
    hwnd = ::CreateWindowW(this->wc.lpszClassName, L"Debugger", WS_OVERLAPPEDWINDOW, 100, 100, this->width, this->height, nullptr, nullptr, this->wc.hInstance, nullptr);

    auto CreateDeviceWGL = [&]() {
        HDC hDc = ::GetDC(hwnd);
        PIXELFORMATDESCRIPTOR pfd = { 0 };
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        const int pf = ::ChoosePixelFormat(hDc, &pfd);
        if (pf == 0)
            return false;
        if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
            return false;
        ::ReleaseDC(hwnd, hDc);

        hdc = ::GetDC(hwnd);
        if (!this->hglrc)
            this->hglrc = wglCreateContext(hdc);
        return true;
    };
    if (!CreateDeviceWGL())
    {
        ::wglMakeCurrent(nullptr, nullptr);
        ::ReleaseDC(hwnd, hdc);
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, this->wc.hInstance);
        return nullptr;
    }
    ::wglMakeCurrent(hdc, this->hglrc);

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    return hwnd;
}

void window_opengl::shutdown()
{
    if (::wglGetCurrentContext() == nullptr)
        ::wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hwnd, hdc);
    ::wglDeleteContext(hglrc);

    if (::IsWindow(hwnd))
    {
        ::ShowWindow(hwnd, SW_HIDE);
        ::SetParent(hwnd, nullptr);
        ::DestroyWindow(hwnd);
    }
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

#include "single_async_executor.hpp"
#include <atomic>
#include <iostream>
struct imapp
{
    window_opengl window;
    stdex::single_async_executor<void> executor;
    std::atomic_flag is_running = ATOMIC_FLAG_INIT;

    void async_execute()
    {
        executor.submit_exclusive([this]() {
            try
            {
                create();
                while (is_running.test_and_set())
                {
                    event_process();
                    new_frame();
                    render();
                }
                destory();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "Unknown exception occurred." << std::endl;
            }
        });
    }

    void create();
    void event_process();
    void new_frame();
    void render();
    void destory();
};
void imapp::create()
{
    if (window.create(1280, 720) == nullptr)
        throw std::runtime_error("Failed to create window");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_InitForOpenGL(window.hwnd);
    ImGui_ImplOpenGL3_Init();

    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    is_running.test_and_set();
}
void imapp::event_process()
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            is_running.clear();
    }
    if (::IsIconic(window.hwnd))
        ::Sleep(10);
}
void imapp::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}
void imapp::render()
{
    ImGui::Render();
    glViewport(0, 0, window.width, window.height);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ::SwapBuffers(window.hdc);
}
void imapp::destory()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    window.shutdown();
}

struct debugger::impl_t
{
    imapp app;

    void setup() { app.async_execute(); }
    void shutdown()
    {
        app.is_running.clear();
        app.executor.wait();
    }
};

debugger::debugger() : impl(std::make_unique<impl_t>()) {}
debugger::~debugger() { impl->shutdown(); }

void debugger::initlize()
{
    impl->setup();
}
void debugger::destory()
{
    impl->shutdown();
}
std::string debugger::call(std::string command, std::string args)
{
    return {};
}