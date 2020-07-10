#pragma once
#include <stack>
#include <string>
#include <vector>
#include <random>

namespace dice {
    class fudge_calculator {
    public:
		int dice_count = 4;
		int add_value = 0;

        // 过程显示1
        std::wstring res_display;

        double result = 0.0;

        std::wstring form_string();
        fudge_calculator(std::wstring dice_count, std::wstring add_value);
        fudge_calculator(int dice_count, int add_value);

		static std::mt19937 ran;

    private:
        void main_calculate();
        size_t get_number_size(size_t current_pos);
    };
} // namespace dice