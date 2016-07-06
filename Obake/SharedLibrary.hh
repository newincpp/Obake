#pragma once
#include <cassert>
#include "Definitions.hh"
#include "Plugin.hh"
#include <EASTL/EASTL/string.h>

#ifdef WIN32
	#include <Windows.h>
#endif

namespace Obake
{
#ifdef WIN32
	typedef struct
	{
		HMODULE lib;
		char* errmsg;
	} lib_t;
#elif defined(LINUX) || defined (APPLE)
	typedef struct
	{
		void* lib;
		char* errmsg;
	} lib_t;
#endif

	class SharedLibrary
	{
	private: 
		lib_t _lib;
		eastl::string _error;

	public:
		~SharedLibrary();
		SharedLibrary();

		bool open(const eastl::string& path_);
		bool sym(const char* name_, void** ptr_);
		void close();
	/*	void setError(const std::string& prefix);
		std::string error() const;*/
	};
}
