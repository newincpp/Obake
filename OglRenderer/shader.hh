#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include "glew.h"

class Shader {
    private:
	GLuint _vertexId;
	GLuint _fragmentId;
	GLuint _geometryId;
	GLuint _programId;

	inline bool _checkValidity(GLenum id_, const char* src_, std::string& filename_)const {
	    GLint compileStatus;
	    glGetShaderiv(id_, GL_COMPILE_STATUS, &compileStatus);
	    if (compileStatus) {
		std::cout << "\033[32msuccesfully compiled\033[0m" << std::endl;
		return true;
	    }
	    GLint InfoLogLength;
	    glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &InfoLogLength);
	    char ErrorMessage[InfoLogLength];
	    glGetShaderInfoLog(id_, InfoLogLength, NULL, ErrorMessage);
	    std::cout << "\033[31mfailed to compile \"" <<  filename_ << "\"\033[0m" << std::endl <<
		"your shader:" << std::endl << 
		src_ << std::endl <<
		"error log:" << std::endl << ErrorMessage << std::endl << "-------------------\033[0m" << std::endl;
	    return false;
	}

    public:
	Shader();
	void add(std::string, GLenum);
	void link();
	inline void use() {
	    glUseProgram(_programId);
	}
};
