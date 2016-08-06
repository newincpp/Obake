#include <iostream>
#include "Camera.hh"

Camera::Camera() : 
    uCamera(2, glm::mat4(1.0f)), 
    _target(glm::vec3(0.0f, 0.0f, 0.0f)),
    _position(glm::vec3(15.0f, 5.2f, 15.0f)),
    _upVector(glm::vec3(0.0f, 1.0f, 0.0f)),
    _fov(1.745f),
    _clipPlane(0.1f, 1000.0f) {
}

void Camera::setMatrix(glm::mat4&& m_) {
    uCamera = m_;
    glm::vec4 p = m_ * glm::vec4(0.0f,0.0f,0.0f,1.0f) ;
    std::cout << "pos: " << p[0] << ' ' << p[1] << ' ' << p[2] << '\n';
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * m_;
}

void Camera::lookAt(glm::vec3&& target_) {
    _target = target_;
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::setPos(glm::vec3&& newPos_) {
    _position = newPos_;
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::refresh() {
	autoRelocate(uCamera);
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
    uCamera.upload();
}
void Camera::fieldOfview(float fov_) {
    _fov = fov_;
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::clipPlane(glm::vec2&& clip_) {
    _clipPlane = clip_;
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::upVector(glm::vec3&& up_) {
    _upVector = glm::normalize(up_);
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
