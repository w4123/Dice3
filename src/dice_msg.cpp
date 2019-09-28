#include <map>
#include <string>
#include "dice_msg.h"

namespace dice::msg {
    std::map<std::string, std::string> global_msg{
        {"strRollDice", "{nick} 投掷 {reason}: {dice_expression}"},
        {"strInvalidDiceError", "错误: 不合法的掷骰表达式"},
        {"strNicknameError", "[无法获取昵称]"}
    };
}
