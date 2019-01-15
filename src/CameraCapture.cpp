#include "CameraCapture.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

void flip_buffer(cinder::Surface8uRef &buffer_data);


CameraCapture::CameraCapture(ci::vec2 resolution, std::string camera_name){
	_frame_available = false;
    try {
		ci::Capture::DeviceRef d = ci::Capture::findDeviceByName(camera_name);
        _capture = cinder::Capture::create( resolution.x, resolution.y,  d);
        _capture->start();
    }
    catch( ci::Exception &exc ) {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc );
    }
    

    _surface = cinder::Surface8u::create(resolution.x, resolution.y, false);
    _texture = cinder::gl::Texture::create(resolution.x, resolution.y);
}

void CameraCapture::update(){

	if( _capture && _capture->checkNewFrame() ) {
        _surface = _capture->getSurface();
		flip_buffer(_surface);
        _texture->update(*_surface);
		_frame_available = true;
    }
}

cinder::gl::TextureRef CameraCapture::texture(){
    return _texture;
}

cinder::Surface8uRef CameraCapture::surface(){
	_frame_available = false;
	return _surface;
}

bool CameraCapture::frame_available() {
	return _frame_available;
}

void flip_buffer(cinder::Surface8uRef &buffer_data) {

	uint8_t * dat = buffer_data->getData();
	int x = 0;
	int y = 0;
	int tempVal;
	int tempVal2;


	int w = buffer_data->getWidth();
	int h = buffer_data->getHeight();

	for (int i = 0; i < 3; i++) {
		for (y = 0; y < h; y++) {
			for (x = 0; x < w / 2; x++) {

				tempVal = (int)dat[((y*w) + (w - 1 - x)) * 3 + i];
				tempVal2 = (int)dat[((y*w) + (x)) * 3 + i];

				dat[((y*w) + (x)) * 3 + i] = tempVal;
				dat[((y*w) + (w - 1 - x)) * 3 + i] = tempVal2;

			}
		}
	}
}