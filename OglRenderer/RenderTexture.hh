#pragma once
#include <iostream>
#include <string>
#include <utility>
#include "glew.h"
#include <glm/glm.hpp>

template <GLuint MODE>
class RenderTexture {
    private:
    	GLenum _genttachmentEnum(unsigned short);
    public:
	GLuint _id;
	GLuint _attachment;
	std::string _name;
	RenderTexture();
	~RenderTexture();
	RenderTexture(unsigned short attachment_, std::string&&, glm::vec2 resolution_ = glm::vec2(1920, 1080));
	void init(unsigned short attachment_, std::string&&, glm::vec2 resolution_ = glm::vec2(1920, 1080));
	void bind(unsigned int i_);
	inline GLuint getAttachment() {
	    return _attachment;
	}
	inline GLuint getId() {
	    return _id;
	}
};

template <GLuint MODE>
RenderTexture<MODE>::RenderTexture() : _name("") {
}

template <GLuint MODE>
RenderTexture<MODE>::~RenderTexture() {
    glDeleteTextures(1, &_id);
}

template <GLuint MODE>
RenderTexture<MODE>::RenderTexture(unsigned short attachment_, std::string&& name_, glm::vec2 resolution_) : _name(name_) {
    init(attachment_, std::move(name_), resolution_);
}

template <GLuint MODE>
void RenderTexture<MODE>::init(unsigned short attachment_, std::string&& name_, glm::vec2 resolution_) {
    _name = std::move(name_);
    if (attachment_ > 15) {
	std::cout << "opengl Does not support more than 15 framebuffer" << std::endl;
	return;
    }
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, MODE, resolution_.x, resolution_.y, 0, MODE, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    _attachment = _genttachmentEnum(attachment_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, _id, 0);
}

template <GLuint MODE>
void RenderTexture<MODE>::bind(unsigned int i_) {
    glActiveTexture(GL_TEXTURE0 + i_);
    glBindTexture(GL_TEXTURE_2D, _id);

    GLint programId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
    glUniform1i(glGetUniformLocation(programId, _name.c_str()), i_);
}
