//
//  main.cpp
//  MyProjectOpenCV
//
//  Created by Erik Nuroyan on 6/20/20.
//  Copyright © 2020 Erik Nuroyan. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <initializer_list>
#include <list>
#include <vector>

struct Point {
    float x;
    float y;
    float z;
    float w;
};

inline bool inRange(int x, int y, int width, int height) {
    return (x >= 0 && x < width) && (y >= 0 && y < height);
}

void BresenhamLineDraw(cv::Mat& img, const Point& p1, const Point& p2) {
    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);
    
    int xCurr = MIN(p1.x, p2.x);
    int p1x = static_cast<int>(p1.x);
    int yCurr = (xCurr == p1x ? p1.y : p2.y);
    int xEnd = (xCurr == p1x ? p2.x : p1.x);
    int yEnd = (xCurr == p1x ? p2.y : p1.y);
    
    int sign = 1;
    if (yEnd - yCurr < 0) {
        sign = -1;
    }
    
    bool swapped = false;
    if (dx < dy) {
        std::swap(xCurr, yCurr);
        std::swap(xEnd, yEnd);
        std::swap(dx, dy);
        swapped = true;
    }
    
    if (sign == -1 && swapped) {
        std::swap(xCurr, xEnd);
        std::swap(yCurr, yEnd);
    }
    
    unsigned char* data = img.data;
    const int iStep = img.step;
    int d = 2 * dy - dx;
    for(; xCurr <= xEnd; ++xCurr) {
        if (swapped) {
            if (inRange(yCurr, xCurr, img.cols, img.rows)) {
                data[xCurr * iStep + yCurr] = 255;
            }
        }
        else {
            if (inRange(xCurr, yCurr, img.cols, img.rows)) {
                data[yCurr * iStep + xCurr] = 255;
            }
        }
        if (d <= 0) {
            d += 2 * dy;
        }
        else {
            d += (2 * dy - 2 * dx);
            yCurr += sign;
        }
    }
}

//Utility Functions
float* getTranslationMatrix(float tx, float ty, float tz) {
    float* translationMatrix = static_cast<float*>(malloc(16 * sizeof(float)));
    translationMatrix[0] = 1.0;
    translationMatrix[1] = 0.0;
    translationMatrix[2] = 0.0;
    translationMatrix[3] = tx;
    
    translationMatrix[4] = 0.0;
    translationMatrix[5] = 1.0;
    translationMatrix[6] = 0.0;
    translationMatrix[7] = ty;
    
    translationMatrix[8] = 0.0;
    translationMatrix[9] = 0.0;
    translationMatrix[10] = 1.0;
    translationMatrix[11] = tz;
    
    translationMatrix[12] = 0.0;
    translationMatrix[13] = 0.0;
    translationMatrix[14] = 0.0;
    translationMatrix[15] = 1.0;
    
    return translationMatrix;
}

float* getYRotationMatrix(float angle) {
    float* rotationMatrix = static_cast<float*>(malloc(16 * sizeof(float)));
    rotationMatrix[0] = cos(angle);
    rotationMatrix[1] = 0.0;
    rotationMatrix[2] = sin(angle);
    rotationMatrix[3] = 0.0;
    
    rotationMatrix[4] = 0.0;
    rotationMatrix[5] = 1.0;
    rotationMatrix[6] = 0.0;
    rotationMatrix[7] = 0.0;
    
    rotationMatrix[8] = -sin(angle);
    rotationMatrix[9] = 0.0;
    rotationMatrix[10] = cos(angle);
    rotationMatrix[11] = 0.0;
    
    rotationMatrix[12] = 0.0;
    rotationMatrix[13] = 0.0;
    rotationMatrix[14] = 0.0;
    rotationMatrix[15] = 1.0;
    
    return rotationMatrix;
}

