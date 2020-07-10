#include "dice_rf_module.h"
#include <regex>
#include <string>
#include "cqsdk/cqsdk.h"
#include "fudge_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"
#include "dice_msg_queue.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool rf_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*rf[^]*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void rf_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*rf(h)?[\\s]*(([0-9]+)#)?([0-9]+)?(?:d)?(?:f)?((?:\\+|\\-)[0-9]+)?[\\s]*([^]*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring res;
            int RollCount = 1;
            if (m[2].first != m[2].second) {
                RollCount = std::stoi(m[3]);
            }

            if (RollCount <= 0 || RollCount > 10) {
                throw exception::dice_expression_invalid_error();
            }

            std::wstring dice_count(m[4]);
			if (dice_count.empty())
			{
				dice_count = L"4";
			}
			std::wstring add_value(m[5]);
			if (add_value.empty())
			{
				add_value = L"0";
			}
			
            std::wstring reason(m[6]);

            res = fudge_calculator(dice_count, add_value).form_string();

            for (int i = 1; i != RollCount; i++) {
                res += '\n';
                res += fudge_calculator(dice_count, add_value).form_string();
            }

            if (m[1].first == m[1].second) {
                dice::msg_queue::MsgQueue.add(e.target,
                                  utils::format_string(
                                      msg::GetGlobalMsg("strRollDice"),
                                      std::map<std::string, std::string>{
                                          {"nick", utils::get_nickname(e.target)},
                                          {"reason", cq::utils::ws2s(reason)},
                                          {"dice_expression", cq::utils::ws2s(res)}
                                      }));
            } else {
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strHiddenDice"),
                                         std::map<std::string, std::string>{{"nick", utils::get_nickname(e.target)}}));
                
                dice::msg_queue::MsgQueue.add(cq::Target(*e.target.user_id),
                                          utils::format_string(msg::GetGlobalMsg("strRollHiddenDice"),
                                                               std::map<std::string, std::string>{
                                                                   {"origin", utils::get_originname(e.target)},
                                                                   {"nick", utils::get_nickname(e.target)},
                                                                   {"reason", cq::utils::ws2s(reason)},
                                                                   {"dice_expression", cq::utils::ws2s(res)}
                                                               }));
            }
        }
    }
} // namespace dice
