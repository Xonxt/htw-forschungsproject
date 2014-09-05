// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "FrameProcessor.h"

#include <string>
#include <sstream>
#include <time.h>

// set 'true' if you're loading a video file
// or 'false' if you use a capture devide (web-cam)
#define VIDEO_FILE true

// should I record the video too?
#define RECORD_VIDEO true

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

// construct a filename based on the current time and supplied extension (with a ".")
std::string generateFileName(const char* ext) {
	std::ostringstream ost;
/*
	// Visual studio
	char timeString[12];
	time_t now = time(0);
	struct tm _Tm;
	localtime_s(&_Tm, &now);
	strftime(timeString, sizeof(timeString), "%H-%M-%S", &_Tm);
*/
	
	// Code::Blocks and XCode
	char timeString[12];
	time_t now = time(0);
	strftime(timeString, sizeof(timeString), "%H-%M-%S", localtime(&now));		

	ost << timeString;

	if (ext[0] != '.')
		ost << ".";

	ost << ext;

	return ost.str();
}

int main(int argc, char* argv[])
{
	/* open capture */
	VideoCapture capture;

	if (VIDEO_FILE) { // from video file        
		capture = VideoCapture(VideoFile[VIDEO_FILE_MVI5513MOV]);
	}
	else { // or from webcam
		capture = VideoCapture(0);
	}

	// check of opened successfully?
	if (!capture.isOpened()) {
		cout << "Failed to open video capture!" << endl;
		return -1;
	}

	// open video recorder
	VideoWriter outputVideo;
	if (RECORD_VIDEO) {
		// output video
		int ex = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
		Size S = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
					  (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
		

		// construct a video file name

		outputVideo.open(generateFileName(".avi"), -1, capture.get(CV_CAP_PROP_FPS), S, true);

		if (!outputVideo.isOpened()) {
			cout << "Could not open the output video for write. " << endl;
		}
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
	Mat frame, stillFrame;

	// should we skip some frames?
	int skipFrames = 70;

	// current frame number
	int frameNumber = 0;
    
    // pause or resume
    bool pause = false;

	// display a menu
	cout << "Use the following keys for result" << endl << endl;
	cout << "'ESC'\texit application" << endl;
	cout << "'SPACE'\tsave screenshot" << endl;
	cout << "'m'\tdisplay backprojection mask" << endl;
	cout << "'s'\tchange skin segmentation method" << endl;
    cout << "'p'\tpause or resume" << endl;

	// infinite loop for the video stream
	while (true) {		
		// read a frame
        if (!pause) {
            capture >> frame;
            frame.copyTo(stillFrame);
        }
        else {
            stillFrame.copyTo(frame);
        }
            
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

			// write the frame into video
			if (outputVideo.isOpened() && RECORD_VIDEO) {
				outputVideo << frame;
			}
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
			imwrite(generateFileName(".jpg"), frame);
		}
		else { // a different key
			switch (char(key)) {
			case 's':	// change segmentation method
				frameProcessor.changeSkinMethod();
				break;
			case 'm':	// display back-projection mask
				frameProcessor.toggleShowMask();
				break;
			case 'p':   // pause/resume
                pause = !pause;
                break;
            }
            
		}
	}

	// close all windows
	destroyAllWindows();

	// release the video writer
    if (RECORD_VIDEO)
        outputVideo.release();

	return 0;
}

