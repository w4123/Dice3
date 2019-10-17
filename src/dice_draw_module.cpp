#include "dice_draw_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool draw_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*draw.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void draw_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*draw[ ]*(.*?)[ ]*([0-9]*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            if (m[1].first != m[1].second) {
                int count = 1;
                if (m[2].first != m[2].second) {
                    count = std::stoi(m[2]);
                }
                if (count == 0 || count > 10) {
                    throw exception::exception(msg::GetGlobalMsg("strCountError"));
                }
                std::string draw_item = cq::utils::ws2s(m[1]);
                if (draw_item.find('|') != std::string::npos) {
                    draw_item = "{#" + draw_item + +":" + std::to_string(count) + "}";
                } else {
                    std::string single_item = "{%" + draw_item + "}";
                    draw_item = single_item;
                    for (int i = 1; i != count; i++) {
                        draw_item += '\n';
                        draw_item += single_item;
                    }
                }
                cq::api::send_msg(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strDraw"),
                                         {{"nick", utils::get_nickname(e.target)}, {"draw_res", draw_item}}));
            }
        }
    }
} // namespace dice
