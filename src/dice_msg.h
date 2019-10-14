#pragma once
#include <map>
#include <string>
#include <vector>
#include "cpprest/asyncrt_utils.h"

namespace dice::msg {
    extern std::string dice_ver;
    extern short dice_build;
    extern std::string dice_info;
    extern std::string dice_full_info;
    extern utility::string_t dice_user_agent;
    extern std::map<std::string, std::string> global_msg;
    extern std::map<std::string, std::string> help_msg;
    extern std::map<std::string, std::vector<std::string>> default_deck;
    std::string GetGlobalMsg(const std::string& str);
    std::string GetHelpMsg(const std::string& str);
}

