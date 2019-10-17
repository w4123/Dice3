#include "dice_fmt_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool fmt_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*fmt[^]*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void fmt_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*fmt[\\s]*([^]*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            if (m[1].first != m[1].second) {
                cq::api::send_msg(e.target,
                                  utils::format_string(cq::utils::ws2s(m[1]),
                                                       {{"nick", utils::get_nickname(e.target)}}));
            }
        }
    }
} // namespace dice
