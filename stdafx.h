// Forschungsproject (teil 3) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2015, Berlin

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
#define FP_COLOR_BLUE cv::Scalar(255,0,0)
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

/* possible event types */
// hand movement directions
enum MovementDirection {
	MOVEMENT_UP = 0,
	MOVEMENT_DOWN,
	MOVEMENT_LEFT,
	MOVEMENT_RIGHT,
	MOVEMENT_NONE
};

// amount of fingers
enum FingerCount {
	FINGERS_ZERO = 0,
	FINGERS_ONE,
	FINGERS_TWO,
	FINGERS_THREE,
	FINGERS_FOUR,
	FINGERS_FIVE
};

// a list of hand-postures
enum HandGesture {
	GESTURE_NONE = -1,
	GESTURE_POSTURE_FIST = 0,
	GESTURE_POSTURE_ONE = 1,
	GESTURE_POSTURE_TWO = 2,
	GESTURE_POSTURE_THREE = 3,
	GESTURE_POSTURE_FOUR = 4,
	GESTURE_POSTURE_FIVE = 5,
	GESTURE_POSTURE_V = 6,
	GESTURE_POSTURE_SPOCK = 7,
	GESTURE_POSTURE_THUMBS_UP = 8,
	GESTURE_SWIPE_UP = 9,
	GESTURE_SWIPE_DOWN = 10,
	GESTURE_SWIPE_LEFT = 11,
	GESTURE_SWIPE_RIGHT = 12,
	GESTURE_DRAWING_CIRCLE = 13,
	GESTURE_DRAWING_RECTANGLE = 14,
	GESTURE_DRAWING_STAR = 15,
	GESTURE_DRAWING_TRIANGLE = 16,
	GESTURE_DRAWING_V = 17,
	GESTURE_DRAWING_PIGTAIL = 18,
	GESTURE_DRAWING_DIAMOND = 19,
	GESTURE_DRAWING_SPIRAL = 20
};

const std::string GestureNames[] = {
	"FIST", "ONE FINGER", "TWO FINGERS", "THREE FINGERS", "FOUR FINGERS", "FIVE FINGERS",
	"VICTORY SIGN", "SPOCK", "THUMBS UP",
	"SWIPE UP", "SWIPE DOWN", "SWIPE LEFT", "SWIPE RIGHT",
	"CIRCLE", "RECTANGLE", "STAR", "TRIANGLE", "V", "PIGTAIL", "DIAMOND", "SPIRAL" };

// movement speed
enum MovementSpeed {
	SPEED_NONE = 0,
	SPEED_SLOW,
	SPEED_FAST
};

// finger angle
enum FingersAngle {
	ANGLE_CLOSE = 0,
	ANGLE_FAR,
	ANGLE_NONE
};

// array of colors
const cv::Scalar fpColors[] = {
	cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 0), cv::Scalar(255, 0, 0),
	cv::Scalar(255, 0, 255), cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 255),
	cv::Scalar(0, 128, 0), cv::Scalar(255, 128, 128), cv::Scalar(255, 128, 255),
	cv::Scalar(255, 255, 255) };

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



struct YCbCrBounds {
	int Y_MIN, Y_MAX;
	int Cr_MIN, Cr_MAX;
	int Cb_MIN, Cb_MAX;

	YCbCrBounds() : Y_MIN(-1), Y_MAX(-1),
		Cr_MIN(-1), Cr_MAX(-1),
		Cb_MIN(-1), Cb_MAX(-1) {};
};

struct HsvBounds {
	int H_MIN, H_MAX;
	int S_MIN, S_MAX;
	int V_MIN, V_MAX;

	HsvBounds() : H_MIN(-1), H_MAX(-1),
		S_MIN(-1), S_MAX(-1),
		V_MIN(-1), V_MAX(-1) {};
};

// crop a rectangle from an image with no problems
//void cropRoi(const cv::Mat image, cv::Mat& roi, cv::Rect& rect);