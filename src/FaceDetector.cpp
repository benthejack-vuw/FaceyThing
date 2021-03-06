#include "FaceDetector.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace cv;

FaceDetector::FaceDetector(std::string config_file, std::string weight_file) {
	_net = cv::dnn::readNetFromCaffe(config_file, weight_file);

	_faceCascade.load(ci::app::getAssetPath("haarcascade_frontalface_default.xml").generic_string());
}

void FaceDetector::detect_faces(const cv::Mat &frame) {

	try {
		_detected_faces.clear();

		cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1, frame.size(), mean(frame), false, false);

		_net.setInput(inputBlob, "data");
		cv::Mat detection = _net.forward("detection_out");
		cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);
			if (confidence > 0.85)
			{
				float x1 = detectionMat.at<float>(i, 3);
				float y1 = detectionMat.at<float>(i, 4);
				float x2 = detectionMat.at<float>(i, 5);
				float y2 = detectionMat.at<float>(i, 6);
				float width = abs(x2 - x1);
				float height = abs(y2 - y1);
				_detected_faces.emplace_back(x1*frame.size().width, y1*frame.size().height, width*frame.size().width, height*frame.size().height);
			}
		}
	}
	catch (cv::Exception e) {
		CI_LOG_EXCEPTION("openCV error in Facial Detection: ", e);
	}
}


void FaceDetector::detect_faces_HAAR(cv::Mat &frame) {
	_detected_faces.clear();
	std::vector<cv::Rect> detected_faces;
	_faceCascade.detectMultiScale(frame, detected_faces);

	for (std::vector<cv::Rect>::const_iterator faceIter = detected_faces.begin(); faceIter != detected_faces.end(); ++faceIter) {
		cv::Rect2f o(faceIter->x, faceIter->y, faceIter->width, faceIter->height);
		_detected_faces.push_back(o);
		ci::gl::drawStrokedRect(ci::fromOcv(o));
	}
}


std::vector<cv::Rect2f> FaceDetector::faces() {
	return _detected_faces;
}