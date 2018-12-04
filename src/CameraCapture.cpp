#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>

#include "CameraCapture.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"
#include "CinderOpenCv.h"

const cv::Scalar meanVal(104.0, 177.0, 123.0);

const std::string tensorflowConfigFile = "./opencv_face_detector.pbtxt";
const std::string tensorflowWeightFile = "./opencv_face_detector_uint8.pb";


const std::string caffeConfigFile = "./deploy.prototxt";
const std::string caffeWeightFile = "./res10_300x300_ssd_iter_140000_fp16.caffemodel";

cv::dnn::Net net;

CameraCapture::CameraCapture(int width, int height){

	//net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);

	net = cv::dnn::readNetFromTensorflow(tensorflowWeightFile, tensorflowConfigFile);

    try {
        _capture = cinder::Capture::create( width, height, cinder::Capture::getDevices()[0]);
        _capture->start();
    }
    catch( ci::Exception &exc ) {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc );
    }
    
    _surface = cinder::Surface8u::create(width, height, false);
	CI_LOG_D(_surface->getWidth() << "  :  " << _surface->getHeight() << "\n");

    _texture = cinder::gl::Texture::create(width, height);
	_read = false;
}

void CameraCapture::update(){
    if( _capture && _capture->checkNewFrame() ) {
		_read = true;
        _surface = _capture->getSurface();
        _texture->update(*_surface);
    }
}

cinder::gl::TextureRef CameraCapture::texture(){
    return _texture;
}

cinder::Surface8uRef CameraCapture::surface(){
    return _surface;
}

void CameraCapture::print_faces(){

	if (_read) {
		cv::Mat image = ci::toOcv(*_surface);
		const int calcscale = 2;

		int scaledWidth = _surface->getWidth() / calcscale;
		int scaledHeight = _surface->getHeight() / calcscale;
		cv::Mat smallImg(scaledHeight, scaledWidth, image.type());
		cv::resize(image, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);


		cv::Mat inputBlob = cv::dnn::blobFromImage(smallImg, 1, smallImg.size(), mean(smallImg), false, false);

		net.setInput(inputBlob, "data");
		cv::Mat detection = net.forward("detection_out");
		cv::Mat detectionMat(detection.size[2], detection.size[3], CV_8U, detection.ptr<float>());

		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);
			if (confidence > 0.75)
			{

				CI_LOG_D("FOUND");

				int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * _surface->getWidth());
				int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * _surface->getHeight());
				int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * _surface->getWidth());
				int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * _surface->getHeight());
				int width =  abs(x2 - x1);
				int height = abs(y2 - y1);


				ci::Rectf drawRect(x1-width/3, y1- height / 3, x2+width/3, y2+height/3);
				ci::gl::drawStrokedRect(drawRect);

			}
		}

	}
}
