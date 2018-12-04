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
#include "CinderDlib.h"

/////////////////////////////////////////////
//
//
//    Utility class to execute capture gathering
//
//
//////////////////////////////////////////////

class CameraCapture{
public:
    CameraCapture(int width, int height);
    void update();
    cinder::gl::TextureRef texture();
    cinder::Surface8uRef surface();
    void print_faces();
    
private:
    cinder::Surface8uRef _surface;
    cinder::gl::TextureRef _texture;
    cinder::CaptureRef _capture;
	bool _read;
};


#endif
