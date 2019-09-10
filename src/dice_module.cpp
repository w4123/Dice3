/*
 * dice_module.cpp
 * 模块基础定义
 */


#include <algorithm>
#include <vector>
#include "dice_module.h"

namespace dice {
    // 构造函数，将模块加入启用模块列表
    dice_module::dice_module()
    {
        enabled_modules.push_back(this);
    }

    // 析构函数，将模块移除启用模块列表
    dice_module::~dice_module() {
        const auto find_res = std::find(enabled_modules.begin(), enabled_modules.end(), this);
        if (find_res != enabled_modules.end()) {
            enabled_modules.erase(find_res);
        }
    }
    std::vector<dice_module*> dice_module::enabled_modules;
} // namespace dice