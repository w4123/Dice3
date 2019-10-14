#include "dice_help_module.h"
#include <SQLiteCpp/Statement.h>
#include "cqsdk/cqsdk.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool help_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*help.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void help_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*help[ ]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring origin_query(m[1]);
            std::transform(origin_query.begin(), origin_query.end(), origin_query.begin(), std::towlower);
            if (origin_query == L"on") {
                utils::set_help_enabled(e.target, true);
                cq::api::send_msg(e.target, msg::GetGlobalMsg("strSetHelpEnabled"));
            } else if (origin_query == L"off") {
                utils::set_help_enabled(e.target, false);
                cq::api::send_msg(e.target, msg::GetGlobalMsg("strSetHelpDisabled"));
            } else {
                if (!utils::is_help_enabled(e.target)) {
                    cq::api::send_msg(e.target, msg::GetGlobalMsg("strHelpDisabled"));
                    return;
                }
                std::string query;
                if (origin_query.empty()) {
                    query = "default";
                } else {
                    query = cq::utils::ws2s(origin_query);
                }
                cq::api::send_msg(e.target, msg::GetHelpMsg(query));
            }
        }
    }
} // namespace dice
