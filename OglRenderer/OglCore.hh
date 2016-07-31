#pragma once
#include <vector>
#include <iostream>
#include "shader.hh"
#define GLEW_STATIC
#include <GL/glew.h>
#include <chrono>
#include "Uniform.hh"
#include "Importer.hh"
#include "Camera.hh"

class log {
    public:
	void print(const char* msg_) {
	    std::cout << msg_ << '\n';
	}
};

#define checkGlError getGlError(__FILE__, __LINE__)

class OglCore {
    private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _beginTime;
	Uniform<float> uTime;
	Shader _base;
	std::vector<Mesh> _scene;
	Camera c;
    public:
	OglCore() : uTime(1, 0.0f) { }
	void init();
	void import(std::string);
	void renderScene();
	inline void getGlError(const char* file_, unsigned long line_);
	unsigned long render();
};
