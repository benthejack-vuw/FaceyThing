#pragma once
#include "CinderOpenCV.h"
#include "FaceTracker.h"
#include "FacialComponents.h"
#include "cinder/Rect.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

typedef ci::Rectf(*UpdateFunction)(std::vector<ci::vec2>);

class CollageItem {
public:
	CollageItem(float rotation_multiplier, int smooth_level);
	void update(TrackedFace this_face, const std::vector<TrackedFace> &all_faces);
	void draw(ci::gl::Texture2dRef tex);

	UpdateFunction update_function;

protected:
	void update_source_position(const std::vector<TrackedFace> &all_faces);
	virtual void update_dest_position(TrackedFace &face) {};


	std::vector<ci::Rectf> _source_buffer, _dest_buffer;
	ci::Rectf _dest_position;
	ci::Area  _source_position;
	float _rotation;
	int _smooth_level, _source_frame_count, _dest_frame_count;
	int _other_face_seed;
};

class CollageItemStatic : public CollageItem{
public:
	CollageItemStatic(float rotation, int smooth_level);
	UpdateFunction position_update_function;

protected:
	void update_dest_position(TrackedFace &face);
};

class CollageItemRandom : public CollageItem {
public:
	CollageItemRandom(ci::vec2 position, float rotation, int smooth_level);
private:
	void update_dest_position(TrackedFace &face);
	ci::vec2 _position;
};