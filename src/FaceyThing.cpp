#include "FaceyThing.h"
#include "FacePainter.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"

FaceyThing::FaceyThing(int face_index):
	_face_index(face_index),
	marked_for_deletion(false),
	_fade(0),
	_fade_speed(0.01)
{
	CI_LOG_D("new");
}

void FaceyThing::setup_collage(TrackedFace &face, int part_count, float rotation_multiplier, int smooth_level) {
	_collage = std::shared_ptr<FaceCollage>(new FaceCollage(face, part_count, rotation_multiplier, smooth_level));
}

void FaceyThing::setup_paint_mesh(ci::vec2 camera_resolution, float fade_speed) {
	_painter_mesh = std::shared_ptr<FaceMesh>(new FaceMesh(camera_resolution));
	_fade_speed = fade_speed;
}

void FaceyThing::update(TrackedFace &face, std::vector<TrackedFace> &all_faces){
	_face = face;

	if (stage_2()) {
		_painter_mesh->update(face.landmarks);
		_collage->update(face, all_faces);
	}
}


void FaceyThing::draw_mesh(ci::gl::Texture2dRef texture) {
	if (stage_2()) {
		ci::gl::ScopedGlslProg glslScope(ci::gl::getStockShader(ci::gl::ShaderDef().texture()));
		texture->bind();
		_painter_mesh->draw();
	}
}

void FaceyThing::draw_mesh_to(std::shared_ptr<FacePainter>painter, ci::gl::Texture2dRef texture) {
	if (stage_2()) {
		painter->render_face(_painter_mesh, texture, _fade);
		_fade += _fade_speed;
		CI_LOG_D(_fade_in);
	}
}

void FaceyThing::draw_collage(ci::gl::Texture2dRef texture) {
	if (stage_2()) {
		_collage->draw(texture);
	}
}

void FaceyThing::draw_detection(ci::gl::Texture2dRef texture) {
	if (stage_1() && !stage_2()) {
		ci::Area source(_face.bounds);
		source.expand(source.getWidth() / 3.0, source.getHeight() / 3.0);

		ci::Area dest(_face.bounds);
		dest.expand(dest.getWidth()/1.5, dest.getHeight()/1.5);
		ci::gl::draw(texture, source, ci::Rectf(dest));

		ci::gl::color(ci::ColorA(158/255.0, 44/255.0, 160/255.0, 1.0));
		ci::gl::lineWidth(4);
		ci::gl::drawStrokedRect(dest);
		ci::gl::lineWidth(1);
		ci::gl::color(ci::ColorA(1,1,1,1));
	}
}


int FaceyThing::index() {
	return _face_index;
}

bool FaceyThing::stage_1() {
	return _face.bounds.calcArea() / camera_bounds.calcArea() > (stages[0] / 100.0);
}

bool FaceyThing::stage_2() {
	return _face.bounds.calcArea() / camera_bounds.calcArea() > (stages[1] / 100.0);
}

bool FaceyThing::stage_3() {
	return _face.bounds.calcArea() / camera_bounds.calcArea() > (stages[2] / 100.0);
}

int FaceyThing::stage() {

	if(stage_3()){
		return 3;
	}
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
