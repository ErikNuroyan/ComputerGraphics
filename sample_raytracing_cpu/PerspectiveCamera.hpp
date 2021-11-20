//
//  PerspectiveCamera.hpp
//  Test
//
//  Created by Erik Nouroyan on 20.11.21.
//

#ifndef PerspectiveCamera_hpp
#define PerspectiveCamera_hpp

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

class PerspectiveCamera {
public:
    PerspectiveCamera();
    PerspectiveCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov = 90.f, int width = 800, int height = 600);
    glm::mat4 getInverseCameraMatrix() const;
    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition() const;
    void setFront(const glm::vec3& front);
    glm::vec3 getFront() const;
    void setUp(const glm::vec3& up);
    glm::vec3 getUp() const;
    void setFOV(float fov);
    float getFOV() const;
    void setWidth(int w);
    int getWidth() const;
    void setHeight(int h);
    int getHeight() const;
    
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float fov;
    int width;
    int height;
};

#endif /* PerspectiveCamera_hpp */
