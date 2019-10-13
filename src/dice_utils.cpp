#include "dice_utils.h"
#include <map>
#include <regex>
#include <string>
#include <tuple>
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "cqsdk/types.h"
#include "dice_db.h"
#include "dice_exception.h"
#include "dice_msg.h"

namespace dice::utils {

    std::string get_date() {
        time_t raw_time;
        time(&raw_time);
        tm time_info{};
        localtime_s(&time_info, &raw_time);
        std::stringstream time_format;
        time_format << std::put_time(&time_info, "%F");
        return time_format.str();
    }

    void set_jrrp(const cq::Target& target, int jrrp_val) {
        SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, jrrp_value, jrrp_date) VALUES (?, ?, ?)");
        st.bind(1, *target.user_id);
        st.bind(2, jrrp_val);
        st.bind(3, get_date());
        st.exec();
    }

    std::tuple<bool, int> get_jrrp(const cq::Target& target) {
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

    int get_defaultdice(const cq::Target& target) {
        SQLite::Statement st(*db::db, "SELECT default_dice FROM qq_info WHERE qq_id = ?");
        st.bind(1, *target.user_id);
        if (st.executeStep()) {
            return st.getColumn(0).getInt();
        }
        return 100;
    }

    std::string get_groupname(const int64_t group_id) {
        auto list = cq::api::get_group_list();
        for (auto& ele : list) {
            if (ele.group_id == group_id) {
                return ele.group_name;
            }
        }
        return msg::GetGlobalMsg("strGroupnameError");
    }

    std::string get_originname(const cq::Target& target) {
        if (target.group_id.has_value()) {
            return "群\"" + get_groupname(*target.group_id) + "\"";
        }
        if (target.discuss_id.has_value()) {
            return "讨论组";
        }
        return "私聊会话";
    }

    // 昵称获取 群/讨论组 type=0代表群， type=1代表讨论组
    std::string get_nickname(const int64_t group_id, const int64_t user_id, const int type) {
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
    std::string get_nickname(const int64_t user_id) {
        SQLite::Statement st(*db::db, "SELECT nick_name FROM qq_info WHERE qq_id=?");
        st.bind(1, user_id);
        if (st.executeStep()) {
            if (!st.getColumn(0).isNull() && !st.getColumn(0).getString().empty()) {
                return st.getColumn(0).getString();
            }
        }
        return cq::api::get_stranger_info(user_id).nickname;
    }

    // 昵称获取 综合
    std::string get_nickname(const cq::Target& target) {
        if (target.user_id.has_value()) {
            if (target.group_id.has_value()) {
                return get_nickname(*target.group_id, *target.user_id, 0);
            }
            if (target.discuss_id.has_value()) {
                return get_nickname(*target.discuss_id, *target.user_id, 1);
            }
            return get_nickname(*target.user_id);
        }
        return msg::GetGlobalMsg("strNicknameError");
    }

    // 设置群昵称
    void set_group_nickname(const int64_t group_id, const int64_t user_id, const int type,
                            const std::string& nick_name) {
        SQLite::Statement st(*db::db,
                             "REPLACE INTO group_user_info(group_id, qq_id, type, nick_name) VALUES(?, ?, ?, ?)");
        st.bind(1, group_id);
        st.bind(2, user_id);
        st.bind(3, type);
        st.bind(4, nick_name);
        st.exec();
    }

    // 设置群昵称
    void set_group_nickname(const cq::Target& target, const std::string& nick_name) {
        if (target.user_id.has_value()) {
            if (target.group_id.has_value()) {
                set_group_nickname(*target.group_id, *target.user_id, 0, nick_name);
            } else if (target.discuss_id.has_value()) {
                set_group_nickname(*target.discuss_id, *target.user_id, 1, nick_name);
            } else {
                throw exception::exception(msg::GetGlobalMsg("strNNPrivateError"));
            }

        } else {
            throw exception::exception(msg::GetGlobalMsg("strSetNicknameError"));
        }
    }

    // 设置全局昵称
    void set_global_nickname(const int64_t user_id, const std::string& nick_name) {
        SQLite::Statement st(*db::db, "REPLACE INTO qq_info(qq_id, nick_name) VALUES(?, ?)");
        st.bind(1, user_id);
        st.bind(2, nick_name);
        st.exec();
    }

    // 设置全局昵称
    void set_global_nickname(const cq::Target& target, const std::string& nick_name) {
        if (target.user_id.has_value()) {
            set_global_nickname(*target.user_id, nick_name);
        } else {
            throw exception::exception(msg::GetGlobalMsg("strSetNicknameError"));
        }
        
    }

    // 格式化字符串
    std::string format_string(const std::string& origin_str, const std::map<std::string, std::string>& format_para) {
        std::string new_str;
        std::regex re("\\{([^\\{\\}]+)\\}");
        auto words_begin = std::sregex_iterator(origin_str.begin(), origin_str.end(), re);
        auto words_end = std::sregex_iterator();
        size_t last = 0;
        for (auto it = words_begin; it != words_end; ++it) {
            if (format_para.count(std::string((*it)[1].first, (*it)[1].second))) {
                new_str += origin_str.substr(last, it->position() - last);
                new_str += format_para.at(std::string((*it)[1].first, (*it)[1].second));
            } else {
                new_str += origin_str.substr(last, it->position() + it->length() - last);
            }
            last = it->position() + it->length();
        }
        new_str += origin_str.substr(last);
        return new_str;
    }

    // 获取权限 群
    bool is_admin_or_owner(const int64_t group_id, const int64_t user_id) {
        auto role = cq::api::get_group_member_info(group_id, user_id).role;
        return (role == cq::GroupRole::ADMIN || role == cq::GroupRole::OWNER);
    }

    // 获取权限 综合 (非群返回永远为真)
    bool is_admin_or_owner(const cq::Target& target) {
        if (target.group_id.has_value() && target.user_id.has_value()) {
            return is_admin_or_owner(*target.group_id, *target.user_id);
        }
        return true;
    }

} // namespace dice::utils
