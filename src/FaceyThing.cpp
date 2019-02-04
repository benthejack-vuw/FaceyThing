
#include "FaceyThing.h"
#include "FacePainter.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace gl;

FaceyThing::FaceyThing(int face_index):
	_face_index(face_index),
	marked_for_deletion(false),
	_fade(0),
	_fade_speed(0.01)
{
	_start_time = time(0);
}

void FaceyThing::setup_collage(TrackedFace &face, int part_count, float rotation_multiplier, int smooth_level, int line_weight) {
	_collage = std::shared_ptr<FaceCollage>(new FaceCollage(face, part_count, rotation_multiplier, smooth_level, line_weight));
	_line_weight = line_weight;
}

void FaceyThing::setup_paint_mesh(vec2 camera_resolution, std::shared_ptr <FacePainter> painter, float scale_up, float fade_speed, float max_fade) {
	_painter = painter;
	_painter_mesh = std::shared_ptr<FaceMesh>(new FaceMesh(camera_resolution));
	_fade_speed = fade_speed;
	_max_fade = max_fade;
	_scale_up = scale_up;
}

void FaceyThing::update(TrackedFace &face, std::vector<TrackedFace> &all_faces){
	_face = face;

	if (stage_2()) {
		_painter_mesh->update(face.landmarks);
		_collage->update(face, all_faces);
	}
}


void FaceyThing::draw_mesh(Texture2dRef texture) {
	if (stage_2()) {
		ScopedGlslProg glslScope(getStockShader(ShaderDef().texture()));
		texture->bind();
		_painter_mesh->draw();
	}
}

void FaceyThing::draw_mesh_to(std::shared_ptr<FacePainter>painter, Texture2dRef texture) {
	painter->render_face(_painter_mesh, texture, 1.0);
}

void FaceyThing::draw_backbar() {
	Area backbar = Area(_face.bounds);

	backbar.expand(backbar.getWidth()/3.0, app::getWindowHeight()*2);
	color(ColorA(1, 1, 1, _fade));
	drawSolidRect(backbar);
	color(ColorA(1, 1, 1, 1));

	_fade += _fade_speed;
	_fade = _fade > _max_fade ? _max_fade : _fade;
}

void FaceyThing::draw_detection(Texture2dRef texture) {
		Area source(_face.bounds);
		source.expand(source.getWidth() / 3.0, source.getHeight() / 3.0);

		Area dest(_face.bounds);
		dest.expand(dest.getWidth(), dest.getHeight());
		gl::draw(texture, source, Rectf(dest));

		color(ColorA(158/255.0, 44/255.0, 160/255.0, 1.0));
		lineWidth(_line_weight);
		drawStrokedRect(dest);
		lineWidth(1);
		color(ColorA(1,1,1,1));
}

void FaceyThing::scale_around_face(float scale) {
	gl::translate(face().bounds.getCenter());
	gl::scale(scale, scale);
	gl::translate(-face().bounds.getCenter());
}

void FaceyThing::draw_painter(Texture2dRef texture) {
	gl::pushMatrices();
		
		scale_around_face(_scale_up);
		
		draw_backbar();
		
		if (stage_2()) {
			draw_mesh_to(_painter, texture);
		}

	gl::popMatrices();

	if (stage_1()) {
		draw_detection(texture);
	}
}

void FaceyThing::draw_collage(Texture2dRef texture) {
	gl::pushMatrices();
		
		scale_around_face(_scale_up);
		draw_mesh(texture);
		_collage->draw(texture);

	gl::popMatrices();
}



int FaceyThing::index() {
	return _face_index;
}

bool FaceyThing::stage_1() {
	return difftime(time(0), _start_time) < time_to_change;
}

bool FaceyThing::stage_2() {
	return !stage_1();
}

int FaceyThing::stage() {
	if (stage_2()) {
		return 2;
	}
	if (stage_1()) {
		return 1;
	}
	return 0;
}

TrackedFace FaceyThing::face() {
	return _face;
}
