#include "CameraCapture.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/scoped.h"

void flip_buffer(cinder::Surface8uRef &buffer_data);


CameraCapture::CameraCapture(ci::vec2 resolution, float scale_down, std::string camera_name){
	_frame_available = false;
    try {
		ci::Capture::DeviceRef d = ci::Capture::findDeviceByName(camera_name);
        _capture = cinder::Capture::create( resolution.x, resolution.y,  d);
        _capture->start();
    }
    catch( ci::Exception &exc ) {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc );
    }
    
	_scaled_image = ci::gl::Fbo::create(resolution.x/scale_down, resolution.y/scale_down, false, false, false);
	_flipped_image = ci::gl::Fbo::create(resolution.x, resolution.y, false, false, false);
	_surface = cinder::Surface8u::create(resolution.x, resolution.y, false);
    _texture = cinder::gl::Texture::create(resolution.x, resolution.y);
	_scaled_texture = cinder::gl::Texture::create(resolution.x/scale_down, resolution.y/ scale_down);

}

void CameraCapture::update(){

	if( _capture && _capture->checkNewFrame() ) {
        _surface = _capture->getSurface();
		flip_buffer();
		scale_down();
		//flip_buffer(_surface);
        _texture->update(*_surface);
		_frame_available = true;
    }
}

cinder::gl::TextureRef CameraCapture::flipped(){
    return _flipped_image->getColorTexture();
}

cinder::Surface8uRef CameraCapture::surface(){
	_frame_available = false;
	return _surface;
}

bool CameraCapture::frame_available() {
	return _frame_available;
}

void CameraCapture::flip_buffer() {
	_flipped_image->bindFramebuffer();
	ci::gl::pushMatrices();
	ci::gl::ScopedViewport vp4(_flipped_image->getSize());
	ci::gl::setMatricesWindow(_flipped_image->getSize());
	ci::gl::translate(_flipped_image->getSize().x, 0);
	ci::gl::scale(-1, 1);
	ci::gl::color(ci::ColorA(1, 1, 1, 1));
	ci::gl::draw(_texture);
	ci::gl::popMatrices();
	_flipped_image->unbindFramebuffer();
}

void CameraCapture::scale_down() {
	_scaled_image->bindFramebuffer();
	ci::gl::ScopedViewport vp4(_scaled_image->getSize());
	ci::gl::setMatricesWindow(_scaled_image->getSize());
	ci::gl::color(ci::ColorA(1, 1, 1, 1));
	ci::gl::draw(_flipped_image->getColorTexture(), ci::Area(_surface->getBounds()), _scaled_image->getBounds());
	_scaled_image->unbindFramebuffer();
	_scaled_surface = ci::Surface8u::create(_scaled_image->readPixels8u(_scaled_image->getBounds()));
	_scaled_texture->update(*_scaled_surface);
}

cinder::Surface8uRef CameraCapture::scaled_surface() {
	return _scaled_surface;
}

cinder::gl::Texture2dRef CameraCapture::scaled_texture() {
	return _scaled_texture;//_scaled_image->getColorTexture();
}
