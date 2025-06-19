#pragma once

#include <iostream>

#define LOG(format, ...)			\
printf(format, __VA_ARGS__);		\
std::cout << std::endl;