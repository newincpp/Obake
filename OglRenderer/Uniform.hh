#pragma once
#include "glew.h"

template <typename T>
class Uniform {
    public:
	GLint _location;
	T _value;
    	explicit Uniform(T v_) : _value(v_) {}
    	explicit Uniform(GLint l_, T v_) : _location(l_), _value(v_) {}
	operator T() { return _value; }
	void operator=(T v_) { _value = v_; }
	void upload(); 
};

