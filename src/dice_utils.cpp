#include <map>
#include <regex>
#include <string>
#include "cqsdk/cqsdk.h"
#include "cqsdk/types.h"
#include "dice_msg.h"
#include "dice_utils.h"

namespace dice::utils {

    // 昵称获取 群
    std::string get_nickname(const int64_t group_id, const int64_t user_id) {
        const cq::GroupMember gm = cq::api::get_group_member_info(group_id, user_id);
        if (!gm.card.empty()) {
            return gm.card;
        }
        return gm.nickname;
    }

    // 昵称获取 讨论组/私聊
    std::string get_nickname(const int64_t user_id) { return cq::api::get_stranger_info(user_id).nickname; }

    // 昵称获取 综合
    std::string get_nickname(const cq::Target& target) {
        if (target.user_id.has_value()) {
            if (target.group_id.has_value()) {
                return get_nickname(*target.group_id, *target.user_id);
            }
            return get_nickname(*target.user_id);
        }
        return msg::global_msg["strNicknameError"];
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

} // namespace dice::utils
