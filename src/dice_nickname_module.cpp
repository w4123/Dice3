#include "dice_nickname_module.h"
#include <cctype>
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"
#include "dice_msg_queue.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool nickname_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*n.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void nickname_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*n(n)?(n)?[\\s]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;

        if (std::regex_match(ws, m, re)) {
            std::string old_nick = utils::get_nickname(e.target);
            std::string nick_name = cq::utils::ws2s(m[3]);
            if (m[2].first != m[2].second) {
                if (nick_name.empty()) {
                    nick_name = utils::format_string("{%name}");
                } else {
                    std::transform(nick_name.begin(), nick_name.end(), nick_name.begin(), [](unsigned char c) {
                        return std::tolower(c);
                    });
                    nick_name = utils::format_string("{%name@{language}}", {{"language", nick_name}});
                }
            }
            if (m[1].first == m[1].second || e.message_type == cq::message::PRIVATE) {
                utils::set_global_nickname(e.target, nick_name);
            } else {
                utils::set_group_nickname(e.target, nick_name);
            }
            if (nick_name.empty()) {
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(
                        msg::GetGlobalMsg("strNickEmpty"),
                        {{"old_nick", old_nick},
                         {"is_global",
                          (m[1].first == m[1].second || e.message_type == cq::message::PRIVATE) ? "全局" : ""}}));
            } else {
                if ((m[1].first == m[1].second && e.message_type == cq::message::PRIVATE
                    || m[1].first != m[1].second && e.message_type != cq::message::PRIVATE) &&
                        utils::if_card_exist(e.target, nick_name)) {
                    utils::set_chosen_card(e.target, nick_name);
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(
                            msg::GetGlobalMsg("strNickSetWithCardChange"),
                            {{"old_nick", old_nick},
                             {"new_nick", nick_name},
                             {"is_global",
                              (m[1].first == m[1].second || e.message_type == cq::message::PRIVATE) ? "全局" : ""}, {"origin", utils::get_originname(e.target)}}));

                } else {
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(
                            msg::GetGlobalMsg("strNickSet"),
                            {{"old_nick", old_nick},
                             {"new_nick", nick_name},
                             {"is_global",
                              (m[1].first == m[1].second || e.message_type == cq::message::PRIVATE) ? "全局" : ""}}));

                }
            }
        }
    }
} // namespace dice
