#include <iostream>
#include <opencv2/opencv.hpp>

struct Point {
    int x;
    int y;
};

struct Context {
    cv::Mat* img;
    Point* points;
};

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
        if (d <= 0) {
            d += 2 * dy;
        }
        else {
            d += (2 * dy - 2 * dx);
            yCurr += sign;
        }
        if (swapped) {
            data[xCurr * iStep + yCurr] = 255;
        }
        else {
            data[yCurr * iStep + xCurr] = 255;
        }
    }
    cv::imshow("MyWindow", img);
}

void MouseCallBack(int event, int x, int y, int flags, void* userdata)
{
    static unsigned int counter = 0;
    if (event == cv::EVENT_LBUTTONDOWN) {
        Context* ctx = static_cast<Context*>(userdata);
        ctx->points[counter % 2] = Point{x, y};
        if (counter % 2) BresenhamLineDraw(*(ctx->img), ctx->points[0], ctx->points[1]);
        ++counter;
    }
}

int main(int argc, const char * argv[]) {
    Point points[2];
    cv::Mat img = cv::Mat::zeros(1200, 1200, CV_8UC1);
    Context ctx = {&img, points};
    
    cv::namedWindow("MyWindow");
    cv::setMouseCallback("MyWindow", MouseCallBack, &ctx);
    cv::imshow("MyWindow", img);
    cv::waitKey(0);
    
    return 0;
}
