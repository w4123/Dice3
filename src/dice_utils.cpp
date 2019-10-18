#include "dice_utils.h"
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "cqsdk/types.h"
#include "dice_calculator.h"
#include "dice_db.h"
#include "dice_exception.h"
#include "dice_msg.h"
#include "random"

namespace dice::utils {
    bool if_card_exist(const cq::Target &target, const std::string &card_name) {
        SQLite::Statement st(*db::db, "SELECT count(*) FROM character_cards WHERE qq_id = ? AND card_name = ?");
        st.bind(1, *target.user_id);
        st.bind(2, card_name);
        if (st.executeStep()) {
            return st.getColumn(0).getInt() > 0;
        }
        return false;
    }
    void set_chosen_card(const cq::Target &target, const std::string &card_name) {
        if (target.group_id.has_value()) {
            SQLite::Statement st(
                *db::db, "REPLACE INTO group_user_info(group_id, type, qq_id, card_chosen) VALUES (?, ?, ?, ?)");
            st.bind(1, *target.group_id);
            st.bind(2, 0);
            st.bind(3, *target.user_id);
            st.bind(4, card_name);
            st.exec();
        } else if (target.discuss_id.has_value()) {
            SQLite::Statement st(
                *db::db, "REPLACE INTO group_user_info(group_id, type, qq_id, card_chosen) VALUES (?, ?, ?, ?)");
            st.bind(1, *target.discuss_id);
            st.bind(2, 1);
            st.bind(3, *target.user_id);
            st.bind(4, card_name);
            st.exec();
        } else {
            SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, card_chosen) VALUES (?, ?)");
            st.bind(1, *target.user_id);
            st.bind(2, card_name);
            st.exec();
        }
    }
    std::string get_chosen_card(const cq::Target &target) {
        if (target.group_id.has_value()) {
            SQLite::Statement st(
                *db::db, "SELECT card_chosen FROM group_user_info WHERE group_id = ? AND type = ? AND qq_id = ?");
            st.bind(1, *target.group_id);
            st.bind(2, 0);
            st.bind(3, *target.user_id);
            if (st.executeStep()) {
                return st.getColumn(0).getString();
            }
            return "default";
        }
        if (target.discuss_id.has_value()) {
            SQLite::Statement st(
                *db::db, "SELECT card_chosen FROM group_user_info WHERE group_id = ? AND type = ? AND qq_id = ?");
            st.bind(1, *target.discuss_id);
            st.bind(2, 1);
            st.bind(3, *target.user_id);
            if (st.executeStep()) {
                return st.getColumn(0).getString();
            }
            return "default";
        }
        SQLite::Statement st(*db::db, "SELECT card_chosen FROM qq_info WHERE qq_id = ?");
        st.bind(1, *target.user_id);
        if (st.executeStep()) {
            return st.getColumn(0).getString();
        }
        return "default";
    }
    std::map<std::string, int> get_card_properties(const cq::Target &target, const std::string &character_card_name,
                                                   const std::set<std::string> &st_character_properties) {
        std::map<std::string, int> ret;
        SQLite::Transaction tran(*db::db);
        for (const auto &it : st_character_properties) {
            SQLite::Statement st(
                *db::db, "SELECT value FROM character_cards WHERE qq_id = ? AND card_name = ? AND property = ?");
            st.bind(1, *target.user_id);
            st.bind(2, character_card_name);
            st.bind(3, it);
            if (st.executeStep()) {
                ret[it] = st.getColumn(0).getInt();
            } else if (msg::SkillDefaultVal.count(it)) {
                ret[it] = msg::SkillDefaultVal.at(it);
            } else {
                throw exception::exception(
                    utils::format_string(msg::GetGlobalMsg("strPropertyNotFoundError"), {{"property_name", it}}));
            }
        }
        return ret;
    }

    std::string get_card_properties_string(const cq::Target &target, const std::string &character_card_name,
                                           const std::set<std::string> &st_character_properties) {
        std::string ret;
        std::map<std::string, int> card_properties =
            get_card_properties(target, character_card_name, st_character_properties);
        std::string fmt = msg::GetGlobalMsg("strStProperty");
        int count = 1;
        for (const auto &it : card_properties) {
            ret += format_string(fmt, {{"property", it.first}, {"value", std::to_string(it.second)}});
            if (count % 3) {
                ret += "  ";
            } else {
                ret += "\n";
            }
            ++count;
        }
        ret.erase(ret.end() - 1);
        return ret;
    }
    void delete_character_properties(const cq::Target &target, const std::string &character_card_name,
                                     const std::set<std::string> &st_character_properties) {
        SQLite::Transaction tran(*db::db);
        for (const auto &it : st_character_properties) {
            SQLite::Statement st(*db::db,
                                 "DELETE FROM character_cards WHERE qq_id = ? AND card_name = ? AND property = ?");
            st.bind(1, *target.user_id);
            st.bind(2, character_card_name);
            st.bind(3, it);
            st.exec();
        }
        tran.commit();
    }
    void delete_character_card(const cq::Target &target, const std::string &character_card_name) {
        SQLite::Statement st(*db::db, "DELETE FROM character_cards WHERE qq_id = ? AND card_name = ?");
        st.bind(1, *target.user_id);
        st.bind(2, character_card_name);
        st.exec();
    }

    std::set<std::string> get_all_card_name(const cq::Target &target) {
        std::set<std::string> ret;
        SQLite::Transaction tran(*db::db);
        SQLite::Statement st(*db::db, "SELECT card_name FROM character_cards WHERE qq_id = ?");
        st.bind(1, *target.user_id);
        while (st.executeStep()) {
            if (!ret.count(st.getColumn(0).getString())) {
                ret.insert(st.getColumn(0).getString());
            }
        }
        tran.commit();
        if (ret.empty()) {
            throw exception::exception(msg::GetGlobalMsg("strDatabaseNotFoundError"));
        }
        return ret;
    }

    std::string get_all_card_name_string(const cq::Target &target) {
        std::set<std::string> card_name = get_all_card_name(target);
        std::string ret;
        for (const auto &it : card_name) {
            ret += it;
            ret += "\n";
        }
        ret.erase(ret.end() - 1);
        return ret;
    }

    std::map<std::string, int> get_all_card_properties(const cq::Target &target,
                                                       const std::string &character_card_name) {
        std::map<std::string, int> ret;
        SQLite::Transaction tran(*db::db);
        SQLite::Statement st(*db::db, "SELECT property, value FROM character_cards WHERE qq_id = ? AND card_name = ?");
        st.bind(1, *target.user_id);
        st.bind(2, character_card_name);
        while (st.executeStep()) {
            ret[st.getColumn(0).getString()] = st.getColumn(1).getInt();
        }
        tran.commit();
        if (ret.empty()) {
            throw exception::exception(msg::GetGlobalMsg("strDatabaseNotFoundError"));
        }
        return ret;
    }
    std::string get_all_card_properties_string(const cq::Target &target, const std::string &character_card_name) {
        std::string ret;
        std::map<std::string, int> card_properties = get_all_card_properties(target, character_card_name);
        std::string fmt = msg::GetGlobalMsg("strStProperty");
        int count = 1;
        for (const auto &it : card_properties) {
            ret += format_string(fmt, {{"property", it.first}, {"value", std::to_string(it.second)}});
            if (count % 3) {
                ret += "  ";
            } else {
                ret += "\n";
            }
            ++count;
        }
        ret.erase(ret.end() - 1);
        return ret;
    }
    void set_character_card(const cq::Target &target, const std::string &character_card_name,
                            const std::map<std::string, int> &mp_character_card) {
        SQLite::Transaction tran(*db::db);
        for (const auto &it : mp_character_card) {
            SQLite::Statement st(*db::db,
                                 "REPLACE INTO character_cards(qq_id, card_name, property, value) VALUES (?, ?, ?, ?)");
            st.bind(1, *target.user_id);
            st.bind(2, character_card_name);
            st.bind(3, it.first);
            st.bind(4, it.second);
            st.exec();
        }
        tran.commit();
    }
    void set_jrrp_enabled(const cq::Target &target, const bool enabled) {
        if (target.group_id.has_value()) {
            set_jrrp_enabled(*target.group_id, 0, enabled);
        } else if (target.discuss_id.has_value()) {
            set_jrrp_enabled(*target.discuss_id, 1, enabled);
        } else {
            throw exception::exception(msg::GetGlobalMsg("strCommandNotAvailableError"));
        }
    }

    void set_jrrp_enabled(const int64_t group_id, const int type, const bool enabled) {
        SQLite::Statement st(*db::db, "REPLACE INTO group_info(group_id, type, jrrp_on) VALUES(?, ?, ?)");
        st.bind(1, group_id);
        st.bind(2, type);
        st.bind(3, enabled);
        st.exec();
    }

    bool is_jrrp_enabled(const int64_t group_id, const int type) {
        SQLite::Statement st(*db::db, "SELECT jrrp_on FROM group_info WHERE group_id=? and type=?");
        st.bind(1, group_id);
        st.bind(2, type);
        if (st.executeStep()) {
            return st.getColumn(0).getInt();
        }
        return true;
    }

    bool is_jrrp_enabled(const cq::Target &target) {
        if (target.group_id.has_value()) {
            return is_jrrp_enabled(*target.group_id, 0);
        }
        if (target.discuss_id.has_value()) {
            return is_jrrp_enabled(*target.discuss_id, 1);
        }
        return true;
    }

    void set_help_enabled(const cq::Target &target, const bool enabled) {
        if (target.group_id.has_value()) {
            set_help_enabled(*target.group_id, 0, enabled);
        } else if (target.discuss_id.has_value()) {
            set_help_enabled(*target.discuss_id, 1, enabled);
        } else {
            throw exception::exception(msg::GetGlobalMsg("strCommandNotAvailableError"));
        }
    }

    void set_help_enabled(const int64_t group_id, const int type, const bool enabled) {
        SQLite::Statement st(*db::db, "REPLACE INTO group_info(group_id, type, help_on) VALUES(?, ?, ?)");
        st.bind(1, group_id);
        st.bind(2, type);
        st.bind(3, enabled);
        st.exec();
    }

    bool is_help_enabled(const int64_t group_id, const int type) {
        SQLite::Statement st(*db::db, "SELECT help_on FROM group_info WHERE group_id=? and type=?");
        st.bind(1, group_id);
        st.bind(2, type);
        if (st.executeStep()) {
            return st.getColumn(0).getInt();
        }
        return true;
    }

    bool is_help_enabled(const cq::Target &target) {
        if (target.group_id.has_value()) {
            return is_help_enabled(*target.group_id, 0);
        }
        if (target.discuss_id.has_value()) {
            return is_help_enabled(*target.discuss_id, 1);
        }
        return true;
    }

    std::string get_date() {
        time_t raw_time;
        time(&raw_time);
        tm time_info{};
        localtime_s(&time_info, &raw_time);
        std::stringstream time_format;
        time_format << std::put_time(&time_info, "%F");
        return time_format.str();
    }

    void set_jrrp(const cq::Target &target, int jrrp_val) {
        SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, jrrp_value, jrrp_date) VALUES (?, ?, ?)");
        st.bind(1, *target.user_id);
        st.bind(2, jrrp_val);
        st.bind(3, get_date());
        st.exec();
    }

    std::tuple<bool, int> get_jrrp(const cq::Target &target) {
        SQLite::Statement st(*db::db, "SELECT jrrp_value, jrrp_date FROM qq_info WHERE qq_id = ?");
        st.bind(1, *target.user_id);
        if (st.executeStep()) {
            if (st.getColumn(0).isNull() || st.getColumn(1).isNull()) return std::make_tuple(false, 0);

            if (get_date() == st.getColumn(1).getString()) {
                return std::make_tuple(true, st.getColumn(0).getInt());
            }
            return std::make_tuple(false, 0);
        }
        return std::make_tuple(false, 0);
    }

    int get_defaultdice(const cq::Target &target) {
        if (target.group_id.has_value()) {
            SQLite::Statement st(*db::db, "SELECT default_dice FROM group_info WHERE group_id = ? AND type = ?");
            st.bind(1, *target.group_id);
            st.bind(2, 0);
            if (st.executeStep()) {
                return st.getColumn(0).getInt();
            }
            return 100;
        }
        if (target.discuss_id.has_value()) {
            SQLite::Statement st(*db::db, "SELECT default_dice FROM group_info WHERE group_id = ? AND type = ?");
            st.bind(1, *target.discuss_id);
            st.bind(2, 1);
            if (st.executeStep()) {
                return st.getColumn(0).getInt();
            }
            return 100;
        }
        SQLite::Statement st(*db::db, "SELECT default_dice FROM qq_info WHERE qq_id = ?");
        st.bind(1, *target.user_id);
        if (st.executeStep()) {
            return st.getColumn(0).getInt();
        }
        return 100;
    }

    std::string get_groupname(const int64_t group_id) {
        auto list = cq::api::get_group_list();
        for (auto &ele : list) {
            if (ele.group_id == group_id) {
                return ele.group_name;
            }
        }
        return msg::GetGlobalMsg("strGroupnameError");
    }

    std::string get_originname(const cq::Target &target) {
        if (target.group_id.has_value()) {
            return "群\"" + get_groupname(*target.group_id) + "\"";
        }
        if (target.discuss_id.has_value()) {
            return "讨论组";
        }
        return "私聊会话";
    }

    // 昵称获取 群/讨论组 type=0代表群， type=1代表讨论组
    std::string get_nickname(const int64_t group_id, const int64_t user_id, const int type, const bool only_from_db) {
        SQLite::Statement st(*db::db, "SELECT nick_name FROM group_user_info WHERE group_id=? AND qq_id=? AND type=?");
        st.bind(1, group_id);
        st.bind(2, user_id);
        st.bind(3, type);
        if (st.executeStep()) {
            if (!st.getColumn(0).isNull() && !st.getColumn(0).getString().empty()) {
                return st.getColumn(0).getString();
            }
        }
        SQLite::Statement st1(*db::db, "SELECT nick_name FROM qq_info WHERE qq_id=?");
        st1.bind(1, user_id);
        if (st1.executeStep()) {
            if (!st1.getColumn(0).isNull() && !st1.getColumn(0).getString().empty()) {
                return st1.getColumn(0).getString();
            }
        }
        if (only_from_db) {
            return "";
        }
        if (type == 0) {
            const cq::GroupMember gm = cq::api::get_group_member_info(group_id, user_id);
            if (!gm.card.empty()) {
                return gm.card;
            }
            return gm.nickname;
        }
        return cq::api::get_stranger_info(user_id).nickname;
    }

    // 昵称获取 私聊
    std::string get_nickname(const int64_t user_id, const bool only_from_db) {
        SQLite::Statement st(*db::db, "SELECT nick_name FROM qq_info WHERE qq_id=?");
        st.bind(1, user_id);
        if (st.executeStep()) {
            if (!st.getColumn(0).isNull() && !st.getColumn(0).getString().empty()) {
                return st.getColumn(0).getString();
            }
        }
        if (only_from_db) {
            return "";
        }
        return cq::api::get_stranger_info(user_id).nickname;
    }

    // 昵称获取 综合
    std::string get_nickname(const cq::Target &target, const bool only_from_db) {
        if (target.user_id.has_value()) {
            if (target.group_id.has_value()) {
                return get_nickname(*target.group_id, *target.user_id, 0, only_from_db);
            }
            if (target.discuss_id.has_value()) {
                return get_nickname(*target.discuss_id, *target.user_id, 1, only_from_db);
            }
            return get_nickname(*target.user_id, only_from_db);
        }
        return msg::GetGlobalMsg("strNicknameError");
    }

    // 设置群昵称
    void set_group_nickname(const int64_t group_id, const int64_t user_id, const int type,
                            const std::string &nick_name) {
        SQLite::Statement st(*db::db,
                             "REPLACE INTO group_user_info(group_id, qq_id, type, nick_name) VALUES(?, ?, ?, ?)");
        st.bind(1, group_id);
        st.bind(2, user_id);
        st.bind(3, type);
        st.bind(4, nick_name);
        st.exec();
    }

    // 设置群昵称
    void set_group_nickname(const cq::Target &target, const std::string &nick_name) {
        if (target.user_id.has_value()) {
            if (target.group_id.has_value()) {
                set_group_nickname(*target.group_id, *target.user_id, 0, nick_name);
            } else if (target.discuss_id.has_value()) {
                set_group_nickname(*target.discuss_id, *target.user_id, 1, nick_name);
            } else {
                throw exception::exception(msg::GetGlobalMsg("strCommandNotAvailableError"));
            }

        } else {
            throw exception::exception(msg::GetGlobalMsg("strSetNicknameError"));
        }
    }

    // 设置全局昵称
    void set_global_nickname(const int64_t user_id, const std::string &nick_name) {
        SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, nick_name) VALUES(?, ?)");
        st.bind(1, user_id);
        st.bind(2, nick_name);
        st.exec();
    }

    // 设置全局昵称
    void set_global_nickname(const cq::Target &target, const std::string &nick_name) {
        if (target.user_id.has_value()) {
            set_global_nickname(*target.user_id, nick_name);
        } else {
            throw exception::exception(msg::GetGlobalMsg("strSetNicknameError"));
        }
    }

    // 格式化字符串
    std::string format_string(const std::string &origin_str, const std::map<std::string, std::string> &format_para) {
        std::string old_str = origin_str;
        bool need_format = true;
        std::map<std::string, std::vector<int>> mp;
        while (need_format) {
            need_format = false;
            std::string new_str;
            std::regex re("\\{([^\\{\\}]+)\\}");
            auto words_begin = std::sregex_iterator(old_str.begin(), old_str.end(), re);
            auto words_end = std::sregex_iterator();
            size_t last = 0;
            for (auto it = words_begin; it != words_end; ++it) {
                need_format = true;
                std::string format_str(std::string((*it)[1].first, (*it)[1].second));
                new_str += old_str.substr(last, it->position() - last);
                if (format_str[0] == '!') {
                    std::string dice = format_str.substr(1);
                    if (dice.empty()) dice = "d";
                    new_str += cq::utils::ws2s(dice_calculator(cq::utils::s2ws(dice)).form_string());
                } else if (format_str[0] == '?') {
                    std::string dice = format_str.substr(1);
                    if (dice.empty()) dice = "d";
                    std::string res = std::to_string(dice_calculator(cq::utils::s2ws(dice)).result);
                    res = res.substr(
                        0, std::max(res.find('.'), std::min(res.find('.') + 3, res.find_last_not_of("0.") + 1)));
                    new_str += res;
                } else if (format_str[0] == '%') {
                    std::string deck_name = format_str.substr(1);
                    SQLite::Statement st(*db::db,
                                         "SELECT content FROM deck WHERE origin=\"public\" AND name=? LIMIT 1 OFFSET "
                                         "ABS(RANDOM()) % MAX((SELECT "
                                         "COUNT(*) FROM deck WHERE origin=\"public\" AND name=?), 1)");
                    st.bind(1, deck_name);
                    st.bind(2, deck_name);
                    if (st.executeStep()) {
                        new_str += st.getColumn(0).getText();
                    } else {
                        throw exception::exception(msg::GetGlobalMsg("strDatabaseNotFoundError"));
                    }
                } else if (format_str[0] == '#') {
                    std::string choose_str = format_str.substr(1);
                    int count = 1;
                    std::regex re_count("([^:]*):([0-9]*)");
                    std::smatch re_count_match;
                    if (std::regex_match(choose_str, re_count_match, re_count)) {
                        if (re_count_match[2].first != re_count_match[2].second) count = std::stoi(re_count_match[2]);
                        choose_str = re_count_match[1];
                    }
                    if (count == 0) {
                        throw exception::exception(msg::GetGlobalMsg("strFormatStrInvalidError"));
                    }
                    std::regex re("[^|]+");
                    std::vector<std::string> choose_vec;
                    auto word_begin = std::sregex_iterator(choose_str.begin(), choose_str.end(), re);
                    auto word_end = std::sregex_iterator();
                    for (auto ii = word_begin; ii != word_end; ii++) {
                        choose_vec.push_back((*ii)[0]);
                    }
                    if (choose_vec.empty()) {
                        throw exception::exception(msg::GetGlobalMsg("strFormatStrInvalidError"));
                    }
                    auto choose_vec_copy = choose_vec;
                    for (int counter = 0; counter != count; counter++) {
                        int index = std::uniform_int_distribution<int>(0, choose_vec.size() - 1)(dice_calculator::ran);
                        new_str += choose_vec[index];
                        choose_vec.erase(choose_vec.begin() + index);
                        if (counter != count - 1) new_str += " ";
                        if (choose_vec.empty()) choose_vec = choose_vec_copy;
                    }
                } else if (format_str[0] == '@') {
                    int count = 0;
                    std::string deck_name = format_str.substr(1);
                    SQLite::Statement st(*db::db, "SELECT COUNT(*) FROM deck WHERE origin=\"public\" AND name=?");
                    st.bind(1, deck_name);
                    if (st.executeStep()) {
                        count = st.getColumn(0).getInt();
                    }
                    if (count == 0) {
                        throw exception::exception(msg::GetGlobalMsg("strDatabaseNotFoundError"));
                    }
                    int offset_num = 0;
                    if (mp.count(deck_name)) {
                        if (count <= static_cast<int>(mp[deck_name].size())) mp[deck_name].clear();
                        count -= mp[deck_name].size();
                    } else {
                        mp[deck_name] = std::vector<int>();
                    }
                    offset_num = std::uniform_int_distribution<int>(0, count - 1)(dice_calculator::ran);
                    std::sort(mp[deck_name].begin(), mp[deck_name].end());
                    for (int ele : mp[deck_name]) {
                        if (offset_num >= ele) offset_num++;
                    }

                    SQLite::Statement st1(
                        *db::db, "SELECT content FROM deck WHERE origin=\"public\" AND name=? LIMIT 1 OFFSET ?");

                    st1.bind(1, deck_name);
                    st1.bind(2, offset_num);
                    if (st1.executeStep()) {
                        new_str += st1.getColumn(0).getText();
                    }
                    mp[deck_name].push_back(offset_num);
                } else if (format_para.count(format_str)) {
                    new_str += format_para.at(std::string((*it)[1].first, (*it)[1].second));
                }
                last = it->position() + it->length();
            }
            new_str += old_str.substr(last);
            old_str = new_str;
        }
        return old_str;
    }

    // 获取权限 群
    bool is_admin_or_owner(const int64_t group_id, const int64_t user_id) {
        auto role = cq::api::get_group_member_info(group_id, user_id).role;
        return (role == cq::GroupRole::ADMIN || role == cq::GroupRole::OWNER);
    }

    // 获取权限 综合 (非群返回永远为真)
    bool is_admin_or_owner(const cq::Target &target) {
        if (target.group_id.has_value() && target.user_id.has_value()) {
            return is_admin_or_owner(*target.group_id, *target.user_id);
        }
        return true;
    }

} // namespace dice::utils
