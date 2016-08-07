#include "FrameBuffer.hh"
#include "OglCore.hh"

FrameBuffer::FrameBuffer()  {
}
void FrameBuffer::init() {
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    /*glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);*/
}
void FrameBuffer::enable() {
    //glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    if (_rtt.size()) {
	GLuint attachments[_rtt.size() - 1];
	for (unsigned short i = 0; i < _rtt.size() - 1; ++i) {
	    attachments[i] = _rtt[i].getAttachment();
	}
	glDrawBuffers(_rtt.size(), attachments);
	checkGlError;
    }
}
void FrameBuffer::addBuffer(std::string&& name_, GLint mode_, glm::vec2 resolution_) {
    _rtt.emplace_back(_rtt.size(), std::move(name_),mode_, resolution_);
}
void FrameBuffer::bindGBuffer() {
    unsigned int i = 0;
    for (RenderTexture& rtt: _rtt) {
	rtt.bind(i);
	++i;
    }
}
void FrameBuffer::disable() {
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &_fbo);
}
