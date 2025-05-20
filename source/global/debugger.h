#pragma once
#include <memory>
#include <string>

#include <opencv2/core/opengl.hpp>

struct unique_render_image
{
    int texture_id;
    int width;
    int height;
    std::shared_ptr<cv::ogl::Texture2D> tex_ref;
};

struct debugger
{
    struct impl_t;
    std::unique_ptr<impl_t> impl;
    debugger();
    ~debugger();

    void initlize();
    void destory();
    void wait_exit();
    void set_render(std::function<void()> render);
    std::string call(std::string command, std::string args);
};
