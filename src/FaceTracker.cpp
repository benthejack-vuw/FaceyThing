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
		_landmark_detector->detect(_resized_frame, _tracker->tracked_regions());

		_frame_count++;
	}
	catch(cv::Exception e) {
		CI_LOG_EXCEPTION("error in facetracker update", e);
	}
}


void FaceTracker::update_frame(ci::Surface8uRef capture) {
	cv::Mat unscaled = ci::toOcv(*capture); 

	int scaledWidth = capture->getWidth() / _calculation_scale;
	int scaledHeight = capture->getHeight() / _calculation_scale;
	_resized_frame = cv::Mat(scaledHeight, scaledWidth, unscaled.type());
	cv::resize(unscaled, _resized_frame, _resized_frame.size(), 0, 0, cv::INTER_LINEAR);
}


std::vector<cv::Rect2f> FaceTracker::detected_faces() {
	return _detector->faces();
}

std::vector<cv::Rect> FaceTracker::tracker_regions() {
	return _tracker->tracked_regions();
}

std::vector<ci::Rectf> FaceTracker::screenspace_tracker_rects() {
	std::vector<ci::Rectf> out;
	for each(cv::Rect2f r in tracker_regions()) {
		out.emplace_back(r.tl().x*_calculation_scale,
			r.tl().y*_calculation_scale,
			r.br().x*_calculation_scale,
			r.br().y*_calculation_scale);
	}
	return out;
}

std::vector<ci::Rectf> FaceTracker::screenspace_detected_faces() {
	std::vector<ci::Rectf> out;
	for each(cv::Rect2f r in _detector->faces()) {
		out.emplace_back(r.tl().x*_calculation_scale,
			r.tl().y*_calculation_scale,
			r.br().x*_calculation_scale,
			r.br().y*_calculation_scale);
	}
	return out;
}

std::vector<ci::vec2> FaceTracker::screenspace_face_centroids() {
	std::vector<ci::vec2> out;
	std::vector<ci::Rectf> rects = screenspace_tracker_rects();
	for each(ci::Rectf r in rects) {
		out.push_back(r.getCenter());
	}
	return out;
}

std::vector<std::vector<ci::vec2>> FaceTracker::screenspace_facial_landmarks() {
	std::vector<std::vector<ci::vec2>> out;

	if (_frame_count > 0) {
		for each (std::vector<cv::Point2f> face in _landmark_detector->landmarks()) {
			std::vector<ci::vec2> ci_face;
			for each (cv::Point2f p in face) {
				ci_face.push_back(ci::vec2(p.x*_calculation_scale, p.y*_calculation_scale));
			}
			out.push_back(ci_face);
		}
	}

	return out;
}

