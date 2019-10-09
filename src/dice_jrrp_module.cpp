#include "dice_jrrp_module.h"
#include "cpprest/http_client.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_utils.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool jrrp_module::match(const cq::event::MessageEvent& e, const std::wstring& ws) {
        std::wregex re(L"[ ]*[\\.。．][ ]*jrrp.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }
    void jrrp_module::process(const cq::event::MessageEvent& e, const std::wstring& ws) {
        auto res = utils::get_jrrp(e.target);
        if (std::get<0>(res)) {
            cq::api::send_msg(e.target,
                              utils::format_string(msg::GetGlobalMsg("strJrrp"),
                                                   {{"nick", utils::get_nickname(e.target)},
                                                    {"jrrp_val", std::to_string(std::get<1>(res))}}));
            return;
        }

        web::http::client::http_client http_client(U("http://api.kokona.tech:5555/"));
        const utf8string data = "QQ=" + std::to_string(cq::api::get_login_user_id()) + "&v=20190114"
                                + "&QueryQQ=" + std::to_string(*e.target.user_id);
        web::http::http_response response =
            http_client
                .request(web::http::methods::POST, "/jrrp", data, "application/x-www-form-urlencoded;charset=utf-8")
                .get();

        auto jrrp_val_str = response.extract_utf8string(true).get();
        cq::api::send_msg(e.target,
                          utils::format_string(msg::GetGlobalMsg("strJrrp"),
                                               {{"nick", utils::get_nickname(e.target)}, {"jrrp_val", jrrp_val_str}}));
        auto jrrp_val = std::stoi(jrrp_val_str);
        utils::set_jrrp(e.target, jrrp_val);

    }
} // namespace dice
