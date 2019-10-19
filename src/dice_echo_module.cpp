#include "dice_echo_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_msg_queue.h"

namespace cq::event {
    struct MessageEvent;
}

namespace dice {
    bool echo_module::match(const cq::event::MessageEvent &e, const std::wstring &ws) { return true; }

    void echo_module::process(const cq::event::MessageEvent &e, const std::wstring &ws) {
        dice::msg_queue::MsgQueue.add(e.target, e.message);
    }
} // namespace dice
