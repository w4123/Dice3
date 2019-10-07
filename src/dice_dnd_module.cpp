#include "dice_dnd_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool dnd_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。]dnd.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void dnd_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。]dnd[ ]*([0-9]*).*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            int GenerateCount = 1;
            if (m[1].first != m[1].second) {
                auto GenerateCountStr = std::wstring(m[1].first, m[1].second);
                if (GenerateCountStr.length() > 2) {
                    throw exception::exception(msg::global_msg["strGenerateCountError"]);
                }
                GenerateCount = std::stoi(std::wstring(m[1].first, m[1].second));
                if (GenerateCount > 10 || GenerateCount <= 0) {
                    throw exception::exception(msg::global_msg["strGenerateCountError"]);
                }
            }

            std::string CharacterCards;

            const std::string strProperty[] = {"力量", "体质", "敏捷", "智力", "感知", "魅力"};
            const bool AddSpace = GenerateCount != 1;
            int AllTotal = 0;
            while (GenerateCount--) {
                for (int i = 0; i != 6; i++) {
                    CharacterCards += strProperty[i];
                    CharacterCards += ":";
                    int RollRes = static_cast<int> (dice_calculator(L"4D6K3").result);
                    AllTotal += RollRes;
                    CharacterCards += std::to_string(RollRes);
                    CharacterCards += " ";
                    if (AddSpace && RollRes < 10) {
                        CharacterCards += " ";
                    }
                }
                CharacterCards += "共计:";
                CharacterCards += std::to_string(AllTotal);
                if (GenerateCount) CharacterCards += "\n";
                AllTotal = 0;
            }
            cq::api::send_msg(
                e.target,
                utils::format_string(msg::global_msg["strCharacterCard"],
                                     std::map<std::string, std::string>{{"nick", utils::get_nickname(e.target)},
                                                                        {"version", "DND"},
                                                                        {"character_cards", CharacterCards}}));
        }
    }
} // namespace dice
