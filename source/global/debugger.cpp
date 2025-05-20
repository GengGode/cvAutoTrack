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

#include "one_call_function.hpp"
#include "single_async_executor.hpp"
#include "syncer.hpp"
#include <atomic>
#include <iostream>

#include <mutex>

struct async_ogl
{
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    std::mutex mutex;
    void init(HWND hwnd, HGLRC main_hglrc)
    {
        this->hwnd = hwnd;
        hdc = ::GetDC(hwnd);
        hglrc = wglCreateContext(hdc);
        if (main_hglrc)
            wglShareLists(main_hglrc, hglrc);
    }
    void destory(HWND hwnd)
    {
        std::unique_lock lock(mutex);
        if (hglrc)
        {
            wglMakeCurrent(hdc, nullptr);
            wglDeleteContext(hglrc);
            hglrc = nullptr;
        }
        if (hdc)
        {
            ReleaseDC(hwnd, hdc);
            hdc = nullptr;
        }
    }
};

struct unique_ogl_lock
{
    async_ogl& async;
    bool ret = {};
    unique_ogl_lock(async_ogl& async) : async(async)
    {
        async.mutex.lock();
        ret = wglMakeCurrent(async.hdc, async.hglrc);
    }
    ~unique_ogl_lock()
    {
        wglMakeCurrent(async.hdc, nullptr);
        wglMakeCurrent(nullptr, nullptr);
        async.mutex.unlock();
    }
};

struct imapp
{
    window_opengl window;
    stdex::one_call_function<void()> one_call;
    stdex::single_async_executor<void> executor;
    stdex::syncer<std::function<void()>, true> render_syncer;
    std::function<void()> render;
    std::atomic_flag is_running = ATOMIC_FLAG_INIT;

    void async_execute()
    {
        executor.submit_exclusive([this]() {
            try
            {
                create();
                while (is_running.test_and_set())
                {
                    one_call();

                    event_process();
                    new_frame();

                    render_syncer.try_sync(render);
                    if (render)
                        render();

                    render_and_swap();
                }
                destory();
            }
            catch (const std::exception& e)
            {
                // std::cerr << "Exception: " << e.what() << std::endl;
            }
            catch (...)
            {
                // std::cerr << "Unknown exception occurred." << std::endl;
            }
        });
    }

    void create();
    void event_process();
    void new_frame();
    void render_and_swap();
    void destory();

    std::shared_ptr<async_ogl> create_async_ogl()
    {
        auto async = std::make_shared<async_ogl>();
        std::promise<void> promise;
        std::future<void> future = promise.get_future();
        one_call = [&promise = std::move(promise), async, this]() {
            async->init(window.hwnd, window.hglrc);
            promise.set_value();
        };
        future.wait();
        return async;
    }
};
void imapp::create()
{
    if (window.create(1280, 720) == nullptr)
        throw std::runtime_error("Failed to create window");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;                             // Disable .ini file
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
        {
            std::cout << "Quit message received." << std::endl;
            // is_running.clear();
        }
    }
    if (::IsIconic(window.hwnd))
        ::Sleep(10);
    if (::IsWindow(window.hwnd) == false)
        is_running.clear();
}
void imapp::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}
void imapp::render_and_swap()
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

#include <opencv2/core/opengl.hpp>

struct debugger::impl_t
{
    imapp app;
    std::vector<unique_render_image> render_images;
    stdex::syncer<std::vector<unique_render_image>, true> render_image_syncer;

    void setup() { app.async_execute(); }
    void shutdown()
    {
        app.is_running.clear();
        wait_exit();
    }
    void wait_exit() { app.executor.wait(); }
    void update_render(std::function<void()> render) { app.render_syncer.set(render); }
};

debugger::debugger() : impl(std::make_unique<impl_t>()) {}
debugger::~debugger()
{
    impl->shutdown();
}
#include "pool.hpp"

void debugger::initlize()
{
    impl->setup();

    set_render([this]() {
        ImGui::Begin("Debugger");
        ImGui::Text("Render Images");
        ImGui::Text("Press 'R' to refresh");
        if (ImGui::Button("Refresh"))
        {
            static stdex::single_async_executor<void> async_executor;
            async_executor.submit_exclusive([this]() {
                auto ogl = impl->app.create_async_ogl();
                unique_ogl_lock lock(*ogl);

                std::vector<unique_render_image> images;
                for (int i = 0; i < 1; ++i)
                {
                    cv::Mat image = cv::Mat::zeros(200, 200, CV_8UC3);
                    cv::rectangle(image, cv::Point(rand() % 50 + 2, 50), cv::Point(150, 150), cv::Scalar(0, 255, 0), -1);
                    unique_render_image render_image;
                    render_image.tex_ref = std::make_shared<cv::ogl::Texture2D>(image, true);
                    render_image.texture_id = render_image.tex_ref->texId();
                    render_image.width = image.cols;
                    render_image.height = image.rows;
                    images.push_back(render_image);
                }
                impl->render_image_syncer.set(images);
            });
        }
        impl->render_image_syncer.try_sync(impl->render_images);
        for (auto& image : impl->render_images)
        {
            ImGui::Image((intptr_t)image.texture_id, ImVec2(image.width, image.height));
        }
        ImGui::End();
    });
}
void debugger::destory()
{
    impl->shutdown();
}
void debugger::wait_exit()
{
    impl->wait_exit();
}
void debugger::set_render(std::function<void()> render)
{
    impl->update_render(render);
}
// #include "self_releasing_async.hpp"

#include <frame/frame.include.h>
#include <global/record/record.stdlog.h>
std::string debugger::call(std::string command, std::string args)
{
    if (command == "capture")
    {
        if (args == "current")
        {

            auto logger = std::make_shared<tianli::global::record::std_logger>();
            auto capture = tianli::frame::frame_source::create(tianli::frame::frame_source::source_type::bitblt, logger);
            cv::Mat frame;

            capture->set_capture_handle(GetDesktopWindow());
            while (frame.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(18));
                capture->get_frame(frame);
                std::cout << "Capture current frame" << std::endl;
                if (frame.empty())
                {
                    std::cout << "Capture failed" << std::endl;
                }
            }
            static stdex::single_async_executor<void> async_executor;
            async_executor.submit_exclusive([frame, this]() {
                auto ogl = impl->app.create_async_ogl();
                unique_ogl_lock lock(*ogl);
                std::vector<unique_render_image> images;
                {
                    unique_render_image render_image;
                    render_image.tex_ref = std::make_shared<cv::ogl::Texture2D>(frame.clone(), true);
                    render_image.texture_id = render_image.tex_ref->texId();
                    render_image.width = render_image.tex_ref->cols();
                    render_image.height = render_image.tex_ref->rows();
                    images.push_back(render_image);
                }
                impl->render_image_syncer.set(images);
            });
        }
    }
    else
    {
        return "Unknown command";
    }
    return {};
}