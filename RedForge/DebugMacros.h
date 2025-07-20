#pragma once

#include <iostream>
#include <format>

#define LOG(Format, ...) std::cout << std::format(Format, __VA_ARGS__) << std::endl;