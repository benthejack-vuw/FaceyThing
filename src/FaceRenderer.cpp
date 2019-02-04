#include "FaceRenderer.h"



int FaceyThing::time_to_change;

FaceRenderer::FaceRenderer(json settings, vec2 window_resolution) :
	_settings(settings),
	_window_resolution(window_resolution),
	_background_fade(0.0)
{
	set_settings();
	setup_tracker();
	setup_painter();
	build_fbo();
}

void FaceRenderer::set_settings() {
	_camera_resolution = vec2(_settings["camera"]["resolution"]["width"], _settings["camera"]["resolution"]["height"]);
	_render_resolution = vec2(_settings["window"]["resolution"]["render_width"], _settings["window"]["resolution"]["render_height"]);
	FaceyThing::time_to_change = _settings["stages"]["timing"];
}

void FaceRenderer::setup_tracker() {
	_tracker = std::shared_ptr<FaceTracker>(new FaceTracker(_settings["tracking"]["scale_factor"]));
}

void FaceRenderer::setup_painter(){
	_painter = std::shared_ptr<FacePainter>(new FacePainter(_render_resolution, _camera_resolution));
	_painter->set_wipe_speed(_settings["painter"]["wipe_speed"]);
	_painter->set_noise_scale(_settings["painter"]["noise_scale"]);
	_painter->set_hue_rotate_speed((float)_settings["painter"]["hue_rotate_speed"]);
}

void FaceRenderer::build_fbo() {
	_screen_fbo = ci::gl::Fbo::create(_render_resolution.x, _render_resolution.y, false, false, false);
}

void FaceRenderer::update(gl::Texture2dRef camera_feed, Surface8uRef scaled_camera_feed) {
	_camera_feed = camera_feed;

	try {
		_tracker->update(scaled_camera_feed);
	}
	catch (cv::Exception e){}

	correlate_faces(_tracker->faces());
}

void FaceRenderer::correlate_faces(std::vector<TrackedFace> &faces) {
	//mark all faces for deletion
	mark_faces_for_deletion();
	
	//update all found faces and unmark them for deletion if found
	update_faces(faces);

	//faces that weren't found in update_faces will be deleted
	delete_marked_faces();
}

void FaceRenderer::mark_faces_for_deletion() {
	for (int i = 0; i < _facey_things.size(); ++i) {
		_facey_things.at(i).marked_for_deletion = true;
	}
}

void FaceRenderer::update_faces(std::vector<TrackedFace> &faces) {
	for (int i = 0; i < faces.size(); ++i) {
		bool found = false;
		for (int j = 0; j < _facey_things.size(); ++j) {
			//if it's not a new face update it
			if (_facey_things.at(j).index() == faces.at(i).global_index) { 
				_facey_things.at(j).update(faces.at(i), faces);
				_facey_things.at(j).marked_for_deletion = false;
				found = true;
			}
		}
		//if it's a new face add it
		if (!found) { 
			add_face(faces.at(i)).update(faces.at(i), faces);
		}
	}
}

int FaceRenderer::max_stage() {
	int max = 0;
	int stage;
	for (int i = 0; i < _facey_things.size(); ++i) {
		stage = _facey_things.at(i).stage();
		max = stage > max ? stage : max;
	}
	return max;
}

FaceyThing & FaceRenderer::add_face(TrackedFace & tf) {
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
		_painter,
		_settings["face"]["scale_up"],
		_settings["background"]["bar_fade_speed"],
		_settings["background"]["bar_max_opacity"]
	);

	return _facey_things.at(_facey_things.size() - 1);
}

void FaceRenderer::delete_marked_faces() {
	for (int i = 0; i < _facey_things.size(); ++i) {
		if (_facey_things.at(i).marked_for_deletion) {
			_facey_things.erase(_facey_things.begin() + i);
			i--;
		}
	}
}

void FaceRenderer::draw_camera_feed() {

	gl::pushMatrices();
		
		//set viewports
		gl::ScopedViewport vp(_render_resolution);
		gl::setMatricesWindow(_camera_resolution);

		//draw solid white background (gl::clear() not working correctly)
		gl::color(ci::ColorA(1, 1, 1, 1));
		gl::drawSolidRect(Rectf(vec2(0, 0), _camera_resolution));

		//draw the raw camera feed but transparent depending on background fade
		gl::enableAlphaBlending();
		gl::color(ci::ColorA(1, 1, 1, 1.0 - _background_fade));
		gl::draw(_camera_feed);

	gl::popMatrices();
}

void FaceRenderer::draw_faceythings_paint() {

	gl::pushMatrices();

		//set viewports
		gl::ScopedViewport vp(_render_resolution);
		gl::setMatricesWindow(_camera_resolution);

		gl::color(ci::ColorA(1, 1, 1, 1));
		for (int i = 0; i < _facey_things.size(); ++i) {
			_facey_things.at(i).draw_painter(_camera_feed);
		}

	gl::popMatrices();

}

void FaceRenderer::draw_faceythings_collage() {
	gl::pushMatrices();

		//set viewports
		gl::ScopedViewport vp3(_render_resolution);
		gl::setMatricesWindow(_camera_resolution);

		gl::color(ci::ColorA(1, 1, 1, 1));
		for (int i = 0; i < _facey_things.size(); ++i) {
			_facey_things.at(i).draw_collage(_camera_feed);
		}

	gl::popMatrices();
}


void FaceRenderer::draw_painter() {

	gl::pushMatrices();
		
		gl::ScopedViewport vp(_render_resolution);
		gl::setMatricesWindow(_render_resolution);

		_painter->draw(_camera_feed);

	gl::popMatrices();
}

void FaceRenderer::increment_background_fade() {

	if (max_stage() > 0) {
		_background_fade += (float)_settings["background"]["fade_speed"];
	}
	else {
		_background_fade -= (float)_settings["background"]["fade_speed"];
	}

	_background_fade = _background_fade > _settings["background"]["max_opacity"] ? _settings["background"]["max_opacity"] : _background_fade;
	_background_fade = _background_fade < _settings["background"]["min_opacity"] ? _settings["background"]["min_opacity"] : _background_fade;

}

void FaceRenderer::draw_render_fbo_to_window() {

	gl::pushMatrices();
		
		gl::ScopedViewport vp3(_window_resolution);
		gl::setMatricesWindow(_render_resolution);

		gl::draw(_screen_fbo->getColorTexture());
	
	gl::popMatrices();

}

void  FaceRenderer::render_to_fbo() {

	gl::ScopedFramebuffer screen(_screen_fbo);
	draw_camera_feed();
	draw_faceythings_paint();
	draw_painter();
	draw_faceythings_collage();
	increment_background_fade();

}


void FaceRenderer::draw()
{
	render_to_fbo();
	draw_render_fbo_to_window();
}