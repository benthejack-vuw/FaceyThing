#pragma once
#include "json.hpp"

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/scoped.h"
#include "cinder/gl/Texture.h"

#include "FaceyThing.h"
#include "FacePainter.h"
#include "FaceTracker.h"

using namespace ci;
using json = nlohmann::json;
//using namespace ci::app;

using json = nlohmann::json;

class FaceRenderer
{
public:
	FaceRenderer(json settings, vec2 window_resolution);
	void update(gl::Texture2dRef camera_feed, Surface8uRef scaled_camera_feed);
	void draw();

private:
	void set_settings();
	void setup_painter();
	void setup_tracker();
	void build_fbo();

	void render_to_fbo();
	void correlate_faces(std::vector<TrackedFace> &faces);
	FaceyThing &add_face(TrackedFace & tf);
	void mark_faces_for_deletion();
	void update_faces(std::vector<TrackedFace> &faces);
	void delete_marked_faces();
	int max_stage();

	void draw_camera_feed();
	void draw_faceythings_paint();
	void draw_faceythings_collage();
	void draw_painter();
	void increment_background_fade();
	void draw_render_fbo_to_window();



	vec2 _camera_resolution, _render_resolution, _window_resolution;

	std::shared_ptr<FaceTracker>    _tracker;
	std::shared_ptr <FacePainter>   _painter;
	std::vector<FaceyThing>         _facey_things;

	float _background_fade;
	ci::gl::FboRef       _screen_fbo;
	ci::gl::Texture2dRef _camera_feed;

	json _settings;
};

