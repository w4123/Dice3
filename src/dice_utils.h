#pragma once
#include <string>
#include <set>
#include "cqsdk/cqsdk.h"
#include "cqsdk/types.h"

namespace dice::utils {
    // 成功等级枚举类
    enum class success_level : int {
        Fumble = 0,
        Failure = 1,
        Success = 2,
        HardSuccess = 3,
        ExtremeSuccess = 4,
        CriticalSuccess = 5
    };

    // 获取当前COC判定规则
    int get_success_rule(const cq::Target& target);

    // 获取判定成功等级
    success_level get_success_level(const cq::Target& target, const int res, const int rate);

    // 获取判定成功等级字符串
    std::string get_success_indicator(const cq::Target& target, const int value, const int judge_value);

    // 查询某人物卡是否存在
    bool if_card_exist(const cq::Target& target, const std::string& card_name);

    // 设置当前绑定的人物卡
    void set_chosen_card(const cq::Target& target, const std::string& card_name);

    // 获取当前绑定的人物卡
    std::string get_chosen_card(const cq::Target& target);

    // 获取某人物卡中的单个属性
    int get_single_card_properties(const cq::Target& target, const std::string& character_card_name,
                                   std::string& property);

    // 获取某人物卡中的某(些)属性
    std::map<std::string, int> get_card_properties(const cq::Target& target, const std::string& character_card_name,
                                                   const std::set<std::string>& st_character_properties);
    // 获取某人物卡中的某(些)属性
    std::string get_card_properties_string(const cq::Target& target, const std::string& character_card_name,
                                       const std::set<std::string>& st_character_properties);
    // 删除某人物卡中的某(些)属性
    void delete_character_properties(const cq::Target& target, const std::string& character_card_name,
                                     const std::set<std::string>& st_character_properties);
    // 删除某人物卡
    void delete_character_card(const cq::Target& target, const std::string& character_card_name);

    // 获取某人的所有人物卡名称
    std::set<std::string> get_all_card_name(const cq::Target& target);

    // 获取某人的所有人物卡名称
    std::string get_all_card_name_string(const cq::Target& target);

    // 获取某个人物卡的全部属性
    std::map<std::string, int> get_all_card_properties(const cq::Target& target,
                                                       const std::string& character_card_name);

    // 获取某个人物卡的全部属性
    std::string get_all_card_properties_string(const cq::Target& target, const std::string& character_card_name);
    
    // 设置用户人物卡
    void set_character_card(const cq::Target& target, const std::string& character_card_name,
                            const std::map<std::string, int>& mp_character_card);
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
    std::string get_nickname(const int64_t group_id, const int64_t user_id, const int type, const bool only_from_db = false);

    // 昵称获取 私聊
    std::string get_nickname(const int64_t user_id, const bool only_from_db = false);

    // 昵称获取 综合
    std::string get_nickname(const cq::Target& target, const bool only_from_db = false);

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
    std::string format_string(
        const std::string& origin_str,
        const std::map<std::string, std::string>& format_para = std::map<std::string, std::string>());

	// 获取权限 群
    bool is_admin_or_owner(const int64_t group_id, const int64_t user_id);

	// 获取权限 综合 (非群返回永远为真)
    bool is_admin_or_owner(const cq::Target& target);
}
