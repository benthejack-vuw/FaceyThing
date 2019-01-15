#pragma once

#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

class FacePainter {
public:
	FacePainter(ci::vec2 window_resolution, ci::vec2 camera_resolution);

	void set_wipe_speed(float wipe_speed);
	void set_noise_scale(float noise_scale);
	
	void draw(std::vector<std::vector<ci::vec2>> landmarks, ci::gl::Texture2dRef tex);

private:

	void setup_mesh();
	void setup_fbos();
	void setup_shaders();

	void generate_delaunay_mesh(std::vector<ci::vec2> points);
	ci::vec2 center(std::vector<ci::vec2>);
	std::vector<ci::vec2> normalize_points(std::vector<ci::vec2>);
	void draw_to_fbo(ci::gl::Texture2dRef tex);
	void bleed();
	int last_buffer();
	void add_point_to_vboiter(ci::gl::VboMesh::MappedAttrib<ci::vec3> &vbo_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &tex_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &custom_pos, ci::vec3 p, ci::Area a);


	ci::vec2 _window_resolution, _camera_resolution;
	ci::gl::GlslProgRef _paint_shader, _bleed_shader;
	std::vector<ci::gl::FboRef> _fbos;
	ci::gl::Texture2dRef _noise_texture, _colour_overlay_texture;
	ci::gl::VboMeshRef _face_mesh;
	int _tri_count, _current_buffer;
	float _noise_progress;
};