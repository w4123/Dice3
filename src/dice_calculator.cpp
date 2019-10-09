#include "dice_calculator.h"
#include <cwctype>
#include <exception>
#include <random>
#include <regex>
#include <string>
#include "dice_exception.h"

namespace dice {

    std::mt19937 dice_calculator::ran(clock());

    dice_calculator::dice_calculator(std::wstring dice_expression) : dice_expression(std::move(dice_expression)) {
        main_calculate();
    }

    dice_calculator::dice_calculator(std::wstring dice_expression, int default_dice)
        : dice_expression(std::move(dice_expression)), default_dice(default_dice) {
        main_calculate();
    }

    wchar_t dice_calculator::operator_stk_top() {
        if (operator_stk.empty()) {
            throw exception::dice_expression_invalid_error();
        }
        return operator_stk.top();
    }
    void dice_calculator::operator_stk_pop() {
        if (operator_stk.empty()) {
            throw exception::dice_expression_invalid_error();
        }
        operator_stk.pop();
    }
    double dice_calculator::num_stk_top() {
        if (num_stk.empty()) {
            throw exception::dice_expression_invalid_error();
        }
        return num_stk.top();
    }
    void dice_calculator::num_stk_pop() {
        if (num_stk.empty()) {
            throw exception::dice_expression_invalid_error();
        }
        num_stk.pop();
    }

    int dice_calculator::priority(wchar_t c) {
        switch (c) {
        case L'^':
            return 0;
        case L'*':
        case L'/':
        case L'x':
        case L'X':
        case L'×':
        case L'÷':
            return 1;
        case L'+':
        case L'-':
            return 2;
        case L'(':
            return 3;
        default:
            throw exception::dice_expression_invalid_error();
        }
    }

