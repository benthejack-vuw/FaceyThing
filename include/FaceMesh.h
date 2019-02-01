#pragma once
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

class FaceMesh {
public:
	FaceMesh(ci::vec2 camera_resolution);
	void update(std::vector<ci::vec2> points);
	void draw();
	void set_face_height(float face_height);
	void set_hue_rotate_speed(float hue_rotate_speed);

private:
	void setup_mesh();
	void generate_delaunay_mesh(std::vector<ci::vec2> points);
	void add_point_to_vboiter(ci::gl::VboMesh::MappedAttrib<ci::vec3> &vbo_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &tex_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &custom_pos, ci::vec3 p, ci::Area a);

	ci::vec2 _camera_resolution;
	ci::gl::VboMeshRef _face_mesh;
	int _tri_count;
	ci::vec2 _translate_point;
	float _face_height, _scale_amount, _hue_rotate_speed, _hue_rotate, _depth;
};
