#pragma once
#include <vector>
#include "dice_module.h"


namespace cq::event {
        struct MessageEvent;
}

namespace dice {
    class bot_module : public dice_module {
        std::tuple<bool, bool> search_db(const cq::event::MessageEvent& e);
        void insert_db(const cq::event::MessageEvent& e, bool disabled);
        void update_db(const cq::event::MessageEvent& e, bool disabled);
    public:
        bot_module();
        bool match(const cq::event::MessageEvent& e, const std::wstring& ws) override;
        void process(const cq::event::MessageEvent& e, const std::wstring& ws) override;
    };
} // namespace dice