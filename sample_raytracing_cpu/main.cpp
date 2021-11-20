//
//  main.cpp
//  Test
//
//  Created by Erik Nouroyan on 19.11.21.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "Triangle.hpp"
#include "PerspectiveCamera.hpp"

Ray constructRayThroughPixel(const PerspectiveCamera& camera, int i, int j) {
    const int width = camera.getWidth();
    const int height = camera.getHeight();
    
    const float xNDC = (j + 0.5f) / width;
    const float yNDC = (i + 0.5f) / height;
    
    const float screenX = 2 * xNDC - 1;
    const float screenY = 1 - 2 * yNDC;
    
    const float aspectRatio = static_cast<float>(width) / height;
    const float cameraX = screenX * aspectRatio * tan(glm::radians(camera.getFOV() / 2));
    const float cameraY = screenY * tan(glm::radians(camera.getFOV() / 2));
    
    glm::mat4 inverseCameraMatrix = camera.getInverseCameraMatrix();
    glm::vec3 camPos = camera.getPosition();
    glm::vec3 dir = inverseCameraMatrix * glm::vec4(cameraX, cameraY, -1.f, 1.f) - glm::vec4(camPos, 1.0);
    
    return {camPos, glm::normalize(dir)};
}

int getSceneIntersection (const Ray& r, const std::vector<Triangle>& scene) {
    float t = MAXFLOAT;
    int minIndex = INT_MAX;
    for (int i = 0; i < scene.size(); ++i) {
        float tTmp = t;
        if (scene[i].intersects(r, tTmp) && tTmp < t) {
            t = tTmp;
            minIndex = i;
        }
    }
    
    return minIndex;
}

cv::Mat rayTracing(const PerspectiveCamera& cam, const std::vector<Triangle>& scene) {
    const int width = cam.getWidth();
    const int height = cam.getHeight();
    cv::Mat frame = cv::Mat::zeros(height, width, CV_8UC3);
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Ray ray = constructRayThroughPixel(cam, i, j);
            int minTriangleIndex = getSceneIntersection(ray, scene);
            if (minTriangleIndex != INT_MAX) {
                frame.at<cv::Vec3b>(i, j) = scene[minTriangleIndex].getColor();
            }
        }
    }
    
    return frame;
}

int main() {
    const int width = 800;
    const int height = 600;
    
    std::vector<Triangle> scene;
    Triangle t1;
    scene.push_back(t1);
    
    Triangle t2;
    t2.setVertices({{1.f, -1.f, -5.f}, {-1.f, -1.f, -5.f}, {0.f, 0.3f, -5.f}});
    t2.setColor({255, 0, 0});
    scene.push_back(t2);
    
    Triangle t3;
    t3.setVertices({{2.f, -0.3f, -8.f}, {-2.f, -0.3f, -8.f}, {0.f, 4.f, -8.f}});
    t3.setColor({0, 255, 0});
    scene.push_back(t3);
    
    PerspectiveCamera pc;
    pc.setPosition(glm::vec3(1.f, 0.f, 2.f));
    cv::Mat frame = rayTracing(pc, scene);
    cv::imshow("MyWind", frame);
    cv::waitKey(0);
    
    return 0;
}
