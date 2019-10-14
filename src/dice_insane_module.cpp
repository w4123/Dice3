#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_insane_module.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool insane_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*(?:ti|li).*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void insane_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*(ti|li).*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring insane_type(m[1]);
            std::transform(insane_type.begin(), insane_type.end(), insane_type.begin(), std::towlower);
            std::string insane_str;
            if (insane_type == L"ti") {
                insane_str = "{%即时症状}";
            } else {
                insane_str = "{%总结症状}";
            }
            cq::api::send_msg(
                e.target,
                utils::format_string(msg::GetGlobalMsg("strInsane"),
                                     {
                                         {"nick", utils::get_nickname(e.target)},
                                         {"insane_type", insane_type == L"ti" ? "即时" : "总结"},
                                         {"insane_str", insane_str}
                                     }));
        }
    }
} // namespace dice
