#include "dice_set_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_db.h"
#include "dice_exception.h"
#include "dice_utils.h"
#include "dice_msg_queue.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool set_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*set.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void set_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*set[\\s]*([0-9]*).*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            int SetDice = 100;
            if (m[1].first != m[1].second) {
                SetDice = std::stoi(m[1]);
            }
            if (SetDice <= 0) {
                throw exception::exception(msg::GetGlobalMsg("strDefaultDiceErr"));
            }
            if (e.message_type == cq::message::PRIVATE) {
                SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, default_dice) VAlUES(?, ?)");
                st.bind(1, *e.target.user_id);
                st.bind(2, SetDice);
                st.exec();
            } else {
                if (!utils::is_admin_or_owner(e.target)) {
                    throw exception::exception(msg::GetGlobalMsg("strPermissionDeniedError"));
                }
                SQLite::Statement st(*db::db, "REPLACE INTO group_info(group_id, type, default_dice) VAlUES(?, ?, ?)");
                if (e.message_type == cq::message::GROUP) {
                    st.bind(1, *e.target.group_id);
                    st.bind(2, 0);
                } else {
                    st.bind(1, *e.target.discuss_id);
                    st.bind(2, 1);
                }
                st.bind(3, SetDice);
                st.exec();
            }
            dice::msg_queue::MsgQueue.add(
                e.target,
                utils::format_string(msg::GetGlobalMsg("strDefaultDice"),
                                     {{"origin", utils::get_originname(e.target)},
                                      {"dice", "D" + std::to_string(SetDice)}}));
        }
    }
} // namespace dice
