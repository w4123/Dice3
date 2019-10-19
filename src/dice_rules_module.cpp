#include "cpprest/http_client.h"
#include "cqsdk/cqsdk.h"
#include "dice_calculator.h"
#include "dice_exception.h"
#include "dice_rules_module.h"
#include "dice_msg_queue.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool rules_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*rules.*", std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_match(ws, re);
    }

    void rules_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        std::wregex re(L"[\\s]*[\\.。．][\\s]*rules[\\s]*(?:([a-z0-9]*)(?::|：))?[\\s]*(.*)",
                       std::regex_constants::ECMAScript | std::regex_constants::icase);
        std::wsmatch m;
        if (std::regex_match(ws, m, re)) {
            if (m[2].first == m[2].second) {
                throw exception::exception(msg::GetGlobalMsg("strParaEmptyError"));
            }

            // 请求必须大写
            std::wstring type(m[1]);
            std::wstring item(m[2]);
            std::transform(item.begin(), item.end(), item.begin(), std::towupper);
            std::transform(type.begin(), type.end(), type.begin(), std::towupper);

            // 请求Client
            web::http::client::http_client http_client(U("http://api.kokona.tech:5555/"));
            // POST数据
            utf8string data = "QQ=" + std::to_string(cq::api::get_login_user_id()) + "&v=20190114"
                              + "&Name=" + utility::conversions::utf16_to_utf8(web::uri::encode_data_string(item));

            if (!type.empty()) {
                data += "&Type=Rules-" + utility::conversions::utf16_to_utf8(type);
            }

            // 设置请求参数
            web::http::http_request req(web::http::methods::POST);
            req.set_request_uri(U("/rules"));
            req.set_body(data, "application/x-www-form-urlencoded;charset=utf-8");
            // 设置UA
            req.headers().add(U("User-Agent"), msg::dice_user_agent);
            // 发起异步请求，堵塞等待结果
            web::http::http_response response = http_client.request(req).get();

            std::string res = response.extract_utf8string().get();
            if (res.empty()) {
                dice::msg_queue::MsgQueue.add(e.target, msg::GetGlobalMsg("strRulesNotFoundError"));
            } else {
                dice::msg_queue::MsgQueue.add(e.target, res);
            }
        }
    }
} // namespace dice
