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

/////////////////////////////////////////////
//
//
//    Utility class to execute capture gathering
//
//
//////////////////////////////////////////////

class CameraCapture{
public:
    CameraCapture(ci::vec2 _resolution, std::string camera_name);
    void update();
    cinder::gl::TextureRef texture();
    cinder::Surface8uRef surface();
	bool frame_available();
    
private:
    cinder::Surface8uRef _surface;
    cinder::gl::TextureRef _texture;
    cinder::CaptureRef _capture;
	bool _frame_available;
};


#endif
