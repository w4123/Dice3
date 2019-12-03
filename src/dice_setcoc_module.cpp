#include "dice_setcoc_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_utils.h"
#include "dice_msg_queue.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "dice_exception.h"
#include "dice_db.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool setcoc_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*setcoc.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void setcoc_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*setcoc[\\s]*([0-9]*).*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            int SuccessRule = 0;
            if (m[1].first != m[1].second) {
                SuccessRule = std::stoi(m[1]);
            }
            
			if (SuccessRule < 0 || SuccessRule > 5) {
                throw exception::exception(msg::GetGlobalMsg("strSuccessRuleErr"));
            }

            if (e.message_type == cq::message::PRIVATE) {
                SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, success_rule) VAlUES(?, ?)");
                st.bind(1, *e.target.user_id);
                st.bind(2, SuccessRule);
                st.exec();
            } else {
                if (!utils::is_admin_or_owner(e.target)) {
                    throw exception::exception(msg::GetGlobalMsg("strPermissionDeniedError"));
                }
                SQLite::Statement st(*db::db, "REPLACE INTO group_info(group_id, type, success_rule) VALUES(?, ?, ?)");
                if (e.message_type == cq::message::GROUP) {
                    st.bind(1, *e.target.group_id);
                    st.bind(2, 0);
                } else {
                    st.bind(1, *e.target.discuss_id);
                    st.bind(2, 1);
                }
                st.bind(3, SuccessRule);
                st.exec();
            }

            dice::msg_queue::MsgQueue.add(
                e.target,
                utils::format_string(msg::GetGlobalMsg("strSuccessRule"),
                                     {{"origin", utils::get_originname(e.target)},
                                     {"rule", std::to_string(SuccessRule)}}));
        }
    }
} // namespace dice
