#include "ObjectTracker.h"
#include "cinder/Log.h"

//---------------------------------------HELPER FORWARD DECLARATIONS--------------------------------------------
cv::Point rect_centroid(cv::Rect2f r);
float centroid_dist(cv::Rect2f r1, cv::Rect2f r2);
bool withinRect(cv::Point2f p, cv::Rect2f r);
bool centroid_within(cv::Rect2f centroid, cv::Rect2f within);
//--------------------------------------------------------------------------------------------------------------

int _global_object_index;

ObjectTracker::ObjectTracker()
{
	_global_object_index = 0;
}

void ObjectTracker::update_trackers(const cv::Mat &frame) {
	for (int i = 0; i < _trackers.size(); ++i) {
		_trackers[i].tracker->update(frame, _trackers[i].bounds);
	}
}


void ObjectTracker::track(const cv::Mat &frame) {
	update_trackers(frame);
}

void ObjectTracker::correlate_regions(std::vector<cv::Rect2f> &faces, const cv::Mat &frame) {
	unlink_faces();
	correlate(faces, frame);
	increment_misses();
	remove_dead_trackers();
}

void ObjectTracker::correlate(std::vector<cv::Rect2f> &faces, const cv::Mat &frame) {

	for (int i = 0; i < faces.size(); ++i) {

		bool found = false;
		cv::Rect face = faces[i];

		for (int j = 0; j < _trackers.size(); ++j) {
			if (centroid_within(face, _trackers[j].bounds) && centroid_within(_trackers[j].bounds, face)) {
				found = true;
				reset_tracker(_trackers[j], face, i, frame);
			}
		}

		if (!found) {
			add_tracker(face, i, frame);
		}
	}

}

void ObjectTracker::increment_misses() {
	for (int i = 0; i < _trackers.size(); ++i) {
		if (_trackers[i].linked_face_index = -1) {
			_trackers[i].find_misses++;
		}
	}
}


std::vector<TrackerData> ObjectTracker::trackers() {
	return _trackers;
}

void ObjectTracker::reset_tracker(TrackerData &tracker, cv::Rect2f rect, int linked_face, const cv::Mat &frame) {
	tracker.linked_face_index = linked_face;
	tracker.find_misses = 0;
	tracker.tracker = cv::TrackerMOSSE::create();
	tracker.bounds = cv::Rect(rect);
	tracker.tracker->init(frame, tracker.bounds);
}

void ObjectTracker::unlink_faces() {
	for (int i = 0; i < _trackers.size(); ++i) {
		_trackers[i].linked_face_index = -1;
	}
}


void ObjectTracker::remove_dead_trackers() {
	for (int i = 0; i < _trackers.size(); ++i) {
		if (_trackers[i].find_misses >= 2) {
			_trackers.erase(_trackers.begin() + i);
			i--;
		}
	}
}

void ObjectTracker::add_tracker(cv::Rect rect, int face_index, const cv::Mat &frame) {
	TrackerData trackerData;

	trackerData.bounds = cv::Rect(rect);
	trackerData.linked_face_index = face_index;
	trackerData.find_misses = 0;
	trackerData.global_index = _global_object_index;

	trackerData.tracker = cv::TrackerMOSSE::create();
	trackerData.tracker->init(frame, trackerData.bounds);

	_trackers.push_back(trackerData);
	_global_object_index++;
}



std::vector<cv::Rect> ObjectTracker::tracked_regions() {
	std::vector<cv::Rect> out;
	for (int i = 0; i < _trackers.size(); ++i) {
		out.push_back(_trackers[i].bounds);
	}
	return out;
}


//-----------------------------------------------------HELPER FUNCTIONS--------------------------------------------------
cv::Point rect_centroid(cv::Rect2f r) {
	float rcx = r.tl().x + r.width / 2.0f;
	float rcy = r.tl().y + r.height / 2.0f;
	return cv::Point(rcx, rcy);
}

float centroid_dist(cv::Rect2f r1, cv::Rect2f r2) {
	ci::vec2 c1 = ci::fromOcv(rect_centroid(r1));
	ci::vec2 c2 = ci::fromOcv(rect_centroid(r2));
	return ci::distance(c1, c2);
}

bool withinRect(cv::Point2f p, cv::Rect2f r) {
	return r.contains(p);
}

bool centroid_within(cv::Rect2f centroid, cv::Rect2f within) {
	return within.contains(rect_centroid(centroid));
}