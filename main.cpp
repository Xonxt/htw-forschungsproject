// ConsoleApplication2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "FrameProcessor.h"

#include <string>
#include <sstream>
#include <time.h>

#define VIDEO_FILE true

const char* VideoFile[] = { "D:\\temp\\FP\\HFD\\MVI_5513.MOV", 
							"D:\\temp\\FP\\HFD\\00237.MTS", 
							"D:\\temp\\FP\\HFD\\00238.MTS", 
							"D:\\temp\\FP\\HFD\\00239.MTS",
                            "/Users/nikita/Movies/MVI_5513.MOV"
};

enum VideoFileNames {
	VIDEO_FILE_MVI5513MOV = 0,
	VIDEO_FILE_00237MTS = 1,
	VIDEO_FILE_00238MTS = 2,
	VIDEO_FILE_00239MTS = 3,
    MAC_VIDEO_FILE_MVI5513MOV = 4
};

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	/* open capture */
	VideoCapture capture;

	if (VIDEO_FILE) { // from video file
		capture = VideoCapture(VideoFile[MAC_VIDEO_FILE_MVI5513MOV]);
	}
	else { // or from webcam
		capture = VideoCapture(0);
	}

	// check of opened successfully?
	if (!capture.isOpened()) {
		cout << "Failed to open video device!" << endl;
		return -1;
	}

	// create Frame Processor object
	FrameProcessor frameProcessor;

	// initizlize frame processor
	if (!frameProcessor.initialize()) {
		cout << "Failed to initialize Frame Processor!" << endl;
		return -2;
	}

	// create a window
	namedWindow("video", 0);
	resizeWindow("video", 640, 480);

	// a frame variable
	Mat frame;

	// should we skip some frames?
	int skipFrames = 70;

	// current frame number
	int frameNumber = 0;

	// infinite loop for the video stream
	while (true) {		

		// read a frame
		capture >> frame;

		// frame not empty?
		if (!frame.empty()) {

			// skip?
			if (++frameNumber <= skipFrames) {
				continue;
			}

			// mirror, if from webcam:
			if (!VIDEO_FILE) {
				Mat _frame;
				flip(frame, _frame, 1);
				_frame.copyTo(frame);
			}

			frameProcessor.processFrame(frame);

			// show the frame
			imshow("video", frame);
		}

		// key pressed?
		int key = waitKey(10);

		// which key?
		if (char(key) == 27) {		// 'Esc'
			// exit loop
			break;
		}
		else if (char(key) == 32) { // 'Spacebar'
			// save snapshot to file
			std::ostringstream ost;
            
            /*
			char name[12];
			time_t now = time(0);
			struct tm _Tm;
			localtime_s(&_Tm, &now);
			strftime(name, sizeof(name), "%H-%M-%S", &_Tm);
			*/
			 
			// 
			char name[12];
            time_t now = time(0);
            strftime(name, sizeof(name), "%H-%M-%S", localtime(&now));
			

			ost << "frame-" << name << ".jpg";
			imwrite(ost.str(), frame);
		}
		else { // a different key
			switch (char(key)) {
			case 's':	// change segmentation method
				frameProcessor.changeSkinMethod();
				break;
			case 'm':	// display back-projection mask
				frameProcessor.toggleShowMask();
				break;
			}
		}
	}

	return 0;
}

