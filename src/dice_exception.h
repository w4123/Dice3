#pragma once
#include <exception>
#include <stdexcept>
#include <string>
#include "dice_msg.h"

namespace dice::exception {
    struct exception : public std::exception
    {
        exception(const char* what) : std::exception(what){};
        exception(const std::string& what) : std::exception(what.c_str()){};
    };

    struct dice_expression_invalid_error : public exception {
        dice_expression_invalid_error() : exception(dice::msg::global_msg["strInvalidDiceError"]){};
        dice_expression_invalid_error(const char* what) = delete;
        dice_expression_invalid_error(const std::string& what) = delete;
    };
    struct runtime_error : public std::runtime_error {
        runtime_error(const char* what) : std::runtime_error(what){};
        runtime_error(const std::string& what) : std::runtime_error(what.c_str()){};
    };

} // namespace dice::exception