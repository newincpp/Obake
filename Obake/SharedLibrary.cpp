#include "SharedLibrary.hh"

Obake::SharedLibrary::~SharedLibrary()
{
}

Obake::SharedLibrary::SharedLibrary()
{
}

bool Obake::SharedLibrary::open(const std::string& path_)
{
#if defined (WIN32)
	HMODULE library = LoadLibraryA(path_.c_str());

#elif defined (UNIX) || defined (APPLE)

	void* library = dlopen(path_.c_str(), RTLD_LAZY);

#endif

	if (library == nullptr)
		return false;

	_lib.lib = library;

	return true;
}

bool Obake::SharedLibrary::sym(const char* name_, void** ptr_)
{
#if defined (WIN32)
	void* initializer = (void*)GetProcAddress(_lib.lib, name_);
#elif defined (UNIX) || defined (APPLE)
	void* initializer = dlsym(_lib, name_);
#endif

	*ptr_ = initializer;

	if (initializer == nullptr)
		return false;
	/*if (_lib.errmsg ? _lib.errmsg : "no error")
		return false;*/

	return true;
}

void Obake::SharedLibrary::close()
{
#if defined (WIN32)
	FreeLibrary(_lib.lib);
#elif defined (UNIX) || defined (APPLE)
	dlclose(_lib.lib);
#endif
}

//void Obake::SharedLibrary::setError(const std::string& prefix)
//{
//#ifdef WIN32
//	std::string err(uv_dlerror(&_lib));
//#elif UNIX || APPLE
//
//#endif
//
//	assert(!err.empty());
//	_error = prefix + ": " + err;
//}

//std::string Obake::SharedLibrary::error() const
//{
//	return _error;
//}
