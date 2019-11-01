#include "dice_msg_queue.h"
#include "cqsdk/cqsdk.h"

namespace dice::msg_queue {
    void msg_queue::start() {
        if (_activated) return;
        _activated = true;
        _msg_send_thread = std::thread([&] {
            _thread_running = true;
            while (_activated) {
                std::pair<cq::Target, std::string> msg;
                {
                    std::lock_guard<std::mutex> lk(_mutex_send_thread);
                    if (!_queue.empty()) {
                        msg = std::move(_queue.front());
                        _queue.pop();
                    }
                }
                if (!msg.second.empty()) {
                    try {
                        cq::api::send_msg(msg.first, msg.second);
                    } catch (const std::exception& e) {
                        cq::logging::debug("Dice!",e.what());
                    }
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }
            }
            _thread_running = false;
        });
        _msg_send_thread.detach();
    }
    void msg_queue::stop() {
        if (!_activated) return;
        _activated = false;
        while (_thread_running) std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    msg_queue::~msg_queue() { stop(); }
    void msg_queue::add(const cq::Target& target, std::string&& msg) {
        std::lock_guard<std::mutex> lk(_mutex_send_thread);
        _queue.push(std::make_pair(target, std::move(msg)));
    }
    void msg_queue::add(const cq::Target& target, const std::string& msg) {
        std::lock_guard<std::mutex> lk(_mutex_send_thread);
        _queue.push(std::make_pair(target, msg));
    }
    msg_queue MsgQueue;
} // namespace dice::msg_queue
