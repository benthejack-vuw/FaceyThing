#include "FaceMesh.h"
#include "CinderOpenCV.h"
#include "cinder/gl/scoped.h"
#include "opencv2/imgproc.hpp"
#include "cinder/gl/gl.h"

#define MAX_FACES 300

using namespace ci;
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------------
std::vector<cv::Vec6f> triangulation(std::vector<ci::vec2> points, cv::Rect2f bounds);
bool triangle_in_bounds(cv::Vec6f verts, cv::Rect2f bounds);
cv::Rect2d expanded_cv_bounding_box(std::vector<ci::vec2> points);
std::vector<ci::vec3> vec3s_from_cvVec6f(cv::Vec6f verts, float depth);
//--------------------------------------------------------------------------------------------------------------------------------------

FaceMesh::FaceMesh( ci::vec2 camera_resolution):
_camera_resolution(camera_resolution)
{
	_depth = 0;
	setup_mesh();
}

void FaceMesh::setup_mesh() {

	vector<gl::VboMesh::Layout> bufferLayout = {
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::TEX_COORD_0, 2),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::CUSTOM_0, 2),
	};

	_face_mesh = gl::VboMesh::create(MAX_FACES * 3, GL_TRIANGLES, bufferLayout);
	vector<vec2> tex_coords(MAX_FACES, vec2(0.0, 0.0));
	_face_mesh->bufferAttrib(geom::Attrib::TEX_COORD_0, tex_coords.size() * sizeof(vec2), tex_coords.data());
	_face_mesh->bufferAttrib(geom::Attrib::CUSTOM_0, tex_coords.size() * sizeof(vec2), tex_coords.data());
}

void FaceMesh::update(std::vector<ci::vec2> points) {
	generate_delaunay_mesh(points);
}

void FaceMesh::set_face_height(float face_height) {
	_face_height = face_height;
}


void FaceMesh::generate_delaunay_mesh(std::vector<ci::vec2> points) {

	//reset count of how many triangles are in the mesh
	_tri_count = 0;

	cv::Rect2f bounds = expanded_cv_bounding_box(points);
	Area a = fromOcv(bounds);
	_translate_point = a.getCenter();
	_scale_amount = _face_height / a.getHeight();

	std::vector<cv::Vec6f> triangleList = triangulation(points, bounds);

	auto vbo_pos = _face_mesh->mapAttrib3f(geom::Attrib::POSITION, true);
	auto tex_pos = _face_mesh->mapAttrib2f(geom::Attrib::TEX_COORD_0, true);
	auto custom_pos = _face_mesh->mapAttrib2f(geom::Attrib::CUSTOM_0, true);

	for (size_t i = 0; i < triangleList.size(); i++)
	{
		std::vector<ci::vec3> verts = vec3s_from_cvVec6f(triangleList.at(i), _depth);
		// add triangles to vbo only if they are completely within the bounds
		if (triangle_in_bounds(triangleList.at(i), bounds))
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


void  FaceMesh::add_point_to_vboiter(ci::gl::VboMesh::MappedAttrib<ci::vec3> &vbo_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &tex_pos, ci::gl::VboMesh::MappedAttrib<ci::vec2> &custom_pos, vec3 p, ci::Area a) {

	float bx = lmap(p.x, (float)a.x1, (float)a.x2, 0.0f, 1.0f);
	float by = lmap(p.y, (float)a.y1, (float)a.y2, 0.0f, 1.0f);

	*vbo_pos = p;
	*tex_pos = vec2(p.x / _camera_resolution.x, 1.0 - p.y / _camera_resolution.y);
	*custom_pos = vec2(bx, by);
	++tex_pos;
	++vbo_pos;
	++custom_pos;
}


void FaceMesh::draw() {
	gl::pushMatrices();
	//gl::translate(_translate_point);
	//gl::scale(_scale_amount, _scale_amount);
	//gl::translate(-_translate_point);
	gl::draw(_face_mesh, 0, _tri_count * 3);
	gl::popMatrices();
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

std::vector<ci::vec3> vec3s_from_cvVec6f(cv::Vec6f verts, float depth) {
	std::vector<ci::vec3> out;
	out.emplace_back(verts[0], verts[1], depth);
	out.emplace_back(verts[2], verts[3], depth);
	out.emplace_back(verts[4], verts[5], depth);
	return out;
}