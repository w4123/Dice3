#include "dice_r_module.h"
#include <regex>
#include <string>
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool r_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[\\.。]r.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void r_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[\\.。]r([0-9d+\\-*/\\(\\)\\^]*)(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        auto ret = std::regex_match(ws, m, re);
        std::wstring res;
        try {
            if (m[1].first != m[1].second) {
                res = dice_calculator(std::wstring(m[1].first, m[1].second)).form_string();
            } else {
                res = dice_calculator(std::wstring(L"d")).form_string();
            }
        } catch(std::exception exp) {
            cq::api::send_msg(e.target, std::string("ERROR: ") + exp.what());
            return;
        }

        cq::api::send_msg(e.target, cq::utils::ws2s(res));
    }
} // namespace dice