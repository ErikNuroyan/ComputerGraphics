//
//  main.cpp
//  MyProjectOpenCV
//
//  Created by Erik Nuroyan on 6/20/20.
//  Copyright © 2020 Erik Nuroyan. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

struct Point {
    int x;
    int y;
    int z;
};

struct Context {
    cv::Mat* img;
    std::vector<Point> points;
};

//Utility Functions
inline bool inRange(int x, int y, int width, int height) {
    return (x >= 0 && x < width) && (y >= 0 && y < height);
}

float* getTranslationMatrix(float tx, float ty) {
    float* translationMatrix = static_cast<float*>(malloc(9 * sizeof(float)));
    translationMatrix[0] = 1.0;
    translationMatrix[1] = 0.0;
    translationMatrix[2] = tx;
    translationMatrix[3] = 0.0;
    translationMatrix[4] = 1.0;
    translationMatrix[5] = ty;
    translationMatrix[6] = 0.0;
    translationMatrix[7] = 0.0;
    translationMatrix[8] = 1.0;
    
    return translationMatrix;
}

float* getScaleMatrix(float sx, float sy, const Point& scalePoint) {
    float* scaleMatrix = static_cast<float*>(malloc(9 * sizeof(float)));
    scaleMatrix[0] = sx;
    scaleMatrix[1] = 0.0;
    scaleMatrix[2] = scalePoint.x * (1 - sx);
    scaleMatrix[3] = 0.0;
    scaleMatrix[4] = sy;
    scaleMatrix[5] = scalePoint.y * (1 - sy);
    scaleMatrix[6] = 0.0;
    scaleMatrix[7] = 0.0;
    scaleMatrix[8] = 1.0;
    
    return scaleMatrix;
}

float* getRotationMatrix(float angle) {
    float* rotationMatrix = static_cast<float*>(malloc(9 * sizeof(float)));
    rotationMatrix[0] = cos(angle);
    rotationMatrix[1] = -sin(angle);
    rotationMatrix[2] = 0.0;
    rotationMatrix[3] = sin(angle);
    rotationMatrix[4] = cos(angle);
    rotationMatrix[5] = 0.0;
    rotationMatrix[6] = 0.0;
    rotationMatrix[7] = 0.0;
    rotationMatrix[8] = 1.0;
    
    return rotationMatrix;
}

Point polygonCenter(std::vector<Point>& vertices) {
    Point center {0, 0};
    for (auto& p: vertices) {
        center.x += p.x;
        center.y += p.y;
    }
    center.x /= vertices.size();
    center.y /= vertices.size();
    center.z = 1;
    
    return center;
}

Point matMul(const float* mat, const Point& p) {
    //Assumes the matrix is 3x3 and the point is 3x1
    Point result {0, 0, 0};
    int* resData = reinterpret_cast<int*>(&result);
    const int* pData = reinterpret_cast<const int*>(&p);
    for (int i = 0; i < 3; ++i) {
        resData[i] = 0;
        for (int j = 0; j < 3; ++j) {
            resData[i] += mat[i * 3 + j] * pData[j];
        }
    }
    return result;
}

float* matMul(const float* mat1, const float* mat2) {
    //Assumes the matrices are 3x3
    float* result = static_cast<float*>(calloc(9 * sizeof(float), 1));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                result[i * 3 + j] += mat1[i * 3 + k] * mat2[k * 3 + j];
            }
        }
    }
    
    return result;
}

//Drawing Functions
void BresenhamLineDraw(cv::Mat& img, const Point& p1, const Point& p2) {
    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);
    
    int xCurr = MIN(p1.x, p2.x);
    int yCurr = (xCurr == p1.x ? p1.y : p2.y);
    int xEnd = (xCurr == p1.x ? p2.x : p1.x);
    int yEnd = (xCurr == p1.x ? p2.y : p1.y);
    
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
    cv::imshow("MyWindow", img);
}

