#pragma once
#include <string>
#include "cqsdk/cqsdk.h"
#include "cqsdk/types.h"

namespace dice::utils {

    // 设置Jrrp开启情况
    void set_jrrp_enabled(const int64_t group_id, const int type, const bool enabled);

    // 设置Jrrp开启情况
    void set_jrrp_enabled(const cq::Target& target, const bool enabled);

    // 获取Jrrp是否开启
    bool is_jrrp_enabled(const cq::Target& target);

    // 获取Jrrp是否开启
    bool is_jrrp_enabled(const int64_t group_id, const int type);


    // 设置Help开启情况
    void set_help_enabled(const int64_t group_id, const int type, const bool enabled);

    // 设置Help开启情况
    void set_help_enabled(const cq::Target& target, const bool enabled);
    
    // 获取Help是否开启
    bool is_help_enabled(const cq::Target& target);

    // 获取Help是否开启
    bool is_help_enabled(const int64_t group_id, const int type);

    // 设置jrrp
    void set_jrrp(const cq::Target& target, int jrrp_val);

    // 获取jrrp
    std::tuple<bool, int> get_jrrp(const cq::Target& target);

    // 获取今日日期
    std::string get_date();

    // 默认面数获取
    int get_defaultdice(const cq::Target& target);

    // 群名获取
    std::string get_groupname(const int64_t group_id);

    // 来源获取
    std::string get_originname(const cq::Target& target);

    // 昵称获取 群/讨论组 type=0为群， type=1为讨论组
    std::string get_nickname(const int64_t group_id, const int64_t user_id, const int type);

    // 昵称获取 私聊
    std::string get_nickname(const int64_t user_id);

    // 昵称获取 综合
    std::string get_nickname(const cq::Target& target);

    // 设置群昵称
    void set_group_nickname(const cq::Target& target, const std::string& nick_name);

    // 设置群昵称
    void set_group_nickname(const int64_t group_id, const int64_t user_id, const int type,
                            const std::string& nick_name);

    // 设置全局昵称
    void set_global_nickname(const cq::Target& target, const std::string& nick_name);
    
    // 设置全局昵称
    void set_global_nickname(const int64_t user_id, const std::string& nick_name);

    // 格式化字符串
    std::string format_string(const std::string& origin_str, const std::map<std::string, std::string>& format_para);

	// 获取权限 群
    bool is_admin_or_owner(const int64_t group_id, const int64_t user_id);

	// 获取权限 综合 (非群返回永远为真)
    bool is_admin_or_owner(const cq::Target& target);
}
