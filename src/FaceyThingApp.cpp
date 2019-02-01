#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/scoped.h"

#include "json.hpp"
#include "CameraCapture.h"
#include "FaceyThing.h"
#include "FacePainter.h"
#include "FaceTracker.h"

using namespace ci;
using namespace ci::app;

using json = nlohmann::json;

int FaceyThing::time_to_change;
Rectf FaceyThing::camera_bounds;

class FaceyThingApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown(KeyEvent event) override;

private:
	void render_to_fbo();
	void correlate_faces(std::vector<TrackedFace> &faces);
	FaceyThing &add_face(TrackedFace & tf);
	void mark_faces_for_deletion();
	void update_faces(std::vector<TrackedFace> &faces);
	void delete_marked_faces();
	int max_stage();

	std::shared_ptr<CameraCapture>	_capture;
	json _settings;
	vec2 _camera_resolution, _render_resolution, _window_resolution;

	std::shared_ptr<FaceTracker>    _tracker;
	std::shared_ptr <FacePainter>   _painter;
	std::vector<FaceyThing>         _facey_things;

	float _background_fade;
	float _scale_up;

	ci::gl::FboRef _screen_fbo;
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
	_camera_resolution   = vec2(_settings["camera"]["resolution"]["width"], _settings["camera"]["resolution"]["height"]);
	_capture = std::shared_ptr<CameraCapture>(new CameraCapture(_camera_resolution, _settings["tracking"]["scale_factor"], _settings["camera"]["name"]));
	_background_fade = 0.0;

	if (_settings["window"]["fullscreen"]) {
		setFullScreen(true);
		_window_resolution = vec2(app::getWindowWidth(), app::getWindowHeight());
	}
	else {
		_window_resolution = vec2(_settings["window"]["resolution"]["output_width"], _settings["window"]["resolution"]["output_height"]);
		setWindowSize(_window_resolution);
	}

	_render_resolution = vec2(_settings["window"]["resolution"]["render_width"], _settings["window"]["resolution"]["render_height"]);
	
	_screen_fbo = ci::gl::Fbo::create(_render_resolution.x, _render_resolution.y, false, false, false);

	_scale_up = (float)_settings["face"]["scale_up"];

	_tracker = std::shared_ptr<FaceTracker>(new FaceTracker(_settings["tracking"]["scale_factor"]));

	_painter = std::shared_ptr<FacePainter>(new FacePainter(_render_resolution, _camera_resolution));
	_painter->set_wipe_speed(_settings["painter"]["wipe_speed"]);
	_painter->set_noise_scale(_settings["painter"]["noise_scale"]);
	_painter->set_hue_rotate_speed((float)_settings["painter"]["hue_rotate_speed"]);

	FaceyThing::time_to_change = _settings["stages"]["timing"];
	FaceyThing::camera_bounds = Rectf(vec2(0,0),_camera_resolution);

}

void FaceyThingApp::update(){
    _capture->update();
	if (_capture->frame_available()) {
		try {
			_tracker->update(_capture->scaled_surface());
		}
		catch (cv::Exception e) {
			CI_LOG_D(e.msg);
		}
	}

	correlate_faces(_tracker->faces());
}

void FaceyThingApp::correlate_faces(std::vector<TrackedFace> &faces){
	mark_faces_for_deletion();
	update_faces(faces);
	delete_marked_faces();
}


void FaceyThingApp::mark_faces_for_deletion() {
	for (int i = 0; i < _facey_things.size(); ++i) {
		_facey_things.at(i).marked_for_deletion = true;
	}
}

void FaceyThingApp::update_faces(std::vector<TrackedFace> &faces) {
	for (int i = 0; i < faces.size(); ++i) {
		bool found = false;
		for (int j = 0; j < _facey_things.size(); ++j) {
			if (_facey_things.at(j).index() == faces.at(i).global_index) {
				_facey_things.at(j).update(faces.at(i), faces);
				_facey_things.at(j).marked_for_deletion = false;
				found = true;
			}
		}
		if (!found) {
			add_face(faces.at(i)).update(faces.at(i), faces);
		}
	}
}

int FaceyThingApp::max_stage() {
	int max = 0;
	int stage;
	for (int i = 0; i < _facey_things.size(); ++i) {
		stage = _facey_things.at(i).stage();
		max = stage > max ? stage : max;
	}
	return max;
}

