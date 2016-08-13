#pragma once
#include <vector>
#include <iostream>
#include <utility>
#include <chrono>
#include "shader.hh"
#include "glew.h"
#include "Uniform.hh"
#include "Importer.hh"
#include "Camera.hh"

#define checkGlError getGlError(__FILE__, __LINE__);
void getGlError(const char* file_, unsigned long line_);

class OglCore {
    private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _beginTime;
	Uniform<float> uTime;
	Uniform<int> _uPostPRocessTexture;
	Shader _sgBuffer;
	Shader _srender;
	Shader _sPostProc;
	std::vector<Mesh> _scene;
	Mesh _renderTarget;
	Camera* c;
    public:
	OglCore() : uTime(1, 0.0f), _uPostPRocessTexture(2) { }
	void init();
	void import(std::string);
	void renderScene();
	unsigned long render();
};

