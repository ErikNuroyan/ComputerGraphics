//
//  PerspectiveCamera.cpp
//  Test
//
//  Created by Erik Nouroyan on 20.11.21.
//

#include "PerspectiveCamera.hpp"

PerspectiveCamera::PerspectiveCamera() {
    position = {0.f, 0.f, 0.f};
    front = {0.f, 0.f, -1.0f};
    up = {0.f, 1.0f, 0.f};
    fov = 90.f;
    width = 800;
    height = 600;
}

PerspectiveCamera::PerspectiveCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, int width, int height) :
                                                        position(pos), front(front), up(up), fov(fov), width(width), height(height) {}

glm::mat4 PerspectiveCamera::getInverseCameraMatrix() const {
    return glm::inverse(glm::lookAt(position, position + front, up));
}

void PerspectiveCamera::setPosition(const glm::vec3& pos) {
    position = pos;
}

glm::vec3 PerspectiveCamera::getPosition() const {
    return position;
}

void PerspectiveCamera::setFront(const glm::vec3& front) {
    this->front = front;
}

glm::vec3 PerspectiveCamera::getFront() const {
    return front;
}

void PerspectiveCamera::setUp(const glm::vec3& up) {
    this->up = up;
}

glm::vec3 PerspectiveCamera::getUp() const {
    return up;
}

void PerspectiveCamera::setFOV(float fov) {
    this->fov = fov;
}

float PerspectiveCamera::getFOV() const {
    return fov;
}
void PerspectiveCamera::setWidth(int w) {
    width = w;
}

int PerspectiveCamera::getWidth() const {
    return width;
}

void PerspectiveCamera::setHeight(int h) {
    height = h;
}

int PerspectiveCamera::getHeight() const {
    return height;
}
