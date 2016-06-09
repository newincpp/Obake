#pragma once

#define OBAKE_MAJOR_VERSION    0
#define OBAKE_MINOR_VERSION    1
#define OBAKE_PATCH_VERSION    0

#define OBAKE_AUX_STR_EXP(__A) #__A
#define OBAKE_AUX_STR(__A)     OBAKE_AUX_STR_EXP(__A)
#define OBAKE_VERSION          OBAKE_AUX_STR(OBAKE_MAJOR_VERSION) "." OBAKE_AUX_STR(OBAKE_MINOR_VERSION) "." OBAKE_AUX_STR(OBAKE_PATCH_VERSION)

#ifdef _WIN32
// Windows (x64 and x86)
#ifndef WIN32
#define WIN32
#endif
#endif

#if __unix__
// Unix
#ifndef UNIX
#define UNIX
#endif
#endif

#if __posix__
// POSIX
#ifndef POSIX
#define POSIX
#endif
#endif

#if __linux__
// Linux
#ifndef LINUX
#define LINUX
#endif
#endif

#if __APPLE__
// Mac OS
#ifndef APPLE
#define APPLE
#endif
#endif

#if __GNUC__
// GCC compiler
#ifndef GNUC
#define GNUC
#endif
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
// MinGW
#ifndef MINGW
#define MINGW
#endif
#endif

// Disable unnecessary warnings
#if defined(_MSC_VER)
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4251) // ... needs to have dll-interface warning
#pragma warning(disable:4355) // 'this' : used in base member initializer list
#pragma warning(disable:4996) // VC++ 8.0 deprecation warnings
#pragma warning(disable:4351) // new behavior: elements of array '...' will be default initialized
#pragma warning(disable:4675) // resolved overload was found by argument-dependent lookup
#pragma warning(disable:4275) // non dll-interface class 'std::exception' used as base for dll-interface class 'scy::Exception'
#endif

#ifdef WIN32
#define PLUGINS_FOLDER	".\\Plugins\\"
#else
#define PLUGINS_FOLDER	"./Plugins/"
#endif