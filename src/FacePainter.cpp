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

#define FACE_SIZE_CUTOFF 10.0
#define MAX_FACES 300


//---------------------------------------------Helper function forward declarations---------------------------------------------------
std::vector<cv::Vec6f> triangulation(std::vector<ci::vec2> points, cv::Rect2f bounds);
void add_point_to_vboiter(ci::gl::VboMesh::MappedAttrib<ci::vec3> &vbo_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &tex_pos, vec3 p);
bool triangle_in_bounds(cv::Vec6f verts, cv::Rect2f bounds);
cv::Rect2d expanded_cv_bounding_box(std::vector<ci::vec2> points);
std::vector<ci::vec3> vec3s_from_cvVec6f(cv::Vec6f verts);
//-------------------------------------------------------------------------------------------------------------------------------------

FacePainter::FacePainter(vec2 window_resolution, vec2 camera_resolution):
	_window_resolution(window_resolution),
	_camera_resolution(camera_resolution),
	_noise_progress(0)
{
	setup_fbos();
	setup_shaders();
	setup_mesh();
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


void FacePainter::setup_mesh() {

	vector<gl::VboMesh::Layout> bufferLayout = {
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::TEX_COORD_0, 2),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::CUSTOM_0, 2),
	};

	_face_mesh = gl::VboMesh::create(MAX_FACES * 3, GL_TRIANGLES, bufferLayout);
	vector<vec2> tex_coords(MAX_FACES, vec2(0.0, 0.0));
	_face_mesh->bufferAttrib(geom::Attrib::TEX_COORD_0, tex_coords.size() * sizeof(vec2), tex_coords.data());
	_face_mesh->bufferAttrib(geom::Attrib::CUSTOM_0,    tex_coords.size() * sizeof(vec2), tex_coords.data());
}

void FacePainter::setup_fbos() {
	_fbos.push_back(gl::Fbo::create(_window_resolution.x, _window_resolution.y, true, false, false));
	_fbos.push_back(gl::Fbo::create(_window_resolution.x, _window_resolution.y, true, false, false));
}

void FacePainter::generate_delaunay_mesh(std::vector<ci::vec2> points) {

	//reset count of how many triangles are in the mesh
	_tri_count = 0;

	cv::Rect2f bounds = expanded_cv_bounding_box(points);
	Area a = fromOcv(bounds);
	Area screen = Area(0, 0, _camera_resolution.x, _camera_resolution.y);


	if (a.calcArea() > screen.calcArea() / FACE_SIZE_CUTOFF) {
		std::vector<cv::Vec6f> triangleList = triangulation(points, bounds);

		auto vbo_pos    = _face_mesh->mapAttrib3f(geom::Attrib::POSITION,    true);
		auto tex_pos    = _face_mesh->mapAttrib2f(geom::Attrib::TEX_COORD_0, true);
		auto custom_pos = _face_mesh->mapAttrib2f(geom::Attrib::CUSTOM_0,    true);

		for (size_t i = 0; i < triangleList.size(); i++)
		{
			std::vector<ci::vec3> verts = vec3s_from_cvVec6f(triangleList[i]);
			// add triangles to vbo only if they are completely within the bounds
			if (triangle_in_bounds(triangleList[i], bounds))
			{
				add_point_to_vboiter(vbo_pos, tex_pos, custom_pos, verts[0], a);
				add_point_to_vboiter(vbo_pos, tex_pos, custom_pos, verts[1], a);
				add_point_to_vboiter(vbo_pos, tex_pos, custom_pos, verts[2], a);
				++_tri_count;
			}
		}

		vbo_pos.unmap();
		tex_pos.unmap();
		custom_pos.unmap();

	}

}

void FacePainter::draw_to_fbo(gl::Texture2dRef tex) {

	{
		gl::pushMatrices();
		_fbos[_current_buffer]->bindFramebuffer();
		gl::ScopedViewport vp(_window_resolution);
		gl::setMatricesWindow(_window_resolution);

		gl::ScopedGlslProg glslScope(_paint_shader);
		tex->bind();
			gl::draw(_face_mesh, 0, _tri_count * 3);
		tex->unbind();

		_fbos[_current_buffer]->unbindFramebuffer();
		gl::popMatrices();
	}

}

void FacePainter::bleed() {
	gl::FboRef texture_fbo     = _fbos[last_buffer()];
	gl::FboRef draw_target_fbo = _fbos[_current_buffer];

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

ci::vec2 FacePainter::center(std::vector<ci::vec2>) {
	return vec2();
}

std::vector<ci::vec2> FacePainter::normalize_points(std::vector<ci::vec2>) {
	std::vector<ci::vec2> r;
	return r;
}

void FacePainter::draw(std::vector<std::vector<ci::vec2>> landmarks, gl::Texture2dRef tex) {

	for each (std::vector<ci::vec2> face in landmarks) {
		generate_delaunay_mesh(face);
		draw_to_fbo(tex);
	}

	_current_buffer = (_current_buffer + 1) % 2;

	_bleed_shader->uniform("noise_y", (float)((sinf(_noise_progress)+1)/2.0));
	bleed();
	_noise_progress == 0.0001;

	gl::draw(_fbos[_current_buffer]->getColorTexture());
}

int FacePainter::last_buffer() {
	return (_current_buffer + 1) % 2;
}

void  FacePainter::add_point_to_vboiter(ci::gl::VboMesh::MappedAttrib<ci::vec3> &vbo_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &tex_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &custom_pos, vec3 p, ci::Area a) {
	
	float bx = lmap(p.x, (float)a.x1, (float)a.x2, 0.0f, 1.0f);
	float by = lmap(p.y, (float)a.y1, (float)a.y2, 0.0f, 1.0f);

	*vbo_pos    = p;
	*tex_pos    = vec2(p.x / _camera_resolution.x, 1.0 - p.y / _camera_resolution.y);
	*custom_pos = vec2(bx, by);
	++tex_pos;
	++vbo_pos;
	++custom_pos;
}




//----------------------HELPER FUNCTIONS------------------------

std::vector<cv::Vec6f> triangulation(std::vector<ci::vec2> points, cv::Rect2f bounds) {

	cv::Subdiv2D subdiv(bounds);

	for each(auto p in points) {
		subdiv.insert(cv::Point2f(p.x, p.y));
	}

	vector<cv::Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	return triangleList;

}

bool triangle_in_bounds(cv::Vec6f verts, cv::Rect2f bounds) {
	cv::Point p1(cvRound(verts[0]), cvRound(verts[1]));
	cv::Point p2(cvRound(verts[2]), cvRound(verts[3]));
	cv::Point p3(cvRound(verts[4]), cvRound(verts[5]));

	return bounds.contains(p1) && bounds.contains(p2) && bounds.contains(p3);
}

cv::Rect2d expanded_cv_bounding_box(std::vector<ci::vec2> points) {
	Area a(points);
	a.expand(a.getWidth() / 2, a.getHeight() / 2);
	return toOcv(a);
}

std::vector<ci::vec3> vec3s_from_cvVec6f(cv::Vec6f verts) {
	std::vector<ci::vec3> out;
	out.emplace_back(verts[0], verts[1], 0);
	out.emplace_back(verts[2], verts[3], 0);
	out.emplace_back(verts[4], verts[5], 0);
	return out;
}