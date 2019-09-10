#include "dice_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_echo_module.h"

// namespace cq::app 包含插件本身的生命周期事件和管理
// namespace cq::event 用于注册 QQ 相关的事件处理函数
// namespace cq::api 用于调用酷 Q 提供的接口
// namespace cq::logging 用于日志
// namespace cq::message 提供封装了的 Message 等类

// 插件入口，在静态成员初始化之后，app::on_initialize 事件发生之前被执行，用于配置 SDK 和注册事件回调
CQ_MAIN {
    cq::app::on_enable = [] {
        static dice::echo_module EchoModule;
        // cq::logging、cq::api、cq::dir 等命名空间下的函数只能在事件回调函数内部调用，而不能直接在 CQ_MAIN 中调用
        cq::logging::debug(u8"Dice!启用", u8"Dice!插件已启动");
    };

    cq::event::on_private_msg = [](const auto &e) {
        dice::dice_module *process_module = nullptr;
        for (auto &m : dice::dice_module::enabled_modules) {
            if (m->match(e)) {
                process_module = m;
                break;
            }
        }
        if (process_module) {
            e.block();
            process_module->process(e);
        }
    };

    cq::event::on_group_msg = [](const auto &e) {
        dice::dice_module *process_module = nullptr;
        for (auto &m : dice::dice_module::enabled_modules) {
            if (m->match(e)) {
                process_module = m;
                break;
            }
        }
        if (process_module) {
            e.block();
            process_module->process(e);
        }
    };

    cq::event::on_discuss_msg = [](const auto &e) {
        dice::dice_module *process_module = nullptr;
        for (auto &m : dice::dice_module::enabled_modules) {
            if (m->match(e)) {
                process_module = m;
                break;
            }
        }
        if (process_module) {
            e.block();
            process_module->process(e);
        }
    };
}
