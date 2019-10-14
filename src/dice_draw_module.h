#pragma once
#include <vector>
#include "dice_module.h"


namespace cq::event {
        struct MessageEvent;
}

namespace dice {
    class draw_module : public dice_module {
    public:
        bool match(const cq::event::MessageEvent& e, const std::wstring& ws) override;
        void process(const cq::event::MessageEvent& e, const std::wstring& ws) override;
    };
} // namespace dice