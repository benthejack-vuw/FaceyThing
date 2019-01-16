#pragma once

#include "FaceMesh.h"
#include "FacePainter.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

class FacePainter {
public:
	FacePainter(ci::vec2 window_resolution, ci::vec2 camera_resolution);

	void set_wipe_speed(float wipe_speed);
	void set_noise_scale(float noise_scale);
	void render_face(std::shared_ptr<FaceMesh>, ci::gl::Texture2dRef tex);
	void draw(ci::gl::Texture2dRef tex);
	void set_hue_rotate_speed(float hue_rotate_speed);


private:
	void setup_shaders();
	void setup_fbos();
	void FacePainter::swap_buffer();
	void bleed();
	int last_buffer();

	ci::vec2 _window_resolution, _camera_resolution;
	ci::gl::GlslProgRef _paint_shader, _bleed_shader;
	std::vector<ci::gl::FboRef> _fbos;
	ci::gl::Texture2dRef _noise_texture, _colour_overlay_texture;

	int _current_buffer;
	float _noise_progress, _hue, _hue_rotate_speed;
};