#include "CameraCapture.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

CameraCapture::CameraCapture(int width, int height){

    try {
        _capture = cinder::Capture::create( width, height);
        _capture->start();
    }
    catch( ci::Exception &exc ) {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc );
    }
    
    _surface = cinder::Surface8u::create(width, height, false);
    _texture = cinder::gl::Texture::create(width, height);
}

void CameraCapture::update(){
	if( _capture && _capture->checkNewFrame() ) {
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