float* getProjectionMatrix(float n, float f, float aspectRatio, float fov = 90) {
    float* projectionMatrix = static_cast<float*>(malloc(16 * sizeof(float)));
    float fovRadian = fov * M_PI / 180;
    projectionMatrix[0] = 1 / (tan(fovRadian / 2) * aspectRatio);
    projectionMatrix[1] = 0.0;
    projectionMatrix[2] = 0.0;
    projectionMatrix[3] = 0.0;
    
    projectionMatrix[4] = 0.0;
    projectionMatrix[5] = 1 / tan(fovRadian / 2);
    projectionMatrix[6] = 0.0;
    projectionMatrix[7] = 0.0;
    
    projectionMatrix[8] = 0.0;
    projectionMatrix[9] = 0.0;
    projectionMatrix[10] = -(f + n) / (f - n);
    projectionMatrix[11] = -2 * f * n / (f - n);
    
    projectionMatrix[12] = 0.0;
    projectionMatrix[13] = 0.0;
    projectionMatrix[14] = -1.0;
    projectionMatrix[15] = 0.0;
    
    return projectionMatrix;
}


Point matMul(const float* mat, const Point& p) {
    //Assumes the matrix is 4x4 and the point is 4x1
    Point result {0, 0, 0 ,0};
    float* resData = reinterpret_cast<float*>(&result);
    const float* pData = reinterpret_cast<const float*>(&p);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            resData[i] += mat[i * 4 + j] * pData[j];
        }
    }
    return result;
}

float* matMul(const float* mat1, const float* mat2) {
    //Assumes the matrices are 4x4
    float* result = static_cast<float*>(calloc(16 * sizeof(float), 1));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result[i * 4 + j] += mat1[i * 4 + k] * mat2[k * 4 + j];
            }
        }
    }
    
    return result;
}

class Polygon {
    
public:
    Polygon(std::initializer_list<Point> il) {
        vertices.insert(vertices.end(), il);
    }
    
    std::vector<Point> getVertices() const {
        return vertices;
    }
    
    void print() const {
        for (const Point& p : vertices) {
            std::cout << p.x << " " << p.y << " " << p.z << " " << p.w << std::endl;
        }
    }
    
private:
    std::vector<Point> vertices;
};

class Cube {
    
public:
    Cube(std::initializer_list<Polygon> il) {
        polygons.insert(polygons.end(), il);
        modelMatrix = getTranslationMatrix(0, 0, 5);
    }
    
    const float* getModelMatrix() const {
        return modelMatrix;
    }
    
    std::vector<Polygon> getPolygons() const {
        return polygons;
    }
    
    void rotate(bool counterClockwise = true, float angle = 3) {
        float angleRadian = angle * M_PI / 180;
        if (!counterClockwise) angleRadian = -angleRadian;
        float* rotationMatrix = getYRotationMatrix(angleRadian);
        float* result = matMul(modelMatrix,rotationMatrix);
        free(modelMatrix);
        modelMatrix = result;
    }
    
    void print() const {
        int counter = 1;
        for (const Polygon& p : polygons) {
            std::cout << "Surface " << counter << std::endl;
            p.print();
            ++counter;
        }
    }
    
    ~Cube() {
        free(modelMatrix);
    }
    
private:
    std::vector<Polygon> polygons;
    float* modelMatrix;
};

class Camera {
public:
    Camera(float aspectRatio) : aspectRatio(aspectRatio) {
        viewMatrix = getTranslationMatrix(0.0, 0.0, 0.0);
        FOV = 90;
        projectionMatrix = getProjectionMatrix(1.0, 100.0, aspectRatio, FOV);
    }
    
    const float* getViewMatrix() const {
        return viewMatrix;
    }
    
    const float* getProjMatrix() const {
        return projectionMatrix;
    }
    
    void translate(float tz) {
        const float* translationMatrix = getTranslationMatrix(0, 0, tz);
        float* result = matMul(translationMatrix,viewMatrix);
        free(viewMatrix);
        viewMatrix = result;
    }
    
    void changeFOV(bool increase = true) {
        increase ? ++FOV : --FOV;
        free(projectionMatrix);
        projectionMatrix = getProjectionMatrix(1.0, 100.0, aspectRatio, FOV);
    }
    
