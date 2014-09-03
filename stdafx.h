// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

//#include "targetver.h"

#include <iostream>
//#include <tchar.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"

/*
#define COLOR_RED cv::Scalar(0,0,255)
#define COLOR_GREEN cv::Scalar(0,255,0)
#define COLOR_BLUE cv::Scalar(255,0,255)
#define COLOR_PURPLE cv::Scalar(255,0,255)
#define COLOR_CYAN cv::Scalar(255,255,0)
#define COLOR_YELLOW cv::Scalar(0,255,255)
#define COLOR_DARKGREEN cv::Scalar(0,128,0)
#define COLOR_LIGHTBLUE cv::Scalar(255,128,128)
#define COLOR_PINK cv::Scalar(255,128,255)
#define COLOR_WHITE cv::Scalar(255,255,255)
*/

enum SkinSegmMethod {
	SKIN_SEGMENT_ADAPTIVE = 0,
	SKIN_SEGMENT_YCRCB = 1,
	SKIN_SEGMENT_HSV = 2
};

enum MovementDirection {
	MOVEMENT_UP = 0,
	MOVEMENT_DOWN,
	MOVEMENT_LEFT,
	MOVEMENT_RIGHT
};

const cv::Scalar fpColors[] = { cv::Scalar(0,0,255), cv::Scalar(0,255,0), cv::Scalar(255,0,0), cv::Scalar(255,0,255), cv::Scalar(255,255,0), cv::Scalar(0,255,255), cv::Scalar(0,128,0), cv::Scalar(255,128,128), cv::Scalar(255,128,255), cv::Scalar(255,255,255)};

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

// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы
