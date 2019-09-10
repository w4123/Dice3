#pragma once
#include <vector>
#include "dice_module.h"


namespace cq::event {
        struct MessageEvent;
}

namespace dice {
    class echo_module : public dice_module {
    public:
        bool match(const cq::event::MessageEvent& e) override;
        void process(const cq::event::MessageEvent& e) override;
    };
} // namespace dice