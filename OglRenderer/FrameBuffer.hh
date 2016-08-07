#pragma once
#include <vector>
#include "RenderTexture.hh"

class FrameBuffer {
    private:
	GLuint _fbo;
	GLuint _rbo;
    public:
	std::vector<RenderTexture> _rtt;

    	FrameBuffer();
	void init();
	void enable();
	void addBuffer(std::string&&, GLint = GL_RGB, glm::vec2 = glm::vec2(1920, 1080));
	void bindGBuffer();
	void disable();
	~FrameBuffer();
};
