#pragma once
#include <mutex>
#include <queue>
#include <atomic>
#include <thread>
#include "cqsdk/cqsdk.h"
namespace dice::msg_queue {

    class msg_queue {
    public:
        std::atomic<bool> _activated = false;
        std::atomic<bool> _thread_running = false;
        std::mutex _mutex_send_thread;
        std::thread _msg_send_thread;
        std::queue<std::pair<cq::Target, std::string>> _queue;
        void add(const cq::Target& target, std::string&& msg);
        void add(const cq::Target& target, const std::string& msg);
        void start();
        void stop();
        msg_queue() = default;
        ~msg_queue();
        msg_queue(const msg_queue&) = delete;
        msg_queue(msg_queue&&) = delete;
    };
    extern msg_queue MsgQueue;
} // namespace dice::msg_queue