#ifndef FaceTracker_h
#define FaceTracker_h

#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Capture.h"

#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>

class FaceTracker {
public:
	FaceTracker();
	void update(ci::Surface8uRef);
	

private:
	std::vector<cv::Rect> find_faces();
	void correlate_faces();
	void update_trackers();
	void remove_dead_trackers();
	void update_images(ci::Surface8uRef capture);

	int _frame_count;
	std::vector<cv::Rect> _faces;
	std::vector<cv::Tracker> _trackers;
	cv::Mat _scaledCVImage;
}
#endif