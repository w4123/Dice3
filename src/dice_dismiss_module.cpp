#include "dice_dismiss_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_db.h"
#include "dice_msg.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool dismiss_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        if (e.message_type==cq::message::PRIVATE)
        {
            return false;
        }
        std::wregex re(L"[ ]*[\\.。．][ ]*dismiss.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);

    }

    void dismiss_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*dismiss[ ](.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring target = m[1];
            std::wstring self_id = std::to_wstring(cq::api::get_login_user_id());
            if (target.empty() || target == self_id || target == self_id.substr(self_id.length() - 4)
                || target == cq::utils::s2ws(cq::api::get_login_nickname())) {
                
	        if (e.message_type == cq::message::GROUP) {
                    if (!utils::is_admin_or_owner(e.target)) {
                        cq::api::send_msg(e.target, msg::GetGlobalMsg("strPermissionDeniedError"));
                        return;
                    }
                    cq::api::set_group_leave(*e.target.group_id, false);

                } else {
                    cq::api::set_discuss_leave(*e.target.discuss_id);
                }
            }
        }
    }
} // namespace dice
