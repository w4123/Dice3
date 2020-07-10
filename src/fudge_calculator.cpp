#include "fudge_calculator.h"
#include <cwctype>
#include <exception>
#include <random>
#include <regex>
#include <string>
#include "dice_exception.h"

namespace dice {

    std::mt19937 fudge_calculator::ran(clock());

    fudge_calculator::fudge_calculator(std::wstring dice_count, std::wstring add_value)
        : dice_count(std::stoi(dice_count)), add_value(std::stoi(add_value)) {
        main_calculate();
    }

    fudge_calculator::fudge_calculator(int dice_count, int default_dice)
        : dice_count(dice_count), add_value(add_value) {
        main_calculate();
    }

    void fudge_calculator::main_calculate() {
		std::uniform_int_distribution gen(-1, 1);
		res_display = L"[";
        for (int counter = 0; counter != dice_count; counter++)
		{
			if (counter != 0) res_display.append(L", ");
			int temp_res = gen(ran);
			if (temp_res == -1) {
				res_display.append(L"-");
				result -= 1;
			}
			else if(temp_res == 0) {
				res_display.append(L"0");
			}
			else {
				res_display.append(L"+");
				result += 1;
			}
		}
		res_display.append(L"]");
		if (add_value)
		{
			if(add_value >= 0) res_display.append(L"+");
			res_display.append(std::to_wstring(add_value));
			result += add_value;
		}
    }

    std::wstring fudge_calculator::form_string() {
		std::wstring str;
        std::wstring dice_expression;
		dice_expression.append(std::to_wstring(dice_count));
		dice_expression.append(L"DF");
		if (add_value)
		{
			if(add_value >= 0) dice_expression.append(L"+");
			dice_expression.append(std::to_wstring(add_value));
		}
		str.append(dice_expression);
        if (dice_expression != res_display) {
            str += L"=" + res_display;
        }
        std::wstring res = std::to_wstring(result);
        res = res.substr(0, std::max(res.find(L'.'), std::min(res.find(L'.') + 3, res.find_last_not_of(L"0.") + 1)));
        if (res != res_display) {
            str += L"=" + res;
        }
        if (str.length() > 160) str = dice_expression + L"=" + res;
        return str;
    }
} // namespace dice
