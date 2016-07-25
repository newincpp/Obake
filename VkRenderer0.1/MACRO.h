#pragma once

#include <iostream>

#include "DEBUG.h"

#ifdef OBAKE_LOG
#define PRINT(string) std::cout << string << '\n';
#endif
