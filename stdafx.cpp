#include "stdafx.h"

// crop a rectangle from an image with no problems
void cropRoi(const cv::Mat inputImage, cv::Mat& outputCrop, cv::Rect& roiRectangle) {
	cv::Rect roiRect;

	roiRect.x = (roiRectangle.x < 0) ? 0 : roiRectangle.x;
	roiRect.y = (roiRectangle.y < 0) ? 0 : roiRectangle.y;

	roiRect.width = (roiRect.x - 1 + roiRectangle.width) >= inputImage.cols ? (inputImage.cols - 1 - roiRect.x) : roiRectangle.width;
	roiRect.height = (roiRect.y - 1 + roiRectangle.height) >= inputImage.rows ? (inputImage.rows - 1 - roiRect.y) : roiRectangle.height;

	roiRect.width -= 1;
	roiRect.height -= 1;

	roiRect.x = abs(roiRect.x);
	roiRect.y = abs(roiRect.y);
	roiRect.width = abs(roiRect.width);
	roiRect.height = abs(roiRect.height);

	outputCrop = cv::Mat(inputImage, roiRect);

	roiRectangle = roiRect;
}