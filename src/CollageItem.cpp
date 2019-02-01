#include "CollageItem.h"
#include "FaceCollage.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

using namespace ci;

//-------------------------------------------------------------------------------------------------------------
void add_rect(ci::Rectf &a, ci::Rectf &b);
void div_rect(ci::Rectf &rect, float divisor);
ci::Rectf average_rects(const std::vector<Rectf> &rects);
ci::Rectf smooth(ci::Rectf new_position, std::vector<Rectf> &buffer, int smooth_level, int &counter);
void add_to_buffer(ci::Rectf rect, std::vector<Rectf> &buffer, int smooth_level, int &counter);
//-------------------------------------------------------------------------------------------------------------


CollageItem::CollageItem(float rotation_multiplier, int smooth_level, int line_weight) :
	_smooth_level(smooth_level),
	_source_frame_count(0),
	_dest_frame_count(0),
	_line_weight(line_weight)
{
	_rotation = randFloat(-3.14159*rotation_multiplier, 3.14159*rotation_multiplier);
	_other_face_seed = randInt(0, 10000);
}


CollageItemStatic::CollageItemStatic(float rotation, int smooth_level, int line_weight) :
	CollageItem(rotation, smooth_level, line_weight)
{}

CollageItemRandom::CollageItemRandom(vec2 position, float rotation, int smooth_level, int line_weight) :
	CollageItem(rotation, smooth_level, line_weight),
	_position(position)
{}

void CollageItem::update(TrackedFace this_face, const std::vector<TrackedFace> &all_faces) {
	update_dest_position(this_face);
	update_source_position(all_faces);
}

void CollageItem::update_source_position(const std::vector<TrackedFace> &all_faces) {

	std::vector<ci::vec2> other_face = all_faces.at(_other_face_seed % all_faces.size()).landmarks;
	Area new_source = Area(update_function(other_face));
	float ratio = _dest_position.getSize().x / _dest_position.getSize().y;
	new_source.expand(0, new_source.getWidth()*ratio);

	_source_position = Area(smooth(new_source, _source_buffer, _smooth_level, _source_frame_count));
}


void CollageItemStatic::update_dest_position(TrackedFace &this_face) {
	Area a = Area(position_update_function(this_face.landmarks));
	a.expand(a.getWidth()/3, a.getHeight() / 3);
	_dest_position = smooth(Rectf(a), _dest_buffer, _smooth_level, _dest_frame_count);
}

void CollageItemRandom::update_dest_position(TrackedFace &this_face){
	Rectf face_bounds = this_face.bounds;

	vec2 two = vec2(2.0, 2.0);
	vec2 source_size(_source_position.getSize());
	vec2 offset = source_size / two;
	vec2 face_center = face_bounds.getCenter();
	vec2 square_bounds = vec2(face_bounds.getSize().x, face_bounds.getSize().x);

	vec2 tl(face_center+(_position*square_bounds/vec2(3.0,3.0)) - offset);

	Rectf new_position(tl, tl+source_size);

	_dest_position = smooth(new_position, _dest_buffer, _smooth_level, _dest_frame_count);
}


void CollageItem::draw(ci::gl::Texture2dRef tex) {
	gl::pushMatrices();
	gl::translate(_dest_position.getCenter());
	gl::rotate(_rotation);
	gl::translate(-_dest_position.getCenter());
	gl::draw(tex, _source_position, _dest_position);
	ci::gl::color(ci::ColorA(158 / 255.0, 44 / 255.0, 160 / 255.0, 1.0));
	ci::gl::lineWidth(_line_weight);
	ci::gl::drawStrokedRect(_dest_position);
	ci::gl::lineWidth(1);
	ci::gl::color(ci::ColorA(1,1,1,1));

	gl::popMatrices();
}


ci::Rectf smooth(ci::Rectf new_position, std::vector<Rectf> &buffer, int smooth_level, int &counter) {
	add_to_buffer(new_position, buffer, smooth_level, counter);
	return average_rects(buffer);
}


void add_to_buffer(ci::Rectf rect, std::vector<Rectf> &buffer, int smooth_level, int &counter) {
	if (buffer.size() < smooth_level) {
		buffer.push_back(rect);
	}
	else {
		buffer.at(counter%buffer.size()) = rect;
	}
	++counter;
}


void add_rect(ci::Rectf &a, ci::Rectf &b) {
	a.x1 += b.x1;
	a.y1 += b.y1;
	a.x2 += b.x2;
	a.y2 += b.y2;
}

void div_rect(ci::Rectf &rect, float divisor) {
	rect.x1 /= divisor;
	rect.y1 /= divisor;
	rect.x2 /= divisor;
	rect.y2 /= divisor;
}

ci::Rectf average_rects(const std::vector<Rectf> &rects) {
	Rectf average(0,0,0,0);
	for each(Rectf r in rects) {
		add_rect(average, r);
	}
	div_rect(average, rects.size());
	return average;
}