    // 替换标准骰子及惩罚奖励骰(3d6, 4d6k3, p3, b3)
    void dice_calculator::scan_and_replace_dice() {
        // 用于把简写替换成完整拼写，方便阅读
        std::wstring replaced_dice_expression;

        // 匹配标准掷骰
        std::wregex d(L"(([0-9]*)d([0-9]*)(k([0-9]*))?)|((b|p)([1-3])?)",
                      std::regex_constants::ECMAScript | std::regex_constants::icase);
        auto words_begin = std::wsregex_iterator(dice_expression.begin(), dice_expression.end(), d);
        auto words_end = std::wsregex_iterator();

        // 骰子个数 骰子面数 上一个匹配到的位置的下一位
        int left = 0, right = 0, last = 0, keep = 0;
        bool EnableKeep = false;
        for (auto i = words_begin; i != words_end; ++i) {
            // 两个连续且中间无任何运算符的骰子属于错误表达式
            if (last && last == i->position()) {
                throw exception::dice_expression_invalid_error();
            }

            // 骰子前后有数字属于错误表达式
            if ((i->position() && std::iswdigit(dice_expression[i->position() - 1]))
                || (i->position() + i->length() != dice_expression.length()
                    && std::iswdigit(dice_expression[i->position() + i->length()])))

            {
                throw exception::dice_expression_invalid_error();
            }

            left = 1;
            right = default_dice;
            keep = 1;
            EnableKeep = false;

            // 把前面的字符串挪过来
            replaced_dice_expression +=
                std::wstring(dice_expression.begin() + last, dice_expression.begin() + i->position());
            res_display += std::wstring(dice_expression.begin() + last, dice_expression.begin() + i->position());
            res_display2 += std::wstring(dice_expression.begin() + last, dice_expression.begin() + i->position());
            last = i->position() + i->length();
            std::wsmatch match = *i;

            // 保存这个骰子的信息并替换缩写
            std::wstring dice(match[0]);

            // 字母转换为大写
            for (auto& c : dice) {
                if (c == L'd') c = L'D';
                if (c == L'k') c = L'K';
                if (c == L'b') c = L'B';
                if (c == L'p') c = L'P';
            }

            // 奖励惩罚骰
            if (match[1].first == match[1].second) {
                int DiceCount = 1;
                if (match[8].first != match[8].second) {
                    DiceCount = std::stoi(match[8]);
                }
                std::uniform_int_distribution<int> gen100(1, 100);
                std::uniform_int_distribution<int> gen9(0, 9);
                std::uniform_int_distribution<int> gen10(1, 10);
                int first_res = gen100(ran);
                std::vector<int> bpTempStorage;
                int res = 0;

                if (first_res % 10) {
                    for (int k = 0; k != DiceCount; ++k) {
                        bpTempStorage.push_back(gen9(ran));
                    }
                } else {
                    for (int k = 0; k != DiceCount; ++k) {
                        bpTempStorage.push_back(gen10(ran));
                    }
                }

                std::wstring di;
                if (match[7] == L"b") {
                    di = std::to_wstring(first_res) + L"[奖励骰:";
                    first_res =
                        std::min((first_res / 10), *std::min_element(bpTempStorage.begin(), bpTempStorage.end())) * 10
                        + (first_res % 10);
                } else {
                    di = std::to_wstring(first_res) + L"[惩罚骰:";
                    first_res =
                        std::max((first_res / 10), *std::max_element(bpTempStorage.begin(), bpTempStorage.end())) * 10
                        + (first_res % 10);
                }
                for (const auto& c : bpTempStorage) {
                    di += std::to_wstring(c);
                    di += L" ";
                }
                di[di.length() - 1] = L']';
                res_display += di;
                res_display2 += std::to_wstring(first_res);
                if (dice.length() == 1) dice += L"1";
                replaced_dice_expression += dice;
                continue;
            }

            // 普通骰
            if (match[2].first != match[2].second) {
                left = std::stoi(match[2]);
            } else {
                dice = std::to_wstring(left) + dice;
            }
            if (match[3].first != match[3].second) {
                right = std::stoi(match[3]);
            } else {
                dice.insert(dice.find(L'D') + 1, std::to_wstring(right));
            }

            if (match[4].first != match[4].second) {
                EnableKeep = true;
                if (match[5].first != match[5].second) {
                    keep = std::stoi(match[5]);
                    if (keep <= 0 || keep > left) {
                        throw exception::dice_expression_invalid_error();
                    }
                } else {
                    dice += std::to_wstring(keep);
                }
            }

            if (left <= 0 || right <= 0) {
                throw exception::dice_expression_invalid_error();
            }

            std::wstring di;
            int temp_total = 0;
            std::uniform_int_distribution<int> gen(1, right);
            std::vector<int> DiceResults;
            for (int k = 0; k != left; k++) {
                int temp_res = gen(ran);
                if (!EnableKeep || static_cast<int>(DiceResults.size()) < keep) {
                    DiceResults.push_back(temp_res);
                } else {
                    auto min_ele = std::min_element(DiceResults.begin(), DiceResults.end());
                    if (temp_res > *min_ele) *min_ele = temp_res;
                }
            }

            for (int k = 0; k != DiceResults.size(); k++) {
                temp_total += DiceResults[k];
                di += std::to_wstring(DiceResults[k]);
                if (k != DiceResults.size() - 1) di += L"+";
            }

            if (!(DiceResults.size() == 1 || i->length() == dice_expression.length()
                  || (i->position() != 0 && dice_expression[i->position() - 1] == L'('
                      && i->position() + i->length() != dice_expression.length()
                      && dice_expression[i->position() + i->length()] == L')'))) {
                di = L"(" + di + L")";
            }
            res_display += di;
            res_display2 += std::to_wstring(temp_total);
            replaced_dice_expression += dice;
        }
        res_display += std::wstring(dice_expression.begin() + last, dice_expression.end());
        res_display2 += std::wstring(dice_expression.begin() + last, dice_expression.end());
        replaced_dice_expression += std::wstring(dice_expression.begin() + last, dice_expression.end());
        dice_expression = replaced_dice_expression;
    }

