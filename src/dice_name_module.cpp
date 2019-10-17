#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_name_module.h"
#include "dice_utils.h"
#include <cctype>

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool name_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*name.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void name_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*name[\\s]*([a-z]*)[\\s]([0-9]*).*",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;

        if (std::regex_match(ws, m, re)) {
            std::string generate_str = "{%name}";
            if (m[1].first != m[1].second) {
                generate_str = "{%name@" + cq::utils::ws2s(m[1]) + "}";
                std::transform(generate_str.begin(), generate_str.end(), generate_str.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
            }
            int generate_count = 1;
            if (m[2].first != m[2].second) {
                generate_count = std::stoi(m[2]);
                if (generate_count <= 0 || generate_count > 10) {
                    throw exception::exception(msg::GetGlobalMsg("strCountError"));
                }
            }
            std::string str = generate_str;
            for (int i = 1; i != generate_count; i++) {
                str += " ";
                str += generate_str;
            }
            cq::api::send_msg(e.target,
                              utils::format_string(msg::GetGlobalMsg("strNameGenerate"),
                                                   {{"nick", utils::get_nickname(e.target)}, {"generate_str", str}}));
        }
    }
} // namespace dice
