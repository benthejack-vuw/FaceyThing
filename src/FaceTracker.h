#ifndef FaceTracker_h
#define FaceTracker_h

#include "FaceDetector.h"
#include "ObjectTracker.h"
#include "LandmarkDetector.h"

#include "cinder/Surface.h"
#include "cinder/Rect.h"

struct TrackedFace;

class FaceTracker {
public:
	FaceTracker(int calculation_scale);
	void update(ci::Surface8uRef);
	std::vector<TrackedFace> faces();


private:
	void update_frame(ci::Surface8uRef capture);
	void update_landmarks();
	void update_face_data(TrackerData td, std::vector<cv::Point2f> face_points);
	std::vector<ci::vec2> screen_space(std::vector<cv::Point2f> points);
	ci::Rectf FaceTracker::screen_space(cv::Rect2f rect);
	void FaceTracker::mark_faces_to_delete();
	void FaceTracker::delete_marked_faces();

	int _calculation_scale;
	int _frame_count;
	cv::Mat _resized_frame, _grey;

	std::shared_ptr<FaceDetector>     _detector;
	std::shared_ptr<ObjectTracker>    _tracker;
	std::shared_ptr<LandmarkDetector> _landmark_detector;
	std::vector<TrackedFace>          _faces;
};

struct TrackedFace {
	int global_index;
	ci::Rectf bounds;
	std::vector<ci::vec2> landmarks;
	bool marked_to_delete;
};


#endif
