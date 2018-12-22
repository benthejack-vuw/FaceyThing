#include "FaceTracker.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include "CameraCapture.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"
#include "CinderOpenCv.h"

const cv::Scalar meanVal(104.0, 177.0, 123.0);
const std::string tensorflowConfigFile = "C:/Users/Ben/Desktop/Work/FaceyThing/build/FaceyThing/Debug/opencv_face_detector.pbtxt";
const std::string tensorflowWeightFile = "C:/Users/Ben/Desktop/Work/FaceyThing/build/FaceyThing/Debug/opencv_face_detector_uint8.pb";

cv::dnn::Net net;

FaceTracker::FaceTracker() {
	_frame_count = 0;
	net = cv::dnn::readNetFromTensorflow(tensorflowWeightFile, tensorflowConfigFile);
}

void FaceTracker::update(ci::Surface8uRef capture) {

	update_images(capture);
	update_trackers();

	if (_frame_count % 10 == 0) {
		_faces = find_faces();
		if (_faces.size() != _trackers.size()) {
			correlate_faces();
		}
	}

	remove_dead_trackers();
	_frame_count++;
}

ci::vec2 rect_centroid(cv::Rect rect) {
	float rcx = rect.tl.x + rect.width / 2.0f;
	float rcy = rect.tl.y + rect.height / 2.0f;
	return ci::vec2(rcx, rcy);
}

float centroid_dist(cv::Rect r1, cv::Rect r2) {
	ci::vec2 c1 = rect_centroid(r1);
	ci::vec2 c2 = rect_centroid(r2);
	return ci::distance(c1, c2);
}

void FaceTracker::correlate_faces() {

	
}

void FaceTracker::update_trackers() {
	
}

void FaceTracker::remove_dead_trackers() {
	for (int i = 0; i < _trackers.size(); ++i) {
		//CI_LOG_D(_trackers[i].get_position());
	}
}

void FaceTracker::update_images(ci::Surface8uRef capture) {
	cv::Mat image = ci::toOcv(*capture);
	const int calcscale = 2;

	int scaledWidth = capture->getWidth() / calcscale;
	int scaledHeight = capture->getHeight() / calcscale;
	_scaledCVImage = cv::Mat(scaledHeight, scaledWidth, image.type());
	cv::resize(image, _scaledCVImage, _scaledCVImage.size(), 0, 0, cv::INTER_LINEAR);
}

std::vector<cv::Rect> FaceTracker::find_faces() {
	cv::Mat inputBlob = cv::dnn::blobFromImage(_scaledCVImage, 1, _scaledCVImage.size(), mean(_scaledCVImage), false, false);

	net.setInput(inputBlob, "data");
	cv::Mat detection = net.forward("detection_out");
	cv::Mat detectionMat(detection.size[2], detection.size[3], CV_8U, detection.ptr<float>());

	std::vector<cv::Rect> out_vec;
	for (int i = 0; i < detectionMat.rows; i++)
	{
		float confidence = detectionMat.at<float>(i, 2);
		if (confidence > 0.75)
		{
			int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * _scaledCVImage.size().width);
			int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * _scaledCVImage.size().height);
			int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * _scaledCVImage.size().width);
			int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * _scaledCVImage.size().height);
			int width = abs(x2 - x1);
			int height = abs(y2 - y1);

			out_vec.emplace_back(x1, y1, x2, y2);
		}
	}

	return out_vec;
}