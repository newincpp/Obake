#pragma once

#include <iostream>

#include "DEBUG.h"

#if OBAKE_LOG
#define PRINT(string) std::cout << string << '\n';
#else
#define PRINT(string);
#endif

#define WIDTH 800
#define HEIGHT 600
