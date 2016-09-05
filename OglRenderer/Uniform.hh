#pragma once
#include <iostream>
#include "glew.h"

template <typename T>
class Uniform {
    private:
	T _value;
	unsigned short _shaderRefCount;
	unsigned short _updateCount;
    public:
    	explicit Uniform(T v_) : _value(v_) {}
	operator T() { _updateCount = _shaderRefCount; return _value; }
	void operator=(T v_) { _updateCount = _shaderRefCount; _value = v_; }
	void upload(GLint); 
};

