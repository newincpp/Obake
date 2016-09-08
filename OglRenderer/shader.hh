#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <streambuf>
#include <functional>
#include "Uniform.hh"

#include "glew.h"

class Shader {
    private:
	GLuint _vertexId;
	GLuint _fragmentId;
	GLuint _geometryId;
	GLuint _programId;
	struct _VirtualUniform {
		unsigned short& updateCountRef; // link to Uniform<T>::_updateCount
		std::function<void(GLint)> update;
		GLint location;
		_VirtualUniform(unsigned short& updateCountRef_, std::function<void(GLint)>&& update_, GLint loc_) : updateCountRef(updateCountRef_), update(update_), location(loc_) {}
		_VirtualUniform(unsigned short& updateCountRef_, std::function<void(GLint)>& update_, GLint loc_) : updateCountRef(updateCountRef_), update(update_), location(loc_) {}
	};
	std::vector<_VirtualUniform> _uTable;

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
	void link(const std::vector<std::string>&& fragDataOutPut_);
	template <typename T>
		void relocateUniform(Uniform<T>&&, const char* name_);
	template <typename T>
	    void referenceUniform(Uniform<T>&, const char*);
	void updateUniform();
	inline void use() {
	    glUseProgram(_programId);
	    updateUniform();
	}
};

template <typename T>
void Shader::referenceUniform(Uniform<T>& u_, const char* name_) {
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(_programId, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
    GLint loc = glGetUniformLocation(_programId, name_);
    std::cout << "programid: " << _programId << " name:" << name_ << " location found:" << loc << std::endl;
    _uTable.emplace_back( u_._updateCount, [&u_](GLint loc_){ u_.upload(loc_); }, loc);

// Code from stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade#12611619 that get uniform value names...
// will be used to check the validity of name_
    //std::vector<GLchar> nameData(256);
    //std::vector<GLenum> properties;
    //properties.push_back(GL_NAME_LENGTH);
    //properties.push_back(GL_TYPE);
    //properties.push_back(GL_ARRAY_SIZE);
    //std::vector<GLint> values(properties.size());
    //for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    //{
    //	glGetProgramResourceiv(prog, GL_PROGRAM_INPUT, attrib, properties.size(),
    //	&properties[0], values.size(), NULL, &values[0]);
    //	
    //	nameData.resize(properties[0]); //The length of the name.
    //	glGetProgramResourceName(prog, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
    //	std::string name((char*)&nameData[0], nameData.size() - 1);
    //}

}

template <typename T>
void Shader::relocateUniform(Uniform<T>&& u_, const char* name_) {
	u_._location = glGetUniformLocation(_programId, name_);
}
