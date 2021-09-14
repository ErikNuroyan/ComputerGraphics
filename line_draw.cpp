//
//  main.cpp
//  MyProjectOpenCV
//
//  Created by Erik Nuroyan on 6/20/20.
//  Copyright © 2020 Erik Nuroyan. All rights reserved.
//


#include <iostream>
#include <opencv2/opencv.hpp>

#define EPSILLON 0.0001

struct Point {
    int x;
    int y;
};

void DDALineDraw (cv::Mat& img, const Point& p1, const Point& p2) {
    const int dx = abs(p1.x - p2.x);
    const int dy = abs(p1.y - p2.y);
    const int step = MAX(dx, dy);
    const double xInc = static_cast<double>(dx) / step;
    const double yInc = static_cast<double>(dy) / step;
    
    double xCurr = MIN(p1.x, p2.x);
    double yCurr = abs(xCurr - p1.x) < EPSILLON ? p1.y : p2.y;
    unsigned char* data = img.data;
    const int iStep = img.step;
    for (int i = 0; i < step; ++i) {
        data[(img.rows - static_cast<int>(round(yCurr))) * iStep + static_cast<int>(round(xCurr))] = 255;
        xCurr += xInc;
        yCurr += yInc;
    }
}

void BresenhamLineDraw(cv::Mat& img, const Point& p1, const Point& p2) {
    //Note: The slope of the line is between 0 and 1 exclusive
    const int dx = abs(p1.x - p2.x);
    const int dy = abs(p1.y - p2.y);
    
    int xCurr = MIN(p1.x, p2.x);
    int yCurr = (xCurr == p1.x ? p1.y : p2.y);
    const int xEnd = (xCurr == p1.x ? p2.x : p1.x);
    
    unsigned char* data = img.data;
    const int iStep = img.step;
    int d = 2 * dx - dy;
    for(;xCurr <= xEnd; ++xCurr) {
        if(d <= 0) {
            d += 2 * dy;
        }
        else {
            d += (2 * dy - 2 * dx);
            ++yCurr;
        }
        data[(img.rows - yCurr) * iStep + xCurr] = 255;
    }
}


int main(int argc, const char * argv[]) {
    const Point p1{8, 40};
    const Point p2{120, 67};
    cv::Mat img = cv::Mat::zeros(300, 300, CV_8UC1);
    
    //DDALineDraw(img, p1, p2);
    BresenhamLineDraw(img, p1, p2);
    
    cv::imshow("Line", img);
    cv::waitKey(0);
    
    return 0;
}
