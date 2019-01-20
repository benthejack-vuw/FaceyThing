#include "FacePainter.h"
#include "CinderOpenCV.h"
#include "cinder/gl/scoped.h"
#include "cinder/gl/GlslProg.h"
#include "opencv2/imgproc.hpp"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

#include "FacialComponents.h"

using namespace ci;
using namespace std;

FacePainter::FacePainter(vec2 window_resolution, vec2 camera_resolution):
	_window_resolution(window_resolution),
	_camera_resolution(camera_resolution),
	_noise_progress(0),
	_current_buffer(0),
	_hue(0)
{
	setup_fbos();
	setup_shaders();
}

void FacePainter::setup_fbos() {
	_fbos.push_back(gl::Fbo::create( _window_resolution.x, _window_resolution.y, true, true, false));
	_fbos.push_back(gl::Fbo::create(_window_resolution.x, _window_resolution.y, true, true, false));
}

void FacePainter::set_wipe_speed(float wipe_speed) {
	_bleed_shader->uniform("wipe_speed", wipe_speed);
}

void FacePainter::set_noise_scale(float noise_scale) {
	_bleed_shader->uniform("noise_scale", noise_scale);
}

void FacePainter::setup_shaders() {

	try {
		auto noise_img = loadImage(ci::app::loadAsset("perlin_noise.png"));
		_noise_texture = gl::Texture2d::create(noise_img);
		_noise_texture->bind(1);

		auto colour_overlay_img = loadImage(ci::app::loadAsset("gradients/one.jpg"));
		_colour_overlay_texture = gl::Texture2d::create(colour_overlay_img);
		_colour_overlay_texture->bind(2);


		auto bleed_vert = ci::app::loadAsset("empty.vert");
		auto bleed_frag = ci::app::loadAsset("bleed_shader.frag");
		auto bleed_format = gl::GlslProg::Format();
		bleed_format.vertex(bleed_vert);
		bleed_format.fragment(bleed_frag);
		_bleed_shader = gl::GlslProg::create(bleed_format);
		_bleed_shader->uniform("previous_frame",  0);
		_bleed_shader->uniform("noise_texture",   1);
		_bleed_shader->uniform("wipe_speed", 0.0005f);
		_bleed_shader->uniform("noise_scale", 0.004f);

		auto paint_vert = app::loadAsset("paint_mesh.vert");
		auto paint_frag = app::loadAsset("paint_mesh.frag");
		auto paint_format = gl::GlslProg::Format();
		paint_format.vertex(paint_vert);
		paint_format.fragment(paint_frag);
		paint_format.attrib(cinder::geom::CUSTOM_0, "bounds_pos");

		_paint_shader = gl::GlslProg::create(paint_format);
		_paint_shader->uniform("texture", 0);
		_paint_shader->uniform("color_overlay", 2);
	}
	catch(cinder::gl::GlslProgCompileExc e)
	{
		CI_LOG_EXCEPTION("compile error:", e);
		std::exit(0);
	}

}



void FacePainter::render_face(std::shared_ptr<FaceMesh> face, ci::gl::Texture2dRef tex, float fade){
	{
		tex->bind();
		_fbos.at(_current_buffer)->bindFramebuffer();
		gl::ScopedGlslProg glslScope(_paint_shader);
		_paint_shader->uniform("hue_rotate", _hue);
		_paint_shader->uniform("fade", fade);
		face->draw();
		_fbos.at(_current_buffer)->unbindFramebuffer();
		tex->unbind();
	}


	_hue += _hue_rotate_speed;
	_hue = _hue > 1 ? _hue - 1 : _hue;
}

void FacePainter::bleed() {
	gl::FboRef texture_fbo     = _fbos.at(last_buffer());
	gl::FboRef draw_target_fbo = _fbos.at(_current_buffer);

	gl::pushMatrices();
	gl::ScopedFramebuffer fbScp(draw_target_fbo);
	gl::ScopedViewport vp(_window_resolution);
	gl::setMatricesWindow(_window_resolution);

	gl::clear(ci::ColorA::zero(), false);
	gl::ScopedGlslProg prog(_bleed_shader);
	texture_fbo->bindTexture();
		gl::drawSolidRect(Rectf(0, 0, draw_target_fbo->getSize().x, draw_target_fbo->getSize().y));
	texture_fbo->unbindTexture();
	gl::popMatrices();
}

void FacePainter::swap_buffer() {
	_current_buffer = (_current_buffer + 1) % 2;
}

void FacePainter::draw(gl::Texture2dRef tex) {	
	swap_buffer();

	_bleed_shader->uniform("noise_y", (float)((sinf(_noise_progress)+1)/2.0));
	bleed();
	//_noise_progress += 0.0001;

	gl::ScopedViewport vp(_window_resolution);
	gl::setMatricesWindow(_window_resolution);
	gl::draw(_fbos.at(_current_buffer)->getColorTexture());
}

int FacePainter::last_buffer() {
	return (_current_buffer + 1) % 2;
}


void FacePainter::set_hue_rotate_speed(float hue_rotate_speed) {
	_hue_rotate_speed = hue_rotate_speed;
}

