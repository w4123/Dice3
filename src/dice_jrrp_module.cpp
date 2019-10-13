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
        std::wregex re(L"[ ]*[\\.。．][ ]*jrrp[ ]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            std::wstring command(m[1]);
            std::transform(command.begin(), command.end(), command.begin(), towlower);
            if (command == L"on") {
                utils::set_jrrp_enabled(e.target, true);
                cq::api::send_msg(e.target, msg::GetGlobalMsg("strSetJrrpEnabled"));
            } else if (command == L"off") {
                utils::set_jrrp_enabled(e.target, false);
                cq::api::send_msg(e.target, msg::GetGlobalMsg("strSetJrrpDisabled"));
            } else {
                if (!utils::is_jrrp_enabled(e.target)) {
                    cq::api::send_msg(e.target, msg::GetGlobalMsg("strJrrpDisabled"));
                    return;
                }
                auto res = utils::get_jrrp(e.target);
                if (std::get<0>(res)) {
                    cq::api::send_msg(e.target,
                                      utils::format_string(msg::GetGlobalMsg("strJrrp"),
                                                           {{"nick", utils::get_nickname(e.target)},
                                                            {"jrrp_val", std::to_string(std::get<1>(res))}}));
                    return;
                }

                // 请求Client
                web::http::client::http_client http_client(U("http://api.kokona.tech:5555/"));
                // POST数据
                const utf8string data = "QQ=" + std::to_string(cq::api::get_login_user_id()) + "&v=20190114"
                                        + "&QueryQQ=" + std::to_string(*e.target.user_id);
                // 设置请求参数
                web::http::http_request req(web::http::methods::POST);
                req.set_request_uri(U("/jrrp"));
                req.set_body(data, "application/x-www-form-urlencoded;charset=utf-8");
                // 设置UA
                req.headers().add(U("User-Agent"), msg::dice_user_agent);
                // 发起异步请求，堵塞等待结果
                web::http::http_response response = http_client.request(req).get();
                // 获取结果
                auto jrrp_val_str = response.extract_utf8string(true).get();
                cq::api::send_msg(
                    e.target,
                    utils::format_string(msg::GetGlobalMsg("strJrrp"),
                                         {{"nick", utils::get_nickname(e.target)}, {"jrrp_val", jrrp_val_str}}));
                auto jrrp_val = std::stoi(jrrp_val_str);
                utils::set_jrrp(e.target, jrrp_val);
            }
        }
    }
} // namespace dice
