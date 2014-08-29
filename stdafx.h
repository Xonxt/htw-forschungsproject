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

enum SkinSegmMethod {
	SKIN_SEGMENT_ADAPTIVE = 0,
	SKIN_SEGMENT_YCRCB = 1,
	SKIN_SEGMENT_HSV = 2
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
