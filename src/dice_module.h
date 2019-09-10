/*
 * dice_module.h
 * 模块基础定义
 */

#pragma once
#include <vector>

namespace cq::event {
        struct MessageEvent;
}

namespace dice {
    // 模块基类
    class dice_module {
    public:
        dice_module();
        ~dice_module();
        dice_module(const dice_module& m) = default;
        dice_module(dice_module&& m) = default;
        dice_module& operator=(const dice_module& m) = default;
        dice_module& operator=(dice_module&& m) = default;

        // 启用模块列表
        static std::vector<dice_module*> enabled_modules;

        // 匹配消息
        virtual bool match(const cq::event::MessageEvent& e, const std::wstring& ws) = 0;

        // 处理消息
        virtual void process(const cq::event::MessageEvent& e, const std::wstring& ws) = 0;
    };
} // namespace dice