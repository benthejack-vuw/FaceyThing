#include "FaceTracker.h"
#include "CinderOpenCV.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

FaceTracker::FaceTracker(int calculation_scale):
	_calculation_scale(calculation_scale),
	_frame_count(0)
{

	_detector = std::shared_ptr<FaceDetector>(
		new FaceDetector(
			ci::app::getAssetPath("deploy.prototxt").generic_string(),
			ci::app::getAssetPath("res10_300x300_ssd_iter_140000_fp16.caffemodel").generic_string()
		)
	);

	_tracker = std::shared_ptr<ObjectTracker>(new ObjectTracker());
	_landmark_detector = std::shared_ptr<LandmarkDetector>(new LandmarkDetector(ci::app::getAssetPath("lbfmodel.yaml").generic_string()));
}


void FaceTracker::update(ci::Surface8uRef capture) {

	try {
		update_frame(capture);

		if (_frame_count % 15 == 0) {
			_detector->detect_faces(_resized_frame);
			_tracker->correlate_regions(_detector->faces(), _resized_frame);
		}

		_tracker->track(_resized_frame);

		mark_faces_to_delete();
		update_landmarks(_tracker->trackers());
		delete_marked_faces();

		_frame_count++;
	}
	catch(cv::Exception e) {
		CI_LOG_EXCEPTION("error in facetracker update", e);
	}
}

void FaceTracker::update_landmarks(std::vector<TrackerData> &trackers) {

	for each(TrackerData d in trackers) {
		//possible speedup by linking indices of rects with outputs
		std::vector<cv::Point2f> points = _landmark_detector->detect(_resized_frame, d.bounds)[0];
		update_face_data(d, points);
	}

}

void FaceTracker::mark_faces_to_delete() {
	for (int i = 0; i < _faces.size(); ++i) {
		_faces[i].marked_to_delete = true;
	}
}

void FaceTracker::delete_marked_faces() {
	for (int i = 0; i < _faces.size(); ++i) {
		if (_faces[i].marked_to_delete) {
			_faces.erase(_faces.begin() + i);
			i--;
		}
	}
}

void FaceTracker::update_face_data(TrackerData td, std::vector<cv::Point2f> face_points) {
	
	bool found = false;

	for (int i = 0; i < _faces.size(); ++i) {
		if (_faces[i].global_index == td.global_index) {
			_faces[i].bounds = screen_space(td.bounds);
			_faces[i].landmarks = screen_space(face_points);
			_faces[i].marked_to_delete = false;
			found = true;
		}
	}

	if (!found) {
		TrackedFace new_face = {td.global_index, screen_space(td.bounds), screen_space(face_points), false};
		_faces.push_back(new_face);
	}
}



void FaceTracker::update_frame(ci::Surface8uRef capture) {
	cv::Mat unscaled = ci::toOcv(*capture); 

	int scaledWidth = capture->getWidth() / _calculation_scale;
	int scaledHeight = capture->getHeight() / _calculation_scale;
	_resized_frame = cv::Mat(scaledHeight, scaledWidth, unscaled.type());
	cv::resize(unscaled, _resized_frame, _resized_frame.size(), 0, 0, cv::INTER_LINEAR);
}


std::vector<TrackedFace> FaceTracker::faces() {
	return _faces;
}


std::vector<ci::vec2> FaceTracker::screen_space(std::vector<cv::Point2f> points) {
	std::vector<ci::vec2> out;
	for each (cv::Point2f p in points) {
		out.emplace_back(p.x*_calculation_scale, p.y*_calculation_scale);
	}
	return out;
}

ci::Rectf FaceTracker::screen_space(cv::Rect2f rect) {
	return ci::Rectf(
		rect.tl().x*_calculation_scale, 
		rect.tl().y*_calculation_scale,
		rect.br().x *_calculation_scale, 
		rect.br().y*_calculation_scale
	);
}