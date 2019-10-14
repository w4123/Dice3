#include "dice_draw_module.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_db.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool draw_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*draw.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void draw_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*draw[ ]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            if (m[1].first != m[1].second) {
                std::string draw_item = cq::utils::ws2s(m[1]);
                if (draw_item.find('|') != std::string::npos) {
                    draw_item = "{#" + draw_item + "}";
                } else {
                    draw_item = "{%" + draw_item + "}";
                }
                cq::api::send_msg(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strDraw"),
                                         {{"nick", utils::get_nickname(e.target)}, {"draw_res", draw_item}}));
            }
        }
    }
} // namespace dice
