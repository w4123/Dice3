#include "dice_r_module.h"
#include <regex>
#include <string>
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool r_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。]r.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void r_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。]r[ ]*([0-9dk+\\-*/\\(\\)\\^]*)[ ]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        auto ret = std::regex_match(ws, m, re);
        std::wstring res;
        if (m[1].first != m[1].second) {
            res = dice_calculator(std::wstring(m[1].first, m[1].second)).form_string();
        } else {
            res = dice_calculator(std::wstring(L"d")).form_string();
        }

        cq::api::send_msg(
            e.target,
            utils::format_string(
                msg::global_msg["strRollDice"],
                std::map<std::string, std::string>{
                    {"nick", utils::get_nickname(e.target)},
                    {"reason", cq::utils::ws2s(std::wstring(m[2].first, m[2].second))},
                    {"dice_expression", cq::utils::ws2s(res)}
                }
	    )
        );

    }
} // namespace dice