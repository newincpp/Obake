#pragma once

#ifdef STL
#undef STL
#endif

//#define stdMode

#ifdef stdMode
#define STL std
#include <iostream>
#else
#define STL eastl
#include <stdio.h>
#endif

namespace Obake {
#ifdef  stdMode
    class ImplStdCout {
	public:
	    static inline void write(const char* msg) {
	    	std::cout << msg << '\n';
	    }
	    static inline void debug(const char* msg) {
#ifdef O_DEBUG
		std::cout << msg << '\n';
#else
		(void)msg;
#endif
	    }
	    static inline void info(const char* msg) {
	    	std::cout << msg << '\n';
	    }
	    static inline void warning(const char* msg) {
	    	std::cout << msg << '\n';
	    }
	    static inline void error(const char* msg) {
	    	std::cout << msg << '\n';
	    }
    };
    class ImplStdlogFile {
	public:
	    void write(const char*) {
	    }
	    void debug(const char*) {
	    }
	    void info(const char*) {
	    }
	    void warning(const char*) {
	    }
	    void error(const char*) {
	    }
    };
#endif

    class ImplPrintf {
	public:
	    static inline void write(const char* msg) {
		printf("%s", msg);
	    }
	    static inline void debug(const char* msg) {
#ifdef O_DEBUG
		printf("%s", msg);
#else
		(void)msg;
#endif
	    }
	    static inline void info(const char* msg) {
		printf("%s", msg);
	    }
	    static inline void warning(const char* msg) {
		printf("%s", msg);
	    }
	    static inline void error(const char* msg) {
		printf("%s", msg);
	    }
    };

#ifdef  stdMode
    typedef ImplStdCout log;
#else 
    typedef ImplPrintf log;
#endif
}
