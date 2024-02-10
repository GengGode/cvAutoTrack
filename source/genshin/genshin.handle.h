#pragma once
#include "global/global.genshin.h"

namespace TianLi::Genshin
{
    tianli::global::GenshinHandle func_get_handle(HWND& in);

    void get_genshin_handle(tianli::global::GenshinHandle& genshin_handle);

    // TODO: 需要将获取UI布局的方法分离出来
    void GetUiRects(tianli::global::GenshinHandle& genshin_handle);

    void update_genshin_handle(const HWND& old_handle, tianli::global::GenshinHandle& out_genshin_handle);
} // namespace TianLi::Genshin
