#pragma once
#include <vector>

namespace cq::event {
        struct MessageEvent;
}

namespace dice {
    class dice_module {
    public:
        dice_module();
        ~dice_module();
        dice_module(const dice_module& m) = default;
        dice_module(dice_module&& m) = default;
        dice_module& operator=(const dice_module& m) = default;
        dice_module& operator=(dice_module&& m) = default;
        static std::vector<dice_module*> enabled_modules;
        virtual bool match(const cq::event::MessageEvent& e, const std::wstring& ws) = 0;
        virtual void process(const cq::event::MessageEvent& e, const std::wstring& ws) = 0;
    };
} // namespace dice