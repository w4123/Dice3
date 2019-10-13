#include "dice_nickname_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_db.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool nickname_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*n.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void nickname_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*n(n)?[ ]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;

        if (std::regex_match(ws, m, re)) {
            std::string old_nick = utils::get_nickname(e.target);
            std::string nick_name = cq::utils::ws2s(m[2]);
            if (m[1].first == m[1].second) {
                utils::set_global_nickname(e.target, nick_name);
            } else {
                utils::set_group_nickname(e.target, nick_name);
            }
            if (nick_name.empty()) {
                cq::api::send_msg(e.target,
                                  utils::format_string(msg::GetGlobalMsg("strNickEmpty"),
                                                       {{"old_nick", old_nick},
                                                        {"is_global", m[1].first == m[1].second ? "全局" : ""}}));
            } else {
                cq::api::send_msg(e.target,
                                  utils::format_string(msg::GetGlobalMsg("strNickSet"),
                                                       {{"old_nick", old_nick},
                                                        {"new_nick", nick_name},
                                                        {"is_global", m[1].first == m[1].second ? "全局" : ""}}));
            }
        }
    }
} // namespace dice