void drawPolygon(Context* ctx) {
    ctx->img->setTo(cv::Scalar(0));
    std::vector<Point>& vertices = ctx->points;
    const int size = vertices.size();
    for (int i = 0; i < size; ++i) {
        BresenhamLineDraw(*(ctx->img), vertices[i], vertices[(i + 1) % size]);
    }
    cv::imshow("MyWindow", *(ctx->img));
}

//Transformations
void translate(std::vector<Point>& vertices, Point& center, float tx, float ty) {
    float* translationMatrix = getTranslationMatrix(tx, ty);
    for (auto& p: vertices) {
        p = matMul(translationMatrix, p);
    }
    center = matMul(translationMatrix, center);
    free(translationMatrix);
}

void scale(std::vector<Point>& vertices, const Point& scalePoint, float sx, float sy) {
    float* scaleMatrix = getScaleMatrix(sx, sy, scalePoint);
    for (auto& p: vertices) {
        p = matMul(scaleMatrix, p);
    }
    free(scaleMatrix);
}

void rotate(std::vector<Point>& vertices, const Point& rotationPoint, float angle) {
    float* translation1 = getTranslationMatrix(rotationPoint.x, rotationPoint.y);
    float* rotationMatrix = getRotationMatrix(angle);
    float* tmp = matMul(translation1, rotationMatrix);
    free(translation1);
    free(rotationMatrix);
    float* translation2 = getTranslationMatrix(-rotationPoint.x, -rotationPoint.y);
    float* finalRotationMatrix = matMul(tmp, translation2);
    free(tmp);
    free(translation2);
    
    for (auto& p: vertices) {
        p = matMul(finalRotationMatrix, p);
    }
    free(finalRotationMatrix);
}

//Mouse Callback
void MouseCallBack(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN) {
        Context* ctx = static_cast<Context*>(userdata);
        if (!ctx->points.empty()) {
            Point p1 = *(ctx->points.rbegin());
            Point p2 = Point{x, y, 1};
            BresenhamLineDraw(*(ctx->img), p1, p2);
            ctx->points.push_back(p2);
        }
        else {
            ctx->points.push_back(Point{x, y, 1});
        }
    }
}

int main(int argc, const char * argv[]) {
    cv::Mat img = cv::Mat::zeros(300, 300, CV_8UC1);
    Context ctx = {&img};
    
    cv::namedWindow("MyWindow");
    cv::setMouseCallback("MyWindow", MouseCallBack, &ctx);
    cv::imshow("MyWindow", img);
    
    Point center;
    while (int k = cv::waitKeyEx(0)) {
        if (k == ' ') {
            BresenhamLineDraw(*(ctx.img), *(ctx.points.rbegin()), *(ctx.points.begin()));
            center = polygonCenter(ctx.points);
        }
        else if (k == 'e') { //Exit the program
            break;
        }
        else if (k == 's') {
            scale(ctx.points, center, 1.1, 1.1);
            drawPolygon(&ctx);
        }
        else if (k == 'd') {
            scale(ctx.points, center, 0.9, 0.9);
            drawPolygon(&ctx);
        }
        else if (k == 'r') {
            rotate(ctx.points, center, -0.087); //Rotation by 5 degree counterclockwise
            drawPolygon(&ctx);
        }
        else if (k == 't') {
            rotate(ctx.points, center, 0.087); //Rotation by 5 degree clockwise
            drawPolygon(&ctx);
        }
        else if (k == 63232) { //up arrow is pressed
            translate(ctx.points, center, 0.0, -1.0);
            drawPolygon(&ctx);
        }
        else if (k == 63233) { //down arrow is pressed
            translate(ctx.points, center, 0.0, 1.0);
            drawPolygon(&ctx);
        }
        else if (k == 63234) { //left arrow is pressed
            translate(ctx.points, center, -1.0, 0.0);
            drawPolygon(&ctx);
        }
        else if (k == 63235) { //right arrow is pressed
            translate(ctx.points, center, 1.0, 0.0);
            drawPolygon(&ctx);
        }
    }
    
    return 0;
}
