#include "dice_msg.h"
#include "dice_db.h"
#include <map>
#include <string>
#include "SQLiteCpp/SQLiteCpp.h"

namespace dice::msg {
    std::string dice_ver = "3.0.0alpha";
    short dice_build = 1000;
    std::string dice_info = "Dice! by 溯洄 Version " + dice_ver + "(" + std::to_string(dice_build) + ")";
    std::string dice_full_info =
        dice_info + " [MSVC " + std::to_string(_MSC_FULL_VER) + " " + __DATE__ + " " + __TIME__ + "]";
    std::map<std::string, std::string> global_msg{
        {"strRollDice", "{nick} 投掷 {reason}: {dice_expression}"},
        {"strInvalidDiceError", "错误: 不合法的掷骰表达式"},
        {"strNicknameError", "内部错误: 无法获取昵称"},
        {"strGroupnameError", "内部错误: 无法获取群名称"},
        {"strPermissionDeniedError", "错误: 此操作需要群主或管理员权限"},
        {"strEnabled", "机器人已启用"},
        {"strDisabled", "机器人已停用"},
        {"strGenerateCountError", "错误: 人物生成个数超出限制(1-10)"},
        {"strCharacterCard", "{nick}的{version}版人物作成:\n{character_cards}"},
        {"strHiddenDice", "{nick}进行了一次暗骰"},
        {"strRollHiddenDice", "在{origin}中 {nick} 投掷 {reason}: {dice_expression}"}
    };

    std::map<std::string, std::string> help_msg{
        {"default",
         "Dice! by 溯洄 Version 3 帮助\n输入.bot以获取版本信息\n输入.help "
         "license以获取软件开源协议信息\n使用说明请访问: https://docs.kokona.tech"},
        {"license",
         "Dice! Version 3 使用MIT协议发布, 详细协议及使用的其他开源项目协议请见源代码\n源代码地址: "
         "https://github.com/w4123/Dice3 \n"}};

    std::string GetGlobalMsg(const std::string& str) {
        SQLite::Statement st(*db::db, "SELECT val FROM global_msg WHERE title=?");
        st.bind(1, str);
        if (st.executeStep()) {
            return st.getColumn(0).getString();
        }
        return "";
    }

    std::string GetHelpMsg(const std::string& str)
    {
        SQLite::Statement st(*db::db, "SELECT val FROM help_msg WHERE title=?");
        st.bind(1, str);
        if (st.executeStep()) {
            return st.getColumn(0).getString();
        }
        return "";
    }
} // namespace dice::msg