FaceyThing & FaceyThingApp::add_face(TrackedFace & tf) {
	_facey_things.emplace_back(tf.global_index);
	_facey_things.at(_facey_things.size() - 1).setup_collage(
		tf,
		_settings["collage"]["box_count"],
		_settings["collage"]["rotation"],
		_settings["collage"]["smoothing"],
		_settings["collage"]["line_weight"]
	);

	_facey_things.at(_facey_things.size() - 1).setup_paint_mesh(
		_camera_resolution,
		_settings["background"]["bar_fade_speed"],
		_settings["background"]["bar_max_opacity"]
	);

	return _facey_things.at(_facey_things.size() - 1);
}

void FaceyThingApp::delete_marked_faces() {
	for (int i = 0; i < _facey_things.size(); ++i) {
		if (_facey_things.at(i).marked_for_deletion) {
			_facey_things.erase(_facey_things.begin() + i);
			i--;
		}
	}
}

void  FaceyThingApp::render_to_fbo() {
	_screen_fbo->bindFramebuffer();

	gl::enableAlphaBlending();

	gl::pushMatrices();
	gl::ScopedViewport vp(_render_resolution);
	gl::setMatricesWindow(_camera_resolution);

	gl::color(ci::ColorA(1, 1, 1, 1));
	gl::drawSolidRect(Rectf(vec2(0, 0), _camera_resolution));

	gl::color(ci::ColorA(1, 1, 1, 1.0 - _background_fade));
	//gl::color(ci::ColorA(1, 1, 1, 1));
	gl::draw(_capture->flipped());

	_screen_fbo->unbindFramebuffer();


	gl::color(ci::ColorA(1, 1, 1, 1));
	for (int i = 0; i < _facey_things.size(); ++i) {

		_screen_fbo->bindFramebuffer();

		gl::ScopedViewport vp4(_render_resolution);
		gl::setMatricesWindow(_camera_resolution);

		gl::pushMatrices();
		gl::translate(_facey_things.at(i).face().bounds.getCenter());
		gl::scale(_scale_up, _scale_up);
		gl::translate(-_facey_things.at(i).face().bounds.getCenter());
		_facey_things.at(i).draw_backbar();
		gl::popMatrices();

		_facey_things.at(i).draw_detection(_capture->flipped());


		_screen_fbo->unbindFramebuffer();


		gl::pushMatrices();
		gl::translate(_facey_things.at(i).face().bounds.getCenter());
		gl::scale(_scale_up, _scale_up);
		gl::translate(-_facey_things.at(i).face().bounds.getCenter());
		_facey_things.at(i).draw_mesh_to(_painter, _capture->flipped());
		gl::popMatrices();
	}

	_screen_fbo->bindFramebuffer();
	{
		gl::ScopedViewport vp2(_render_resolution);
		gl::setMatricesWindow(_render_resolution);
		_painter->draw(_capture->flipped());
	}


	gl::ScopedViewport vp3(_render_resolution);
	gl::setMatricesWindow(_camera_resolution);
	for (int i = 0; i < _facey_things.size(); ++i) {
		gl::pushMatrices();
		gl::translate(_facey_things.at(i).face().bounds.getCenter());
		gl::scale(_scale_up, _scale_up);
		gl::translate(-_facey_things.at(i).face().bounds.getCenter());
		_facey_things.at(i).draw_mesh(_capture->flipped());
		_facey_things.at(i).draw_collage(_capture->flipped());
		gl::popMatrices();
	}

	gl::popMatrices();

	if (max_stage() > 0) {
		_background_fade += (float)_settings["background"]["fade_speed"];
	}
	else {
		_background_fade -= (float)_settings["background"]["fade_speed"];
	}

	_background_fade = _background_fade > _settings["background"]["max_opacity"] ? _settings["background"]["max_opacity"] : _background_fade;
	_background_fade = _background_fade < _settings["background"]["min_opacity"] ? _settings["background"]["min_opacity"] : _background_fade;
	_screen_fbo->unbindFramebuffer();


}


void FaceyThingApp::draw()
{
	render_to_fbo();
	
	gl::pushMatrices();
	gl::ScopedViewport vp3(_window_resolution);
	gl::setMatricesWindow(_window_resolution);
	gl::draw( _screen_fbo->getColorTexture(), Area(0,0,_render_resolution.x, _render_resolution.y), Rectf(0,0,_window_resolution.x, _window_resolution.y));
	gl::popMatrices();


//	ci::gl::color(ci::ColorA(158 / 255.0, 44 / 255.0, 160 / 255.0, 1.0));
	ci::gl::color(ci::ColorA(1, 1, 1, 1));

	std::vector<cinder::Rectf> detected_rects = _tracker->subtraction_detection();
	for each(ci::Rectf r in detected_rects) {
		gl::drawSolidRect(r);
	}
	ci::gl::color(ci::ColorA(1,1,1,1));



}

CINDER_APP( FaceyThingApp, RendererGl,  )
