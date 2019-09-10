#include "dice_module.h"
#include "cqsdk/cqsdk.h"
#include "dice_r_module.h"

CQ_MAIN {

    cq::app::on_enable = [] {
        static dice::r_module RModule;
    };

    auto main_func = [](const auto &e) {
        std::wstring ws = cq::utils::s2ws(e.message.extract_plain_text());
        dice::dice_module *process_module = nullptr;
        for (auto &m : dice::dice_module::enabled_modules) {
            if (m->match(e, ws)) {
                process_module = m;
                break;
            }
        }
        if (process_module) {
            e.block();
            process_module->process(e, ws);
        }
    };
    
    cq::event::on_private_msg = main_func;
    cq::event::on_group_msg = main_func;
    cq::event::on_discuss_msg = main_func;

}
