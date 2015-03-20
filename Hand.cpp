// Forschungsproject (teil 3) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2015, Berlin

#include "stdafx.h"
#include "Hand.h"

Hand::Hand() {
	// new hand created!
	Parameters.moveAngle = NAN;
	Parameters.moveSpeed = -1;

	// YCrCb thresholds
	YCbCr.Y_MIN = 0;
	YCbCr.Y_MAX = 255;
	YCbCr.Cr_MIN = 133;
	YCbCr.Cr_MAX = 173;
	YCbCr.Cb_MIN = 77;
	YCbCr.Cb_MAX = 127;

	// HSV thresholds
	HSV.H_MIN = 0;
	HSV.H_MAX = 15;
	HSV.S_MIN = 76;
	HSV.S_MAX = 118;
	HSV.V_MIN = 0;
	HSV.V_MAX = 255;

	// previous position
	prevPosition.x = -1;
	prevPosition.y = -1;
}

Hand::~Hand() {
	//Tracker.hist.release();
	Tracker.KalmanTracker.KF.~KalmanFilter();
}

// add a new measurement for the Kalman tracker
void Hand::addKalmanMeasurement(const cv::Point coords) {
	// predict the location with KalmanFilter
	cv::Mat prediction = Tracker.KalmanTracker.KF.predict();

	Tracker.KalmanTracker.measurement(0) = coords.x;
	Tracker.KalmanTracker.measurement(1) = coords.y;
}

// get the latest predicted position by the Kalman
cv::Point Hand::getLatestKalmanPrediction() {
	// estimate the new position of the hand
	cv::Mat estimated = Tracker.KalmanTracker.KF.correct(Tracker.KalmanTracker.measurement);
	cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));

	Tracker.kalmTrack.push_back(statePt);

	return statePt;
}

void Hand::assignNewLocation(const Hand newHand) {
	handBox = newHand.handBox;
	Tracker.trackWindow = handBox.boundingRect();

	detectionBox = newHand.detectionBox;

	addKalmanMeasurement(cv::Point(newHand.handBox.center.x, newHand.handBox.center.y));
	Tracker.kalmTrack.push_back(getLatestKalmanPrediction());

	Tracker.camsTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));

	Tracker.isTracked = false;
}

