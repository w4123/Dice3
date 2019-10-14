#include "dice_echo_module.h"
#include "cqsdk/cqsdk.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool echo_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) { return true; }

    void echo_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        cq::api::send_msg(e.target, e.message);
    }
} // namespace dice
