#include "Uniform.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
    private:
	Uniform<glm::mat4> uCamera;
	glm::vec3 _target;
	glm::vec3 _position;
	glm::mat4 _view;
	glm::mat4 _proj;
    public:
	Camera();
	void lookAt(glm::vec3 target_);
	void setPos(glm::vec3 newPos_);
	void refresh();
};
