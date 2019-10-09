#include "dice_set_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"
#include "dice_db.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool set_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*set.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void set_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*set[ ]*([0-9]*).*",
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
            SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, default_dice) VAlUES(?, ?)");
            st.bind(1, *e.target.user_id);
            st.bind(2, SetDice);
            st.exec();
            cq::api::send_msg(
                e.target,
                utils::format_string(msg::GetGlobalMsg("strDefaultDice"),
									{
										{"nick", utils::get_nickname(e.target)}, 
										{"dice", "D" + std::to_string(SetDice)}
									}
            ));
        }
    }
} // namespace dice
