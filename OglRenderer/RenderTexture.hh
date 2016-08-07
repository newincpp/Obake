#pragma once
#include <iostream>
#include <string>
#include "glew.h"
#include <glm/glm.hpp>

class RenderTexture {
    public:
	GLuint _id;
	GLuint _attachment;
	std::string _name;
	RenderTexture(unsigned short attachment_, std::string&&, GLint, glm::vec2);
	void bind(unsigned int i_);
	inline GLuint getAttachment() {
	    return _attachment;
	}
	inline GLuint getId() {
	    return _id;
	}
};
