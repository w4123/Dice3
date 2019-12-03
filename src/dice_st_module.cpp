#include "dice_st_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_msg_queue.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {

    bool st_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*st[^]*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void st_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(
            L"[\\s]*[\\.。．][\\s]*st[\\s]*((?:(del[\\s]*(?:(?:(.*?)(?:--|—))?([^]*)))|(clr[\\s]*(.*)))|(?:(show[\\s]*("
            L"?:(?:(."
            L"*?)(?:--|—))?([^]*)))|(?:(switch[\\s]*(.*))|((?:(.*?)(?:--|—))?([^]*)))))",
            std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            // 获取全部人物卡
            if (m[1].first == m[1].second) {
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strStList"),
                                         {{"nick", utils::get_nickname(e.target)},
                                          {"card_names", utils::get_all_card_name_string(e.target)}}));
                return;
            }
            // 删除属性
            if (m[2].first != m[2].second) {
                std::string character_card_name = cq::utils::ws2s(m[3]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_chosen_card(e.target);
                }

                std::string properties(cq::utils::ws2s(m[4]));
                std::transform(properties.begin(), properties.end(), properties.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                if (properties == "all") {
                    utils::delete_character_card(e.target, character_card_name);
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(msg::GetGlobalMsg("strStClr"), {{"card_name", character_card_name}}));
                } else {
                    std::set<std::string> st_properties;
                    std::regex property_re("([^\\s\\|0-9]+)");
                    auto word_begin = std::sregex_iterator(properties.begin(), properties.end(), property_re);
                    auto word_end = std::sregex_iterator();
                    for (auto word_it = word_begin; word_it != word_end; word_it++) {
                        std::string property_name = (*word_it)[1];
                        if (msg::SkillNameReplace.count(property_name)) {
                            property_name = msg::SkillNameReplace.at(property_name);
                        }
                        if (!st_properties.count(property_name)) {
                            st_properties.insert(property_name);
                        }
                    }
                    utils::delete_character_properties(e.target, character_card_name, st_properties);
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(msg::GetGlobalMsg("strStDel"),
                                             {{"card_name", character_card_name},
                                              {"num_of_properties", std::to_string(st_properties.size())}}));
				}
                return;
            }
            // 删除人物卡
            if (m[5].first != m[5].second) {
                std::string character_card_name = cq::utils::ws2s(m[6]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_chosen_card(e.target);
                }
                utils::delete_character_card(e.target, character_card_name);
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strStClr"), {{"card_name", character_card_name}}));
                return;
            }
            // 展示人物卡
            if (m[7].first != m[7].second) {
                std::string character_card_name = cq::utils::ws2s(m[8]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_chosen_card(e.target);
                }
                std::string properties(cq::utils::ws2s(m[9]));
                std::transform(properties.begin(), properties.end(), properties.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                if (properties == "all") {
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(msg::GetGlobalMsg("strStShow"),
                                             {{"card_name", character_card_name},
                                              {"card_properties",
                                               utils::get_all_card_properties_string(e.target, character_card_name)}}));
                } else {
                    std::set<std::string> st_properties;
                    std::regex property_re("([^\\s\\|0-9]+)");
                    auto word_begin = std::sregex_iterator(properties.begin(), properties.end(), property_re);
                    auto word_end = std::sregex_iterator();
                    for (auto word_it = word_begin; word_it != word_end; word_it++) {
                        std::string property_name = (*word_it)[1];

                        if (msg::SkillNameReplace.count(property_name)) {
                            property_name = msg::SkillNameReplace.at(property_name);
                        }
                        if (!st_properties.count(property_name)) {
                            st_properties.insert(property_name);
                        }
                    }
                    if (st_properties.empty()) {
                        throw exception::exception(msg::GetGlobalMsg("strStShowEmptyError"));
                    }
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(
                            msg::GetGlobalMsg("strStShow"),
                            {{"card_name", character_card_name},
                             {"card_properties",
                              utils::get_card_properties_string(e.target, character_card_name, st_properties)}}));
                }
                return;
            }
            // 切换人物卡
            if (m[10].first != m[10].second) {
                std::string character_card_name = cq::utils::ws2s(m[11]);
                if (character_card_name.empty()) {
                    character_card_name = "default";
                }
                utils::set_chosen_card(e.target, character_card_name);
                dice::msg_queue::MsgQueue.add(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strStSwitch"), {{"card_name", character_card_name}}));
                return;
            }
            // 人物卡设置部分
            if (m[12].first != m[12].second) {
                std::string character_card_name = cq::utils::ws2s(m[13]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_chosen_card(e.target);
                }

                std::string character_card(cq::utils::ws2s(m[14]));
                std::transform(
                    character_card.begin(), character_card.end(), character_card.begin(), [](unsigned char c) {
                        return std::tolower(c);
                    });
                std::map<std::string, int> mp_character_card;
                std::map<std::string, std::string> mp_character_card_change;
                std::set<std::string> st_character_card_change;
                std::regex card_re(
                    "([^\\s\\|0-9\\+\\-]+?)[:=\\s]?([0-9]+)|([^\\s\\|0-9]+?)([\\+\\-][0-9d\\+\\-\\(\\)]+)");
                auto word_begin = std::sregex_iterator(character_card.begin(), character_card.end(), card_re);
                auto word_end = std::sregex_iterator();
                for (auto word_it = word_begin; word_it != word_end; word_it++) {
                    std::smatch card_match = *word_it;
                    if (card_match[1].first != card_match[1].second) {
                        std::string property_name = card_match[1];

                        if (msg::SkillNameReplace.count(property_name)) {
                            property_name = msg::SkillNameReplace.at(property_name);
                        }
                        if (st_character_card_change.count(property_name)) {
                            throw dice::exception::exception(msg::GetGlobalMsg("strPropertyRepeatError"));
                        }
                        mp_character_card[property_name] = std::stoi(card_match[2]);
                    } else {
                        std::string property_name = card_match[3];
                        if (msg::SkillNameReplace.count(property_name)) {
                            property_name = msg::SkillNameReplace.at(property_name);
                        }
                        if (st_character_card_change.count(property_name) || mp_character_card.count(property_name)) {
                            throw dice::exception::exception(msg::GetGlobalMsg("strPropertyRepeatError"));
                        }
                        st_character_card_change.insert(property_name);
                        mp_character_card_change[property_name] = card_match[4];
                    }
                }
                auto old_value = utils::get_card_properties(e.target, character_card_name, st_character_card_change);
                std::string change_str;
                for (const auto &it_change : mp_character_card_change) {
                    if (it_change.second.find_first_of("d()+-", 1) == std::string::npos) {
                        int new_value = old_value[it_change.first] + std::stoi(it_change.second);
                        mp_character_card[it_change.first] = new_value;
                        change_str += utils::format_string(msg::GetGlobalMsg("strStPropertyChange"),
                                                           {{"property", it_change.first},
                                                            {"change", it_change.second},
                                                            {"value_old", std::to_string(old_value[it_change.first])},
                                                            {"value_new", std::to_string(new_value)}});
                        change_str += "\n";
                    } else {
                        std::string dice = it_change.second;
                        if (!dice.empty() && dice[0] == '+') dice = dice.substr(1);
                        dice_calculator cal(cq::utils::s2ws(dice), utils::get_defaultdice(e.target));
                        int new_value = old_value[it_change.first] + static_cast<int>(cal.result);
                        mp_character_card[it_change.first] = new_value;
                        change_str += utils::format_string(
                            msg::GetGlobalMsg("strStPropertyChange"),
                            {{"property", it_change.first},
                             {"change", it_change.second + "=" + std::to_string(static_cast<int>(cal.result))},
                             {"value_old", std::to_string(old_value[it_change.first])},
                             {"value_new", std::to_string(new_value)}});
                        change_str += "\n";
                    }
                }
                utils::set_character_card(e.target, character_card_name, mp_character_card);
                if (st_character_card_change.empty()) {
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(msg::GetGlobalMsg("strStSet"),
                                             {{"card_name", character_card_name},
                                              {"num_of_properties", std::to_string(mp_character_card.size())}}));
                } else {
                    change_str.erase(change_str.end() - 1);
                    dice::msg_queue::MsgQueue.add(
                        e.target,
                        utils::format_string(msg::GetGlobalMsg("strStSetWithChange"),
                                             {{"card_name", character_card_name},
                                              {"num_of_properties", std::to_string(mp_character_card.size())},
                                              {"change_str", change_str}}));
                }
            }
        }
    }
} // namespace dice
