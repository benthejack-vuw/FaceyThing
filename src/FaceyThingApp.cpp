#include "json.hpp"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "CinderOpenCV.h"

#include "CameraCapture.h"
#include "FaceTracker.h"
#include "FacialComponents.h"
#include "FacePainter.h"
#include "FaceCollage.h"
#include "cinder/Rand.h"

#include "FacialComponents.h"
#include "OMeshVBO.h"

using namespace ci;
using namespace ci::app;

using json = nlohmann::json;

class FaceyThingApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;


	std::shared_ptr<CameraCapture>	_capture;
	std::shared_ptr<FaceTracker>    _tracker;
	std::shared_ptr<FaceCollage>    _collage;
	std::shared_ptr <FacePainter>   _painter;

	json _settings;

	vec2 _camera_resolution, _window_resolution;
};

void FaceyThingApp::setup()
{
	ifstream raw_settings(loadAsset("settings.json")->getFilePath());
	raw_settings >> _settings;
	_camera_resolution   = vec2(_settings["camera"]["resolution"]["width"], _settings["camera"]["resolution"]["height"]);

	if (_settings["window"]["fullscreen"]) {
		setFullScreen(true);
		_window_resolution = vec2(app::getWindowWidth(), app::getWindowHeight());
	}
	else {
		_window_resolution = vec2(_settings["window"]["resolution"]["width"], _settings["window"]["resolution"]["height"]);
		setWindowSize(_window_resolution);
	}


    _capture = std::shared_ptr<CameraCapture>( new CameraCapture(_camera_resolution, _settings["camera"]["name"]) );
    _tracker = std::shared_ptr<FaceTracker>  ( new FaceTracker(_settings["tracking"]["scale_factor"]) );

	_painter = std::shared_ptr<FacePainter>  ( new FacePainter(_window_resolution, _camera_resolution ) );
		_painter->set_wipe_speed( _settings["painter"]["wipe_speed"] );
		_painter->set_noise_scale( _settings["painter"]["noise_scale"] );

	_collage = std::shared_ptr<FaceCollage>  ( new FaceCollage() );
		_collage->set_extra_boxes_count(_settings["collage"]["extra_boxes"]);
		_collage->set_rotation_amount(_settings["collage"]["rotation_amount"]);
}

void FaceyThingApp::update(){
    _capture->update();
	if (_capture->frame_available()) {
		try {
			_tracker->update(_capture->surface());
		}
		catch (cv::Exception e) {
			CI_LOG_D(e.msg);
		}
	}

}

void FaceyThingApp::draw()
{
	gl::ScopedMatrices();
	gl::ScopedViewport vp(_window_resolution);
	gl::setMatricesWindow(_camera_resolution);


	gl::color(ColorA(1, 1, 1,1));
	gl::drawSolidRect(Rectf(0, 0, _window_resolution.x, _window_resolution.y));

	
    gl::color( ColorA( 1, 1, 1, 0.2 ) );	
	gl::draw( _capture->texture());

	gl::color(ColorA(1, 1, 1, 1));

	std::vector<std::vector<ci::vec2>> landmarks = _tracker->screenspace_facial_landmarks();
	
	_painter->draw(landmarks, _capture->texture());
	_collage->draw(_capture->texture(), _tracker->screenspace_tracker_rects(), landmarks);
	
}

CINDER_APP( FaceyThingApp, RendererGl )
