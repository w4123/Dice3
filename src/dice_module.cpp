#include <algorithm>
#include <vector>
#include "dice_module.h"

namespace dice {
    dice_module::dice_module()
    {
        enabled_modules.push_back(this);
    }
    dice_module::~dice_module() {
        const auto find_res = std::find(enabled_modules.begin(), enabled_modules.end(), this);
        if (find_res != enabled_modules.end()) {
            enabled_modules.erase(find_res);
        }
    }
    std::vector<dice_module*> dice_module::enabled_modules;
} // namespace dice