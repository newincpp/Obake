#include <vector>
#include "glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Uniform.hh"

template<typename InputType_>
struct __remove_reference__ {
    typedef InputType_ type;
};
template<typename InputType_>
struct __remove_reference__<InputType_&> {
    typedef InputType_ type;
};
template<typename InputType_>
struct __remove_reference__<InputType_&&> {
    typedef InputType_ type;
};


class Mesh {
    private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
	int size;
    public:
	Uniform<glm::mat4> uMeshTransform;
	std::string _name;
	Mesh();
	void uploadToGPU(std::vector<GLfloat>& vbo_, std::vector<GLuint>& ebo_);
	void render();
};
