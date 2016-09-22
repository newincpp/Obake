#include "Uniform.hh"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

template <>
void Uniform<float>::upload(GLint location_) {
    glUniform1f(location_, _value);
}

template <>
void Uniform<int>::upload(GLint location_) {
    glUniform1i(location_, _value);
}

template <>
void Uniform<unsigned int>::upload(GLint location_) {
    glUniform1ui(location_, _value);
}

template <>
void Uniform<glm::vec2>::upload(GLint location_) {
    glUniform2f(location_, _value.x, _value.y);
}

template <>
void Uniform<glm::vec3>::upload(GLint location_) {
    glUniform3f(location_, _value.x, _value.y, _value.z);
}

template <>
void Uniform<glm::vec4>::upload(GLint location_) {
    glUniform4f(location_, _value.x, _value.y, _value.z, _value.w);
}

template <>
void Uniform<glm::mat2>::upload(GLint location_) {
    glUniformMatrix2fv(location_, 1, GL_FALSE, glm::value_ptr(_value));
}

template <>
void Uniform<glm::mat3>::upload(GLint location_) {
    glUniformMatrix3fv(location_, 1, GL_FALSE, glm::value_ptr(_value));
}

template <>
void Uniform<glm::mat4>::upload(GLint location_) {
    glUniformMatrix4fv(location_, 1, GL_FALSE, glm::value_ptr(_value));
}