void Hand::initTracker() {
	// init Tracker
	Tracker.trackWindow = handBox.boundingRect();
	Tracker.isTracked = false;

	Tracker.trackedFrames = 0;

	// init Kalman filter
    /*
	Tracker.KalmanTracker.KF = cv::KalmanFilter(6, 2, 0);
	Tracker.KalmanTracker.state = cv::Mat(6, 1, CV_32F);
	Tracker.KalmanTracker.processNoise = cv::Mat(6, 1, CV_32F);

	randn(Tracker.KalmanTracker.state, cv::Scalar::all(0), cv::Scalar::all(0.1));
	randn(Tracker.KalmanTracker.KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));

	Tracker.KalmanTracker.KF.transitionMatrix = *(cv::Mat_<float>(6, 6) <<
			1, 0, 1, 0, 0.5, 0,
			0, 1, 0, 1, 0, 0.5,
			0, 0, 1, 0, 1, 0,
			0, 0, 0, 1, 0, 1,
			0, 0, 0, 0, 1, 0,
			0, 0, 0, 0, 0, 1);

	Tracker.KalmanTracker.KF.measurementMatrix = *(cv::Mat_<float>(2, 6) << 
			1, 0, 1, 0, 0.5, 0,
			0, 1, 0, 1, 0, 0.5);

	Tracker.KalmanTracker.measurement = cv::Mat(2, 1, CV_32F);
	Tracker.KalmanTracker.measurement.setTo(cv::Scalar(0));

	Tracker.KalmanTracker.KF.statePre.at<float>(0) = handBox.center.x;
	Tracker.KalmanTracker.KF.statePre.at<float>(1) = handBox.center.y;
	Tracker.KalmanTracker.KF.statePre.at<float>(2) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(3) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(4) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(5) = 0;
 */
   
    // init Kalman filter
	Tracker.KalmanTracker.KF = cv::KalmanFilter(4, 2, 0);
	Tracker.KalmanTracker.state = cv::Mat(4, 1, CV_32F);
	Tracker.KalmanTracker.processNoise = cv::Mat(4, 1, CV_32F);
    
	randn(Tracker.KalmanTracker.state, cv::Scalar::all(0), cv::Scalar::all(0.1));
	randn(Tracker.KalmanTracker.KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
    
	Tracker.KalmanTracker.KF.transitionMatrix = *(cv::Mat_<float>(4, 4) <<
                                                  1, 0, 1, 0,
                                                  0, 1, 0, 1,
                                                  0, 0, 1, 0,
                                                  0, 0, 0, 1);
    
	Tracker.KalmanTracker.KF.measurementMatrix = *(cv::Mat_<float>(2, 4) <<
                                                   1, 0, 1, 0,
                                                   0, 1, 0, 1);
    
	Tracker.KalmanTracker.measurement = cv::Mat(2, 1, CV_32F);
	Tracker.KalmanTracker.measurement.setTo(cv::Scalar(0));
    
	Tracker.KalmanTracker.KF.statePre.at<float>(0) = handBox.center.x;
	Tracker.KalmanTracker.KF.statePre.at<float>(1) = handBox.center.y;
	Tracker.KalmanTracker.KF.statePre.at<float>(2) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(3) = 0;

	cv::setIdentity(Tracker.KalmanTracker.KF.measurementMatrix);
	cv::setIdentity(Tracker.KalmanTracker.KF.processNoiseCov, cv::Scalar::all(1e-2));
	cv::setIdentity(Tracker.KalmanTracker.KF.measurementNoiseCov, cv::Scalar::all(1e-1));
	cv::setIdentity(Tracker.KalmanTracker.KF.errorCovPost, cv::Scalar::all(.1));

	Tracker.isKalman = false;

	Tracker.kalmTrack.clear();
	//Tracker.kalmTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));

	Tracker.camsTrack.clear();
	//Tracker.camsTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));
}

// recalculate the hand's thresholding ranges:
void Hand::recalculateRange(const cv::Mat& frame, SkinSegmMethod method, const bool useHistogram) {
	if (method != SKIN_SEGMENT_ADAPTIVE) {
		cv::Rect rect = handBox.boundingRect();
		
		// reduce the rect to half of its size
		rect.y += (rect.height * 0.5);
		rect.height *= 0.5;
		rect.x += (rect.width * 0.2);
		rect.width *= 0.6;
        
        cv::Mat crop;
        
        // Method of color-picker
        if (!useHistogram) {
            cv::Rect picker(rect.x + rect.width / 2, rect.y + rect.height / 2, 4, 4);
        
            crop = cv::Mat(frame, picker);
        
            cv::pyrDown(crop, crop);
            cv::pyrDown(crop, crop);
        }
        else { // histogram
            crop = cv::Mat(frame.clone(), rect);
        }
        
		// convert it to HSV or YCrCb, depending on chosen segmentation method
		if (method == SKIN_SEGMENT_HSV)
			cv::cvtColor(crop, crop, cv::COLOR_BGR2HSV);
		else
			cv::cvtColor(crop, crop, cv::COLOR_BGR2YCrCb);
        
        if (!useHistogram) {
            
            cv::Vec3b vec = crop.at<cv::Vec3b>(0,0);
            
            if (method == SKIN_SEGMENT_HSV) {
                HSV.H_MIN = vec[0] - 10;
                HSV.H_MAX = vec[0] + 10;
                HSV.S_MIN = vec[1] - 35;
                HSV.S_MAX = vec[1] + 35;
                HSV.V_MIN = vec[2] - 35;
                HSV.V_MAX = vec[2] + 35;
            }
            else { // if YCrCb
                YCbCr.Y_MIN = vec[0] - 50;
                YCbCr.Y_MAX = vec[0] + 50;
                YCbCr.Cr_MIN = vec[1] - 20;
                YCbCr.Cr_MAX = vec[1] + 20;
                YCbCr.Cb_MIN = vec[2] - 25;
                YCbCr.Cb_MAX = vec[2] + 25;
            }
        }
        else { // histogram method:
            std::vector<cv::Mat> bgr_planes;
            cv::split(crop, bgr_planes);
            
            int histSize = 256;
            float range[] = { 0, 256 };
            const float* histRange = { range };
            
            std::vector<cv::Mat> histograms(3);
            
            /// Compute the histograms:
            cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), histograms[0], 1, &histSize, &histRange, true, false);
            cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), histograms[1], 1, &histSize, &histRange, true, false);
            cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), histograms[2], 1, &histSize, &histRange, true, false);
            
            int color_ranges[3][2] = {0, 0, 0, 0, 0, 0};
            
            for (int idx=0; idx<3; idx++) {
                double minVal, maxVal;
                cv::Point maxloc;
                cv::minMaxLoc(histograms[idx], &minVal, &maxVal, 0, &maxloc);
                
                int numMaxElement = (int)maxloc.y;
                
                // here we need to do te gaussian filtering and then differentiate;
                // Generate the Gaussian:
                int sigma = 5, size = 30, i = -size/2;
                std::vector<int> x;
                std::generate_n(std::back_inserter(x), (size), [&]() {return i++;});
                std::vector<double> gaussFilter;
                
                double filterSum = 0;
                
                i = 0;
                std::generate_n(std::back_inserter(gaussFilter), x.size(), [&]() {
                    double val = exp(-pow(x[i++],2) / (2 * sigma * sigma));
                    filterSum+=val;
                    return val;
                });
                
                std::for_each(gaussFilter.begin(), gaussFilter.end(), [&](double& V) { V /= filterSum;});
                
                // filter the histogram with the gaussian:
                int paddedLength = histograms[idx].rows + size - 1;
                std::vector<double> convolved(paddedLength); //zeros
                reverse(gaussFilter.begin(), gaussFilter.end());
                for(int outputIdx=0; outputIdx<paddedLength; outputIdx++) //index into 'convolved' vector
                {
                    int vecIdx = outputIdx - size + 1; //aligns with leftmost element of kernel
                    int lowerBound = std::max( 0, -vecIdx );
                    int upperBound = std::min( size, histograms[idx].rows - vecIdx );
                    
                    for( int kernelIdx = lowerBound; kernelIdx < upperBound; kernelIdx++ )
                    {
                        convolved[outputIdx] += gaussFilter[kernelIdx] * histograms[idx].at<float>(vecIdx+kernelIdx);
                    }
                }
                
                std::vector<double> diffVector;
                
                // i = size/2; // chek this!!!
                
                for (i = size/2; i < paddedLength - size/2; i++) {
                    diffVector.push_back(convolved[i+1]-convolved[i]);
                }
                
                // find lower bound:
                auto itLeft = std::max_element(diffVector.begin(), diffVector.begin() + numMaxElement);
                int left = std::distance(diffVector.begin(), itLeft);
                
                // find upper bound
                auto itRight = std::max_element(diffVector.begin() + numMaxElement, diffVector.end());
                int right = std::distance(diffVector.begin(), itRight) + numMaxElement;
                
                left = left - (numMaxElement - left);// * 0.9;
                right = right + (right - numMaxElement);// * 0.9;
                
                color_ranges[idx][0] = left;
                color_ranges[idx][1] = right;
            }
            if (method == SKIN_SEGMENT_HSV) {
                HSV.H_MIN = color_ranges[0][0];
                HSV.H_MAX = color_ranges[0][1];
                HSV.S_MIN = color_ranges[1][0];
                HSV.S_MAX = color_ranges[1][1];
                HSV.V_MIN = color_ranges[2][0];
                HSV.V_MAX = color_ranges[2][1];
            }
            else { // if YCrCb
                YCbCr.Y_MIN = color_ranges[0][0];
                YCbCr.Y_MAX = color_ranges[0][1];
                YCbCr.Cr_MIN = color_ranges[1][0];
                YCbCr.Cr_MAX = color_ranges[1][1];
                YCbCr.Cb_MIN = color_ranges[2][0];
                YCbCr.Cb_MAX = color_ranges[2][1];
            }
        }
        
        //std::cout << "--Range: " << (int)vec[0] << "-" << (int)vec[1] << "-" << (int)vec[2] << "--" << std::endl;
        
		// assign the ranges
	}
}
