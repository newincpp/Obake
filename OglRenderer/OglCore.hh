#pragma once
#include <vector>
#include <iostream>
#include "shader.hh"
#include "glew.h"
#include <chrono>
#include "Uniform.hh"
#include "Importer.hh"
#include "Camera.hh"

#define checkGlError getGlError(__FILE__, __LINE__);
void getGlError(const char* file_, unsigned long line_);

class RenderTexture {
    public:
	GLuint _id;
	GLuint _attachment;
	RenderTexture(unsigned short attachment_) {
	    if (attachment_ > 15) {
		std::cout << "opengl Does not support more than 15 framebuffer" << std::endl;
		return;
	    }
	    glGenTextures(1, &_id);
	    glBindTexture(GL_TEXTURE_2D, _id);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_ + GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _id, 0);
	    _attachment = attachment_ + GL_COLOR_ATTACHMENT0;
	}
	void bind(unsigned int i_) {
	    glActiveTexture(GL_TEXTURE0 + i_);
	    glBindTexture(GL_TEXTURE_2D, _id);
	    std::cout << "i: " << i_ + 4 << " " << i_ << std::endl;
	    glUniform1i(i_ + 4, i_); 
	}
	GLuint getAttachment() {
	    return _attachment;
	}
	GLuint getId() {
	    return _id;
	}
};

class FrameBuffer {
    public:
	GLuint _fbo;
	GLuint _rbo;

	std::vector<RenderTexture> _rtt;
    	FrameBuffer()  {
	}
	void init() {
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenRenderbuffers(1, &_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo); 
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);  
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
	}
	void enable() {
	    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	    if (_rtt.size()) {
		GLuint attachments[_rtt.size()];
		for (unsigned short i = 0; i < _rtt.size() ; ++i) {
		    attachments[i] = _rtt[i].getAttachment();
		}
		glDrawBuffers(_rtt.size(), attachments);
	    }
	}
	void addBuffer() {
	    _rtt.emplace_back(_rtt.size());
	}
	void bindGBuffer() {
	    unsigned int i = 0;
	    for (RenderTexture& rtt: _rtt) {
		rtt.bind(i);
		++i;
	    }
	}
	void disable () {
	    glBindRenderbuffer(GL_RENDERBUFFER, 0);
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
	Uniform<int> _uPostPRocessTexture;
	Shader _sgBuffer;
	Shader _srender;
	Shader _sPostProc;
	std::vector<Mesh> _scene;
	Mesh _renderTarget;
	Camera c;
	FrameBuffer _gBuffer;
    public:
	OglCore() : uTime(1, 0.0f), _uPostPRocessTexture(2) { }
	void init();
	void import(std::string);
	void renderScene();
	unsigned long render();
};

