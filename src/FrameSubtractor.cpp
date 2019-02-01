#include "FrameSubtractor.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "CinderOpenCV.h"

using namespace cv;



FrameSubtractor::FrameSubtractor(): _init(true)
{

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	params.filterByArea = true;
	params.minArea = 1500;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;

	// Filter by Inertia
	params.filterByInertia = true;

	 //Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 255;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 10;

	// Set up detector with params
	_detector = SimpleBlobDetector::create(params);
}

std::vector<cinder::Rectf> FrameSubtractor::subtract_and_detect(cv::Mat current_frame) {

	if (_init) {
		current_frame.copyTo(_last_frame);
		_init = false;
	}

	cv::Mat subtracted(current_frame.rows, current_frame.cols, current_frame.type());
	cv::subtract(_last_frame, current_frame, subtracted);

	bitwise_not(subtracted, subtracted);

	ci::gl::Texture2dRef tex = ci::gl::Texture::create(ci::fromOcv(subtracted));
	ci::gl::draw(tex);

	std::vector<cv::KeyPoint> points;
	_detector->detect(subtracted, points);

	current_frame.copyTo(_last_frame);

	return convert_keypoints(points);
}

std::vector<cinder::Rectf> FrameSubtractor::convert_keypoints(const std::vector<cv::KeyPoint> &points) {
	std::vector<cinder::Rectf> out;

	for (auto i : points) {
		out.emplace_back(i.pt.x, i.pt.y, i.pt.x + i.size*10, i.pt.y + i.size*10);
	}

	return out;
}