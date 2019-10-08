#pragma once
#include <map>
#include <string>

namespace dice::msg {
    extern std::string dice_ver;
    extern short dice_build;
    extern std::string dice_info;
    extern std::string dice_full_info;
    extern std::map<std::string, std::string> global_msg;
    extern std::map<std::string, std::string> help_msg;
    std::string GetGlobalMsg(const std::string& str);
    std::string GetHelpMsg(const std::string& str);
}

