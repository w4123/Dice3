#include "dice_st_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
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
            L"[\\s]*[\\.。．][\\s]*st[\\s]*((?:(del[\\s]*(?:(?:(.*?)-)?([^]*)))|(clr[\\s]*(.*)))|(?:(show[\\s]*(?:(?:(."
            L"*?)-)?([^]*)))|((?:(.*?)-)?([^]*))))",
            std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            // 获取全部人物卡
            if (m[1].first == m[1].second) {
                cq::api::send_msg(e.target,
                                  utils::format_string(msg::GetGlobalMsg("strStList"),
                                                       {{"nick", utils::get_nickname(e.target)},
                                                        {"card_names", utils::get_all_card_name_string(e.target)}}));
                return;
            }
            // 删除属性
            if (m[2].first != m[2].second) {
                std::string character_card_name = cq::utils::ws2s(m[3]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_nickname(e.target, true);
                    if (character_card_name.empty()) character_card_name = "default";
                }

                std::string properties(cq::utils::ws2s(m[4]));
                std::set<std::string> st_properties;
                std::regex property_re("([^\\s\\|0-9]+)");
                auto word_begin = std::sregex_iterator(properties.begin(), properties.end(), property_re);
                auto word_end = std::sregex_iterator();
                for (auto word_it = word_begin; word_it != word_end; word_it++) {
                    std::string property_name = (*word_it)[1];
                    std::transform(
                        property_name.begin(), property_name.end(), property_name.begin(), [](unsigned char c) {
                            return std::tolower(c);
                        });
                    if (msg::SkillNameReplace.count(property_name)) {
                        property_name = msg::SkillNameReplace.at(property_name);
                    }
                    if (!st_properties.count(property_name)) {
                        st_properties.insert(property_name);
                    }
                }
                utils::delete_character_properties(e.target, character_card_name, st_properties);
                cq::api::send_msg(e.target,
                                  utils::format_string(msg::GetGlobalMsg("strStDel"),
                                                       {{"card_name", character_card_name},
                                                        {"num_of_properties", std::to_string(st_properties.size())}}));
                return;
            }
            // 删除人物卡
            if (m[5].first != m[5].second) {
                std::string character_card_name = cq::utils::ws2s(m[6]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_nickname(e.target, true);
                    if (character_card_name.empty()) character_card_name = "default";
                }
                utils::delete_character_card(e.target, character_card_name);
                cq::api::send_msg(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strStClr"), {{"card_name", character_card_name}}));
                return;
            }
            // 展示人物卡
            if (m[7].first != m[7].second) {
                std::string character_card_name = cq::utils::ws2s(m[8]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_nickname(e.target, true);
                    if (character_card_name.empty()) character_card_name = "default";
                }
                std::string properties(cq::utils::ws2s(m[9]));
                std::transform(properties.begin(), properties.end(), properties.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                if (properties == "all") {
                    cq::api::send_msg(
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
                    cq::api::send_msg(
                        e.target,
                        utils::format_string(
                            msg::GetGlobalMsg("strStShow"),
                            {{"card_name", character_card_name},
                             {"card_properties",
                              utils::get_card_properties_string(e.target, character_card_name, st_properties)}}));
                }
                return;
            }
            // 人物卡设置部分
            if (m[10].first != m[10].second) {
                std::string character_card_name = cq::utils::ws2s(m[11]);
                if (character_card_name.empty()) {
                    character_card_name = utils::get_nickname(e.target, true);
                    if (character_card_name.empty()) character_card_name = "default";
                }

                std::string character_card(cq::utils::ws2s(m[12]));
                std::map<std::string, int> mp_character_card;
                std::regex card_re("([^\\s\\|0-9]+?)[:=\\s]?([0-9]+)");
                auto word_begin = std::sregex_iterator(character_card.begin(), character_card.end(), card_re);
                auto word_end = std::sregex_iterator();
                for (auto word_it = word_begin; word_it != word_end; word_it++) {
                    std::smatch card_match = *word_it;
                    std::string property_name = card_match[1];
                    std::transform(
                        property_name.begin(), property_name.end(), property_name.begin(), [](unsigned char c) {
                            return std::tolower(c);
                        });
                    if (msg::SkillNameReplace.count(property_name)) {
                        property_name = msg::SkillNameReplace.at(property_name);
                    }
                    mp_character_card[property_name] = std::stoi(card_match[2]);
                }
                utils::set_character_card(e.target, character_card_name, mp_character_card);
                cq::api::send_msg(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strStSet"),
                                         {{"card_name", character_card_name},
                                          {"num_of_properties", std::to_string(mp_character_card.size())}}));
            }
        }
    }
} // namespace dice
