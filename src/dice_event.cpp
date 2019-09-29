/*
 * dice_event.cpp
 * 主事件处理
 */

#include "cqsdk/cqsdk.h"
#include "dice_db.h"
#include "dice_module.h"
#include "dice_r_module.h"
#include "dice_bot_module.h"

CQ_MAIN {
    // 应用启用时调用，进行模块启用
    cq::app::on_enable = [] {
        // 连接数据库
        db = std::make_unique<SQLite::Database>(cq::api::get_app_directory() + "DiceConfig.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE, 3000);

        static dice::bot_module BotModule;

        // 启用普通掷骰模块
        static dice::r_module RModule;
    };

    // 主消息处理函数
    auto main_func = [](const auto &e) {
        // 转换为宽字符串，用于正则匹配
        std::wstring ws = cq::utils::s2ws(e.message.extract_plain_text());

        // 遍历已启用的模块，对消息进行匹配
        dice::dice_module *process_module = nullptr;
        for (auto &m : dice::dice_module::enabled_modules) {
            if (m->match(e, ws)) {
                process_module = m;
                break;
            }
        }

        // 如果匹配到符合的模块，使用此模块进行处理
        if (process_module) {
            e.block();
            process_module->process(e, ws);
        }
    };

    // 将主消息处理函数绑定到三种消息事件上
    cq::event::on_private_msg = main_func;
    cq::event::on_group_msg = main_func;
    cq::event::on_discuss_msg = main_func;

    cq::app::on_disable = [] { db = nullptr; };
    cq::app::on_coolq_exit = [] { db = nullptr; };
}
