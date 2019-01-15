#ifndef FaceTracker_h
#define FaceTracker_h

#include "FaceDetector.h"
#include "ObjectTracker.h"
#include "LandmarkDetector.h"

#include "cinder/Surface.h"
#include "cinder/Rect.h"


class FaceTracker {
public:
	FaceTracker(int calculation_scale);
	void update(ci::Surface8uRef);

	std::vector<cv::Rect> tracker_regions();
	std::vector<cv::Rect2f> detected_faces();

	std::vector<std::vector<ci::vec2>> screenspace_facial_landmarks();
	std::vector<ci::Rectf> screenspace_tracker_rects();
	std::vector<ci::vec2> screenspace_face_centroids();
	std::vector<ci::Rectf> screenspace_detected_faces();

private:
	void update_frame(ci::Surface8uRef capture);

	int _calculation_scale;
	int _frame_count;
	cv::Mat _resized_frame;

	std::shared_ptr<FaceDetector>     _detector;
	std::shared_ptr<ObjectTracker>    _tracker;
	std::shared_ptr<LandmarkDetector> _landmark_detector;
};


#endif
