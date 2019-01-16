#include "LandmarkDetector.h"
#include "cinder/Log.h"

LandmarkDetector::LandmarkDetector(std::string model_path) {
	_facemark = cv::face::FacemarkLBF::create();
	_facemark->loadModel(model_path);
}

std::vector<std::vector<cv::Point2f>> LandmarkDetector::detect(const cv::Mat &frame, cv::Rect region){
	
	std::vector<std::vector<cv::Point2f>> landmarks;
	std::vector<cv::Rect> wrapped_region;
	wrapped_region.push_back(region);

	try {
		_facemark->fit(frame, wrapped_region, landmarks);
	}
	catch (cv::Exception e) {
		CI_LOG_EXCEPTION("ERROR IN FACIAL LANDMARK DETECTION:", e);
	}

	return landmarks;
}