    void dice_calculator::main_calculate() {
        scan_and_replace_dice();
        expression_analyse();
    }

    bool dice_calculator::is_number(wchar_t c) { return std::wstring(L"0123456789").find(c) != std::wstring::npos; }
    size_t dice_calculator::get_number_size(size_t current_pos) {
        size_t temp_res = res_display2.find_first_not_of(L"0123456789.", current_pos);
        if (temp_res == std::wstring::npos) {
            return res_display2.length() - temp_res;
        }
        return temp_res - current_pos;
    }

    void dice_calculator::expression_analyse() {
        for (size_t i = 0; i < res_display2.size();) {
            if ((res_display2[i] == L'-' && (i == 0 || res_display2[i - 1] == L'(')) || is_number(res_display2[i])) {
                if (res_display2[i] == L'-' && i + 1 < res_display2.size() && res_display2[i + 1] == L'(') {
                    num_stk.push(-1);
                    i++;
                } else {
                    double number = stod(res_display2.substr(i));
                    num_stk.push(number);
                    if (res_display2[i] == L'-') i++;
                    i += get_number_size(i);
                }

                if (i < res_display2.size() && res_display2[i] == L'(') {
                    if (!operator_stk.empty() && priority(L'*') >= priority(operator_stk.top()))
                        pop_stack_and_calculate();
                    operator_stk.push(L'*');
                }
            } else {
                if (operator_stk.empty() || res_display2[i] == L'(') {
                    operator_stk.push(res_display2[i]);
                } else if (res_display2[i] == L')') {
                    while (operator_stk_top() != L'(') {
                        pop_stack_and_calculate();
                    }
                    operator_stk.pop();
                    if (i + 1 < res_display2.size() && res_display2[i + 1] == L'(') {
                        if (!operator_stk.empty() && priority(L'*') >= priority(operator_stk.top()))
                            pop_stack_and_calculate();
                        operator_stk.push(L'*');
                    }
                } else {
                    if (priority(res_display2[i]) >= priority(operator_stk_top())) {
                        pop_stack_and_calculate();
                    }
                    operator_stk.push(res_display2[i]);
                }
                ++i;
            }
        }
        while (!operator_stk.empty()) {
            pop_stack_and_calculate();
        }
        if (num_stk.size() != 1) {
            throw exception::dice_expression_invalid_error();
        }
        result = num_stk.top();
    }

    void dice_calculator::pop_stack_and_calculate() {
        double left = 0, right = 0, res = 0;
        wchar_t oper = operator_stk_top();
        right = num_stk_top();
        num_stk.pop();
        left = num_stk_top();
        num_stk.pop();
        operator_stk.pop();
        switch (oper) {
        case L'+':
            res = left + right;
            break;
        case L'-':
            res = left - right;
            break;
        case L'*':
        case L'x':
        case L'X':
        case L'×':
            res = left * right;
            break;
        case L'/':
        case L'÷':
            res = left / right;
            break;
        case L'^':
            res = pow(left, right);
            break;
        default:
            throw exception::dice_expression_invalid_error();
        }
        num_stk.push(res);
    }
    std::wstring dice_calculator::form_string() {
        std::wstring str = dice_expression;
        if (dice_expression != res_display) {
            str += L"=" + res_display;
        }
        if (res_display != res_display2) {
            str += L"=" + res_display2;
        }
        std::wstring res = std::to_wstring(result);
        res = res.substr(0, std::max(res.find(L'.'), std::min(res.find(L'.') + 3, res.find_last_not_of(L"0.") + 1)));
        if (res != res_display2) {
            str += L"=" + res;
        }
        if (str.length() > 160) str = dice_expression + L"=" + res;
        return str;
    }
} // namespace dice