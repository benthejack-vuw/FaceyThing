#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "CameraCapture.h"
// #include "FaceTracker.h"


using namespace ci;
using namespace ci::app;

class FaceyThingApp : public App {
  public:
	void setup() override;
    void update() override;
    void draw() override;

    std::shared_ptr<CameraCapture>	_capture;
    // std::shared_ptr<FaceTracker>    _tracker;
    //
    // std::vector<cinder::Rectf>       _faces;
};

void FaceyThingApp::setup()
{
    setWindowSize(1280, 720);
    _capture = std::shared_ptr<CameraCapture>( new CameraCapture(1280, 720) );
    // _tracker = std::shared_ptr<FaceTracker>( new FaceTracker() );
}

void FaceyThingApp::update(){
    _capture->update();
    // _faces = *_tracker->faces(_capture->surface());
}

void FaceyThingApp::draw()
{
	gl::clear();
    gl::color( ColorA( 1, 1, 1, 1 ) );
	gl::draw( _capture->texture());
  
	_capture->print_faces();

	//
  //   gl::color( ColorA( 1, 1, 0, 0.45f ) );
  //   for( std::vector<Rectf>::const_iterator faceIter = _faces.begin(); faceIter != _faces.end(); ++faceIter ){
  //       gl::drawSolidRect( *faceIter );
  //   }
}

CINDER_APP( FaceyThingApp, RendererGl )
