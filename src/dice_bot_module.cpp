#include "dice_bot_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_db.h"
#include "dice_msg.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool bot_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*bot.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        if (std::regex_match(ws, re)) return true;
        return search_db(e);
    }

    bool bot_module::search_db(const cq::event::MessageEvent& e) {
        if (e.message_type == cq::message::DISCUSS) {
            SQLite::Statement statement(*db::db, "SELECT bot_on, is_ban FROM group_info WHERE group_id = ? AND type = ?");
            statement.bind(1, *e.target.discuss_id);
            statement.bind(2, 1);
            if (statement.executeStep()) {
                return !statement.getColumn(0).getInt() || statement.getColumn(1).getInt();
            }
            return false;
        } 
        if (e.message_type == cq::message::GROUP) {
            SQLite::Statement statement(*db::db,
                                        "SELECT bot_on, is_ban FROM group_info WHERE group_id = ? AND type = ?");
            statement.bind(1, *e.target.group_id);
            statement.bind(2, 0);
            if (statement.executeStep()) {
                return !statement.getColumn(0).getInt() || statement.getColumn(1).getInt();
            }
            return false;
        }
        SQLite::Statement statement(*db::db, "SELECT bot_on, is_ban FROM qq_info WHERE qq_id = ?");
        statement.bind(1, *e.target.user_id);
        if (statement.executeStep()) {
            return !statement.getColumn(0).getInt() || statement.getColumn(1).getInt();
        }
        return false;
    }

    void bot_module::update_db(const cq::event::MessageEvent& e, bool bot_on) {
        if (e.message_type == cq::message::DISCUSS) {
            SQLite::Statement statement(*db::db, 
				        "REPLACE INTO group_info(group_id, type, bot_on) VALUES(?, ?, ?)");
            statement.bind(1, *e.target.discuss_id);
            statement.bind(2, 1);
            statement.bind(3, bot_on);
            statement.exec();
        } else if (e.message_type == cq::message::GROUP) {
            SQLite::Statement statement(*db::db,
                                        "REPLACE INTO group_info(group_id, type, bot_on) VALUES(?, ?, ?)");
            statement.bind(1, *e.target.group_id);
            statement.bind(2, 0);
            statement.bind(3, bot_on);
            statement.exec();
        } else {
            SQLite::Statement statement(*db::db,
                                        "REPLACE INTO qq_info(qq_id, bot_on) VALUES(?, ?)");
            statement.bind(1, *e.target.user_id);
            statement.bind(2, bot_on);
            statement.exec();
        }
    }
    void bot_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*bot[ ]*(on|off)?[ ]*([0-9]*).*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring command = m[1];
            std::wstring target = m[2];
            std::wstring self_id = std::to_wstring(cq::api::get_login_user_id());
            if (target.empty() || target == self_id || target == self_id.substr(self_id.length() - 4)) {
                if (command == L"on") {
                    if (e.message_type == cq::message::GROUP && !utils::is_admin_or_owner(e.target)) {
                        cq::api::send_msg(e.target, msg::GetGlobalMsg("strPermissionDeniedError"));
                        return;
		    }
                    update_db(e, true);
                    cq::api::send_msg(e.target, msg::GetGlobalMsg("strEnabled"));
                } else if (command == L"off") {
                    if (e.message_type == cq::message::GROUP && !utils::is_admin_or_owner(e.target)) {
                        cq::api::send_msg(e.target, msg::GetGlobalMsg("strPermissionDeniedError"));
                        return;
                    }
                    update_db(e, false);
                    cq::api::send_msg(e.target, msg::GetGlobalMsg("strDisabled"));
                } else {
                    cq::api::send_msg(e.target, msg::dice_full_info);
                }
            }
        }
    }
} // namespace dice
