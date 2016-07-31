#include "Camera.hh"

Camera::Camera() : uCamera(2, glm::mat4(1.0f)), _view(glm::lookAt( glm::vec3(15.0f, 5.2f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))), _proj(glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f)) {
}
void Camera::lookAt(glm::vec3 target_) {
    _target = target_;
    _view = glm::lookAt( glm::vec3(1.2f, 1.2f, 1.2f), target_, glm::vec3(0.0f, 1.0f, 0.0f));
}
void Camera::setPos(glm::vec3 newPos_) {
    _position = newPos_;
    _view = glm::lookAt( newPos_, _target, glm::vec3(0.0f, 1.0f, 0.0f));
}
void Camera::refresh() {
    uCamera = _proj * _view;
    uCamera.upload();
}
