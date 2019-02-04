#include <fstream>
#include "json.hpp"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"

#include "CameraCapture.h"
#include "FaceRenderer.h"


using namespace ci;
using namespace ci::app;

using json = nlohmann::json;

class FaceyThingApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown(KeyEvent event) override;

private:
	std::shared_ptr<CameraCapture>	_capture;
	std::shared_ptr<FaceRenderer>	_face_renderer;

	json _settings;

};

void FaceyThingApp::keyDown(KeyEvent event) {
	if (event.getCode() == 27) {//esc key pressed
		quit();
	}
}

void FaceyThingApp::setup()
{
	randSeed(std::chrono::system_clock::now().time_since_epoch().count());
	hideCursor();

	std::ifstream raw_settings(loadAsset("settings.json")->getFilePath());
	raw_settings >> _settings;
	
	vec2 camera_resolution(_settings["camera"]["resolution"]["width"], _settings["camera"]["resolution"]["height"]);
	_capture = std::shared_ptr<CameraCapture>(new CameraCapture(camera_resolution, _settings["tracking"]["scale_factor"], _settings["camera"]["name"]));
	
	vec2 window_resolution;
	if (_settings["window"]["fullscreen"]) {
		setFullScreen(true);
		window_resolution = vec2(app::getWindowWidth(), app::getWindowHeight());
	}
	else {
		window_resolution = vec2(_settings["window"]["resolution"]["output_width"], _settings["window"]["resolution"]["output_height"]);
		setWindowSize(window_resolution);
	}

	_face_renderer = std::shared_ptr<FaceRenderer>(new FaceRenderer(_settings, window_resolution));

}

void FaceyThingApp::update(){
    _capture->update();
	if (_capture->frame_available()) {
		_face_renderer->update(_capture->flipped(), _capture->scaled_surface());
	}
}

void FaceyThingApp::draw() {
	_face_renderer->draw();
}

CINDER_APP( FaceyThingApp, RendererGl,  )
