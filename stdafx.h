#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <math.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"

#define PI 3.1415926535897932

// define a few colors
#define FP_COLOR_RED cv::Scalar(0,0,255)
#define FP_COLOR_GREEN cv::Scalar(0,255,0)
#define FP_COLOR_BLUE cv::Scalar(255,0,255)
#define FP_COLOR_PURPLE cv::Scalar(255,0,255)
#define FP_COLOR_CYAN cv::Scalar(255,255,0)
#define FP_COLOR_YELLOW cv::Scalar(0,255,255)
#define FP_COLOR_DARKGREEN cv::Scalar(0,128,0)
#define FP_COLOR_LIGHTBLUE cv::Scalar(255,128,128)
#define FP_COLOR_PINK cv::Scalar(255,128,255)
#define FP_COLOR_WHITE cv::Scalar(255,255,255)
#define FP_COLOR_BLACK cv::Scalar(0,0,0)

// define several skin-segmentation methods
enum SkinSegmMethod {
	SKIN_SEGMENT_ADAPTIVE = 0,
	SKIN_SEGMENT_YCRCB = 1,
	SKIN_SEGMENT_HSV = 2
};

// hand movement directions
enum MovementDirection {
	MOVEMENT_UP = 0,
	MOVEMENT_DOWN,
	MOVEMENT_LEFT,
	MOVEMENT_RIGHT
};

const std::string DirectionStrings[] = { "UP", "DOWN", "LEFT", "RIGHT" };

// array of colors
const cv::Scalar fpColors[] = { cv::Scalar(0,0,255), cv::Scalar(0,255,0), cv::Scalar(255,0,0), 
								cv::Scalar(255,0,255), cv::Scalar(255,255,0), cv::Scalar(0,255,255), 
								cv::Scalar(0,128,0), cv::Scalar(255,128,128), cv::Scalar(255,128,255), 
								cv::Scalar(255,255,255)};

// access colors with these names
enum Colors {
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_CYAN,
    COLOR_YELLOW,
    COLOR_DARKGREEN,
    COLOR_LIGHTBLUE,
    COLOR_PINK,
    COLOR_WHITE
};

// a list of hand-postures
enum HandPostures {
	HAND_POSTURE_ONE = 0,
	HAND_POSTURE_TWO,
	HAND_POSTURE_V,
	HAND_POSTURE_THREE,
	HAND_POSTURE_SPOCK,
	HAND_POSTURE_FOUR,
	HAND_POSTURE_FIVE,
	HAND_POSTURE_THUMBS_UP,
	HAND_POSTURE_FIST
};

struct YCbCrBounds {
	int Y_MIN,  Y_MAX;
	int Cr_MIN, Cr_MAX;
	int Cb_MIN, Cb_MAX;

	YCbCrBounds() : Y_MIN(-1),  Y_MAX(-1),
					Cr_MIN(-1), Cr_MAX(-1),
					Cb_MIN(-1), Cb_MAX(-1) {};
};

struct HsvBounds {
	int H_MIN, H_MAX;
	int S_MIN, S_MAX;
	int V_MIN, V_MAX;

	HsvBounds() :	H_MIN(-1), H_MAX(-1),
					S_MIN(-1), S_MAX(-1),
					V_MIN(-1), V_MAX(-1) {};
};

// crop a rectangle from an image with no problems
void cropRoi(const cv::Mat image, cv::Mat& roi, cv::Rect& rect);