#ifndef DISPARITY_H
#define DISPARITY_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <algorithm> 
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//计算坏点率
void Evaluate(Mat standard, Mat myMap);

//三通道彩色图像转成单通道灰度图, type表示不同的转换方式，其实优化效果并不大
Mat turnIntoGray(Mat input, int type);

//SSD实现
Mat SSD(Mat Left, Mat Right, bool LeftOrRight, int size);

//NCC实现
Mat NCC(Mat Left, Mat Right, bool LeftOrRight, int size);

//增加灰度图的强度
Mat addIntensity(Mat original, int add);

//ASW实现
Mat ASW(Mat Left, Mat Right, bool LeftOrRight, int size, int rc, int rp);

#endif