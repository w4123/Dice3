#include "dice_bot_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_db.h"
#include "dice_msg.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bot_module::bot_module() {
        db->exec(
            "CREATE TABLE IF NOT EXISTS info (id INTEGER NOT NULL, type INTEGER NOT NULL, disabled INTEGER NOT NULL, "
            "banned INTEGER NOT NULL)");
    }

    bool bot_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[.。]bot.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        if (std::regex_match(ws, re)) return true;
        return std::get<1>(search_db(e));
    }

    std::tuple<bool, bool> bot_module::search_db(const cq::event::MessageEvent& e) {
        SQLite::Statement statement(*db, "SELECT disabled, banned FROM info WHERE id = ? AND type = ?");
        if (e.message_type == cq::message::DISCUSS) {
            statement.bind(1, *e.target.discuss_id);
        } else if (e.message_type == cq::message::GROUP) {
            statement.bind(1, *e.target.group_id);
        } else {
            statement.bind(1, *e.target.user_id);
        }
        statement.bind(2, e.message_type);
        if (statement.executeStep()) {
            if (statement.getColumn(0).getInt() || statement.getColumn(1).getInt()) {
                return std::make_tuple(true, true);
            }
            return std::make_tuple(true, false);
        }
        return std::make_tuple(false, false);
    }
    void bot_module::insert_db(const cq::event::MessageEvent& e, bool disabled) {
        SQLite::Statement statement(*db, "INSERT INTO info (id, type, disabled, banned) VALUES (?, ?, ?, 0)");
        if (e.message_type == cq::message::DISCUSS) {
            statement.bind(1, *e.target.discuss_id);
        } else if (e.message_type == cq::message::GROUP) {
            statement.bind(1, *e.target.group_id);
        } else {
            statement.bind(1, *e.target.user_id);
        }
        statement.bind(2, e.message_type);
        statement.bind(3, disabled);
        statement.exec();
    }

    void bot_module::update_db(const cq::event::MessageEvent& e, bool disabled) {
        SQLite::Statement statement(*db, "UPDATE info SET disabled = ? WHERE id = ? AND type = ?");
        statement.bind(1, disabled);
        if (e.message_type == cq::message::DISCUSS) {
            statement.bind(2, *e.target.discuss_id);
        } else if (e.message_type == cq::message::GROUP) {
            statement.bind(2, *e.target.group_id);
        } else {
            statement.bind(2, *e.target.user_id);
        }
        statement.bind(3, e.message_type);
        statement.exec();
    }
    void bot_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[.。]bot[ ]*(on|off)?[ ]*([0-9]*).*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring command = std::wstring(m[1].first, m[1].second);
            std::wstring target = std::wstring(m[2].first, m[2].second);
            std::wstring self_id = std::to_wstring(cq::api::get_login_user_id());
            if (target.empty() || target == self_id || target == self_id.substr(self_id.length() - 4)) {
                if (command == L"on") {
                    if (std::get<0>(search_db(e))) {
                        update_db(e, false);
                    } else {
                        insert_db(e, false);
                    }
                    cq::api::send_msg(e.target, msg::global_msg["strEnabled"]);
                } else if (command == L"off") {
                    if (std::get<0>(search_db(e))) {
                        update_db(e, true);
                    } else {
                        insert_db(e, true);
                    }
                    cq::api::send_msg(e.target, msg::global_msg["strDisabled"]);
                } else {
                    cq::api::send_msg(e.target, msg::dice_full_info);
                }
            }
        }
    }
} // namespace dice
