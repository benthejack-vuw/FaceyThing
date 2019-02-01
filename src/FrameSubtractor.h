#pragma once
#include "cinder/Vector.h"
#include "cinder/Rect.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"

class FrameSubtractor
{
public:
	FrameSubtractor();
	std::vector<cinder::Rectf> subtract_and_detect(cv::Mat current_frame);

private:
	std::vector<cinder::Rectf> convert_keypoints(const std::vector<cv::KeyPoint> &points);

	bool _init;
	cv::Ptr<cv::SimpleBlobDetector> _detector;
	cv::Mat _last_frame;
};

