#pragma once
#include <stack>
#include <string>
#include <vector>

namespace dice {
    class dice_calculator {
    public:
        const std::wstring dice_expression;

        // 过程显示1
        std::wstring res_display;

        // 过程显示2, 这个字符串也被用于计算表达式
        std::wstring res_display2;
        double result = 0.0;

        std::wstring form_string();
        dice_calculator(std::wstring dice_expression);

    private:
        std::stack<double> num_stk;
        std::stack<wchar_t> operator_stk;
        wchar_t operator_stk_top();
        void operator_stk_pop();
        double num_stk_top();
        void num_stk_pop();
        int priority(wchar_t c);
        void main_calculate();
        void scan_and_replace_dice();
        void expression_analyse();
        void pop_stack_and_calculate();
        bool is_number(wchar_t c);
        size_t get_number_size(size_t current_pos);
    };
} // namespace dice