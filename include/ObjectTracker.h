#pragma once

#include "CinderOpenCV.h"
#include "opencv2/tracking.hpp"

struct TrackerData;

class ObjectTracker {
public:
	ObjectTracker();
	void update_trackers(const cv::Mat &frame);
	std::vector<TrackerData> trackers();
	void ObjectTracker::track(const cv::Mat &frame);
	void ObjectTracker::correlate_regions(std::vector<cv::Rect2f> &faces, const cv::Mat &frame);
	std::vector<cv::Rect> tracked_regions();


private:
	void remove_dead_trackers();
	void add_tracker(cv::Rect, int face_index, const cv::Mat &frame);
	void reset_tracker(TrackerData &tracker, cv::Rect2f rect, int linked_face, const cv::Mat &frame);
	void unlink_faces();
	void ObjectTracker::correlate(std::vector<cv::Rect2f> &faces, const cv::Mat &frame);
	void ObjectTracker::increment_misses();

	std::vector<TrackerData> _trackers;
};

struct TrackerData {
	cv::Ptr<cv::Tracker> tracker;
	cv::Rect2d bounds;
	int global_index, linked_face_index, find_misses;
};