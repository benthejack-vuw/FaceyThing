#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/pixel.h>

#include "CameraCapture.h"
#include "cinder/Log.h"
#include "CinderDlib.h"

CameraCapture::CameraCapture(int width, int height){
    try {
        _capture = cinder::Capture::create( width, height, cinder::Capture::getDevices()[1]);
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
        std::cout << _surface->getWidth() << "  :  " << _surface->getHeight() << "\n";
    }
}

cinder::gl::TextureRef CameraCapture::texture(){
    return _texture;
}

cinder::Surface8uRef CameraCapture::surface(){
    return _surface;
}

void CameraCapture::print_faces(){
    frontal_face_detector detector = get_frontal_face_detector();
    array2d<rgb_pixel> img;
    
    ci::toDlib(_surface, img);
    std::cout << "detecting" <<"\n";
    std::vector<rectangle> dets = detector(img);
    std::cout << "Number of faces detected: " << dets.size() << std::endl;
    
}
