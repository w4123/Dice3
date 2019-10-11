/*
 * dice_event.cpp
 * 主事件处理
 */

#include "cqsdk/cqsdk.h"
#include "dice_bot_module.h"
#include "dice_coc_module.h"
#include "dice_db.h"
#include "dice_dnd_module.h"
#include "dice_help_module.h"
#include "dice_jrrp_module.h"
#include "dice_module.h"
#include "dice_r_module.h"
#include "dice_rules_module.h"
#include "dice_set_module.h"

CQ_MAIN {
    // 应用启用时调用，进行模块启用
    cq::app::on_enable = [] {
        // 连接数据库
        dice::db::db = std::make_unique<SQLite::Database>(
            cq::api::get_app_directory() + "DiceConfig_" + std::to_string(cq::api::get_login_user_id()) + ".db",
            SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE,
            3000);

        dice::db::InitialiseDB();

        static dice::bot_module BotModule;

        static dice::help_module HelpModule;

        static dice::rules_module RulesModule;

        static dice::set_module SetModule;

        static dice::jrrp_module JrrpModule;

        static dice::coc_module CocModule;

        static dice::dnd_module DndModule;

        // 启用普通掷骰模块
        static dice::r_module RModule;
    };

    // 主消息处理函数
    auto main_func = [](const auto &e) {
        if (!e.message.empty() && e.message.begin()->type == "at"
            && e.message.begin()->data.at("qq") != std::to_string(cq::api::get_login_user_id())) {
            return;
        }

        // 转换为宽字符串，用于正则匹配
        std::wstring ws = cq::utils::s2ws(e.message.extract_plain_text());

        std::wregex re(L"[ ]*[\\.。．].*");

        if (!std::regex_match(ws, re)) return;

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
            try {
                process_module->process(e, ws);
            } catch (std::exception &ex) {
                cq::api::send_msg(e.target, ex.what());
                cq::logging::debug("Dice! V3", ex.what());
            }
        }
    };

    // 将主消息处理函数绑定到三种消息事件上
    cq::event::on_private_msg = main_func;
    cq::event::on_group_msg = main_func;
    cq::event::on_discuss_msg = main_func;

    cq::app::on_disable = [] { dice::db::db = nullptr; };
    cq::app::on_coolq_exit = [] { dice::db::db = nullptr; };
}

CQ_MENU(menu_semi_replace_db)
{
    dice::db::SemiReplaceDB();
    MessageBoxW(nullptr, L"操作\"半重置数据库\"已完成", L"Dice!", MB_OK | MB_ICONINFORMATION);
}
CQ_MENU(menu_replace_db)
{
    dice::db::ReplaceDB();
    MessageBoxW(nullptr, L"操作\"重置数据库\"已完成", L"Dice!", MB_OK | MB_ICONINFORMATION);
}
    