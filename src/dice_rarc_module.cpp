#include "dice_rarc_module.h"
#include <regex>
#include <string>
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"
#include "dice_msg_queue.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool rarc_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*r(a|c)[^]*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void rarc_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(
            L"[\\s]*[\\.。．][\\s]*r(a|c)(h)?[\\s]*((?:b|p)(?:[1-3])?)?[\\s]*(?:(.*?)(?:--|—))?([^\\s0-9]*)[\\s]*([0-9]*)[\\s]*([^]*)",
            std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            // 获取骰子
            std::wstring dice(m[3]);
            if (dice.empty()) dice = L"D100";
			// 原因
            std::wstring reason(m[5]);
            if (m[7].first != m[7].second) {
                if (reason.empty())
                    reason = m[7];
                else
                    reason += L"(" + m[7].str() + L")";
            }
			// 人物卡
			std::string character_card(cq::utils::ws2s(m[4]));
			if (character_card.empty()) {
				character_card = utils::get_chosen_card(e.target);
			}
			else {
				reason = m[4].str() + L"--" + reason;
			}
            // 投掷属性
            std::string property(cq::utils::ws2s(m[5]));

            int judge_value;
            if (m[6].first != m[6].second) {
                judge_value = std::stoi(m[6]);
				if (property.empty() && !m[4].str().empty()) {
					throw exception::exception(msg::GetGlobalMsg("strPropertyInvalidError"));
				}
            } else {
                if (!property.empty()) {
                    judge_value =
                        utils::get_single_card_properties(e.target, character_card, property);
                } else {
                    throw exception::exception(msg::GetGlobalMsg("strPropertyInvalidError"));
                }
            }
            if (judge_value < 0) {
                throw exception::exception(msg::GetGlobalMsg("strPropertyInvalidError"));
            }
            dice_calculator cal(dice);

            std::wstring res;
            res = cal.form_string();

            if (m[2].first == m[2].second) {
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(
                        msg::GetGlobalMsg("strRollDiceWithJudge"),
                        std::map<std::string, std::string>{
                            {"nick", utils::get_nickname(e.target)},
                            {"reason", cq::utils::ws2s(reason)},
                            {"dice_expression", cq::utils::ws2s(res)},
                            {"judge_value", std::to_string(judge_value)},
                            {"success_indicator", utils::get_success_indicator(e.target, static_cast<int>(cal.result), judge_value)}}));
            } else {
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strHiddenDice"),
                                         std::map<std::string, std::string>{{"nick", utils::get_nickname(e.target)}}));
                dice::msg_queue::MsgQueue.add(
                    cq::Target(*e.target.user_id),
                    utils::format_string(
                        msg::GetGlobalMsg("strRollHiddenDiceWithJudge"),
                        std::map<std::string, std::string>{
                            {"origin", utils::get_originname(e.target)},
                            {"nick", utils::get_nickname(e.target)},
                            {"reason", cq::utils::ws2s(reason)},
                            {"dice_expression", cq::utils::ws2s(res)},
                            {"judge_value", std::to_string(judge_value)},
                            {"success_indicator",
                             utils::get_success_indicator(e.target, static_cast<int>(cal.result), judge_value)}}));
            }
        }
    }
} // namespace dice
