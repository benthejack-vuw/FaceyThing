#pragma once

#include "CinderOpenCV.h"
#include "opencv2/dnn.hpp"

class FaceDetector {
public:
	FaceDetector(std::string config_file, std::string weight_file);
	void detect_faces(const cv::Mat &frame);
	void detect_faces_HAAR(cv::Mat &frame);
	std::vector<cv::Rect2f> faces();

private:
	cv::dnn::Net _net;
	cv::CascadeClassifier _faceCascade;
	std::vector<cv::Rect2f> _detected_faces;
};