    ~Camera() {
        free(viewMatrix);
        free(projectionMatrix);
    }
    
private:
    float* viewMatrix;
    float* projectionMatrix;
    float FOV;
    const float aspectRatio;
};

void normalizeCoordinates(std::vector<Point>& vertices) {
    for (Point& p : vertices) {
        p.x /= p.w;
        p.y /= p.w;
        p.z /= p.w;
        p.w /= p.w;
    }
}

void viewPortTransform(std::vector<Point>& vertices, int width, int height) {
    for (Point& p : vertices) {
        p.x = p.x * width / 2 + width / 2;
        p.y = -p.y * height / 2 + height / 2;
    }
}

void drawPolygon(const std::vector<Point>& vertices, cv::Mat& img) {
    const int size = vertices.size();
    for (int i = 0; i < size; ++i) {
        BresenhamLineDraw(img, vertices[i], vertices[(i + 1) % size]);
    }
}

void pipeline(const Cube& cube, const Camera& camera, cv::Mat& img, int width, int height) {
    const float* projMatrix = camera.getProjMatrix();
    const float* viewMatrix = camera.getViewMatrix();
    const float* modelMatrix = cube.getModelMatrix();
    
    float* tmp1 = matMul(projMatrix, viewMatrix);
    float* result = matMul(tmp1, modelMatrix);
    free(tmp1);
    
    std::vector<Polygon> polygons = cube.getPolygons();
    img.setTo(cv::Scalar(0));
    for (Polygon& p : polygons) {
        std::vector<Point>&& vertices = p.getVertices();
        for (Point& point: vertices) {
            point = matMul(result, point);
        }
        normalizeCoordinates(vertices);
        viewPortTransform(vertices, width, height);
        drawPolygon(vertices, img);
    }
    imshow("MyWindow", img);
}

int main(int argc, const char * argv[]) {
    const int width = 900;
    const int height = 900;
    cv::Mat img = cv::Mat::zeros(height, width, CV_8UC1);
    
    cv::namedWindow("MyWindow");
    
    Cube c {
        {{1,-1,1,1}, {-1,-1,1,1}, {-1,1,1,1}, {1,1,1,1}}, //surface 1
        {{-1,-1,1,1}, {-1,1,1,1}, {-1,1,-1,1}, {-1,-1,-1,1}}, //surface 2
        {{-1,-1,-1,1}, {-1,1,-1,1}, {1,1,-1,1}, {1,-1,-1,1}}, //surface 3
        {{1,1,-1,1}, {1,-1,-1,1}, {1,-1,1,1}, {1,1,1,1}}, //surface 4
        {{1,1,1,1}, {-1,1,1,1}, {-1,1,-1,1}, {1,1,-1,1}}, //surface 5
        {{1,-1,1,1}, {-1,-1,1,1}, {-1,-1,-1,1}, {1,-1,-1,1}} //surface 6
    };
    
    float aspectRatio = static_cast<float>(img.cols) / img.rows;
    Camera cam(aspectRatio);
    pipeline(c, cam, img, width, height);
    while (int k = cv::waitKeyEx(0)) {
        if (k == 'i') {
            cam.changeFOV();
            pipeline(c, cam, img, width, height);
        }
        else if (k == 'd') {
            cam.changeFOV(false);
            pipeline(c, cam, img, width, height);
        }
        else if (k == 'r') {
            c.rotate();
            pipeline(c, cam, img, width, height);
        }
        else if (k == 't') {
            c.rotate(false);
            pipeline(c, cam, img, width, height);
        }
        else if (k == 63232) { //up arrow is pressed
            cam.translate(-0.2);
            pipeline(c, cam, img, width, height);
        }
        else if (k == 63233) { //down arrow is pressed
            cam.translate(0.2);
            pipeline(c, cam, img, width, height);
        }
        else if (k == 27) { //ESC is pressed
            break;
        }
    }

    return 0;
}
