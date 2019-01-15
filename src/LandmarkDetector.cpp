#include "LandmarkDetector.h"
#include "cinder/Log.h"

LandmarkDetector::LandmarkDetector(std::string model_path) {
	_facemark = cv::face::FacemarkLBF::create();
	_facemark->loadModel(model_path);
}

void LandmarkDetector::detect(const cv::Mat &frame, std::vector<cv::Rect> regions) {
	_landmarks.clear();

	try {
		_facemark->fit(frame, regions, _landmarks);
	}
	catch (cv::Exception e) {
		CI_LOG_EXCEPTION("ERROR IN FACIAL LANDMARK DETECTION:", e);
	}
}

std::vector<std::vector<cv::Point2f>> LandmarkDetector::landmarks() {
	return _landmarks;
}
