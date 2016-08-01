#pragma once
#include <vector>
#include <iostream>
#include "shader.hh"
#include "glew.h"
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

class RenderTexture {
    public:
	GLuint id;
	RenderTexture(unsigned short attachment_) {
	    if (attachment_ > 15) {
		std::cout << "opengl Does not support more than 15 framebuffer" << std::endl;
		return;
	    }
	    glGenTextures(1, &id);
	    glBindTexture(GL_TEXTURE_2D, id);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_ + GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
	}
	GLuint getAttachment() {
	    return id + GL_COLOR_ATTACHMENT0;
	}
};

class FrameBuffer {
    public:
	GLuint _fbo;
	std::vector<RenderTexture> _rtt;
    	FrameBuffer() : _fbo(0) {
	}
	void enable() {
	    if (!_fbo) {
		glGenFramebuffers(1, &_fbo);
	    }
	    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	    if (_rtt.size()) {
		GLuint attachments[_rtt.size()];
		for (unsigned short i = 0; i < _rtt.size() ; ++i) {
		    attachments[i] = _rtt[i].getAttachment();
		}
		glDrawBuffers(_rtt.size(), attachments);
	    }
	    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "\033[32mframeBuffer valid \\o/\033[0m" << '\n';
	    }
	}
	void addBuffer() {
	    _rtt.emplace_back(_rtt.size());
	}
	void disable () {
	    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	}
	~FrameBuffer() {
	    glDeleteFramebuffers(1, &_fbo);
	}
};

class OglCore {
    private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _beginTime;
	Uniform<float> uTime;
	Shader _sgBuffer;
	Shader _srender;
	std::vector<Mesh> _scene;
	Camera c;
	FrameBuffer gBuffer;
    public:
	OglCore() : uTime(1, 0.0f) { }
	void init();
	void import(std::string);
	void renderScene();
	inline void getGlError(const char* file_, unsigned long line_);
	unsigned long render();
};
