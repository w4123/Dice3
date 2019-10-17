#pragma once
#include <map>
#include <string>
#include <vector>
#include "cpprest/asyncrt_utils.h"

namespace dice::msg {
    extern const std::string dice_ver;
    extern const short dice_build;
    extern const std::string dice_info;
    extern const std::string dice_full_info;
    extern const utility::string_t dice_user_agent;
    extern const std::map<std::string, std::string> global_msg;
    extern const std::map<std::string, std::string> help_msg;
    extern const std::map<std::string, std::vector<std::string>> default_deck;
    extern const std::map<std::string, int> SkillDefaultVal;
    extern const std::map<std::string, std::string> SkillNameReplace;
    std::string GetGlobalMsg(const std::string& str);
    std::string GetHelpMsg(const std::string& str);
}

