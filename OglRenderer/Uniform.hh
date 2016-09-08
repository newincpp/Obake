#pragma once
#include <iostream>
#include "glew.h"

template <typename T>
class Uniform {
    private:
	T _value;
	unsigned short _shaderRefCount; // how many shader the uniform is referanced in
    public:
	unsigned short _updateCount; // remining update
    	explicit Uniform(T v_) : _value(v_), _shaderRefCount(0), _updateCount(0) {}
	operator T() { _updateCount = _shaderRefCount; return _value; }
	void operator=(T v_) { _updateCount = _shaderRefCount; _value = v_; }
	void upload(GLint); 
};
