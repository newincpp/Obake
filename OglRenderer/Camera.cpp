#include <iostream>
#include "Camera.hh"

Camera::Camera() : 
    uCamera(2, glm::mat4(1.0f)), 
    _target(glm::vec3(0.0f, 0.0f, 0.0f)),
    _position(glm::vec3(15.0f, 5.2f, 15.0f)),
    _upVector(glm::vec3(0.0f, 1.0f, 0.0f)),
    _fov(1.745f),
    _clipPlane(0.1f, 1000.0f) {

    _gBuffer.addBuffer("gPosition");
    _gBuffer.addBuffer("gNormal");
    _gBuffer.addBuffer("gAlbedoSpec");
    _gBuffer.addDepthBuffer("gDepth");
    _gBuffer.enable();
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
    std::cout << "targ: " << _target[0] << ' ' << _target[1] << ' ' << _target[2] << '\n';
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::setPos(glm::vec3&& newPos_) {
    _position = newPos_;
    std::cout << "pos: " << _position[0] << ' ' << _position[1] << ' ' << _position[2] << '\n';
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::use() {
    _gBuffer.enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
    autoRelocate(uCamera);
    uCamera.upload();
}
void Camera::unUse() {
    _gBuffer.disable();
    _gBuffer.bindGBuffer();
}
void Camera::fieldOfview(float fov_) {
    _fov = fov_;
    std::cout << "fov: " << _fov << '\n';
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::clipPlane(glm::vec2&& clip_) {
    _clipPlane = clip_;
    std::cout << "clip: " << _clipPlane[0] << ' ' << _clipPlane[1] << '\n';
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::upVector(glm::vec3&& up_) {
    _upVector = glm::normalize(up_);
    std::cout << "up: " << _upVector[0] << ' ' << _upVector[1] << ' ' << _upVector[2] << '\n';
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
