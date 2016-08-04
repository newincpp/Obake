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
    //std::cout << _target.x << ' ' << _target.y << ' '<< _target.z << '\n';
    //std::cout << _position.x << ' ' << _position.y << ' ' << _position.z << '\n';
    //std::cout << _fov << '\n';
    //std::cout << _clipPlane.x << ' ' << _clipPlane.y << '\n';
    //std::cout << _upVector.x << ' ' << _upVector.y << ' ' << _upVector.z << '\n';
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
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
    _upVector = up_;
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
