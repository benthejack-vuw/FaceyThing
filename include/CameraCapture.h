//
//  CameraCapture.h
//  FaceyThing
//
//  Created by Ben Jack on 11/09/18.
//

#ifndef CameraCapture_h
#define CameraCapture_h

#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Capture.h"
#include "cinder/gl/Fbo.h"

/////////////////////////////////////////////
//
//
//    Utility class to execute capture gathering
//
//
//////////////////////////////////////////////

class CameraCapture{
public:
    CameraCapture(ci::vec2 _resolution, float scale_down, std::string camera_name);
    void update();
    cinder::gl::TextureRef flipped();
    cinder::Surface8uRef surface();
	cinder::Surface8uRef scaled_surface();
	cinder::gl::Texture2dRef CameraCapture::scaled_texture();
	bool frame_available();
    
private:
	void scale_down();
	void flip_buffer();

	ci::gl::FboRef _scaled_image, _flipped_image;
    cinder::Surface8uRef _surface, _scaled_surface;
    cinder::gl::TextureRef _texture, _scaled_texture;
    cinder::CaptureRef _capture;
	bool _frame_available;
};


#endif
