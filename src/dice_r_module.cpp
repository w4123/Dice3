#include "dice_r_module.h"
#include "cqsdk/cqsdk.h"
#include <regex>
#include <string>

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool r_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[\\.。]r.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void r_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {  
        std::wregex re(L"[\\.。]r([0-9d+\\-]*)(.*)",
                      std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        auto ret = std::regex_match(ws, m, re);
        if (m[1].first != m[1].second)
            cq::api::send_msg(e.target, cq::utils::ws2s(std::wstring(m[1].first, m[1].second)));
    }
} // namespace dice