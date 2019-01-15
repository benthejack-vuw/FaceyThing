#include "FaceDistortionMesh.h"
#include "CinderOpenCV.h"
#include "cinder/gl/scoped.h"
#include "cinder/gl/GlslProg.h"
#include "opencv2/imgproc.hpp"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "FacialComponents.h"


using namespace ci;
using namespace std;

#define MAX_FACES 300

FacePainter::FaceDistortionMesh() {
	

	_fbo = gl::Fbo::create(1920, 1080, true, false, false);

	vector<gl::VboMesh::Layout> bufferLayout = {
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::TEX_COORD_0, 2),
	};

	_face_mesh = gl::VboMesh::create(MAX_FACES*3, GL_TRIANGLES, bufferLayout);
	vector<vec2> tex_coords(MAX_FACES, vec2(0.0, 0.0));
	_face_mesh->bufferAttrib(geom::Attrib::TEX_COORD_0, tex_coords.size() * sizeof(vec2), tex_coords.data());
}

void FacePainter::update(std::vector<ci::vec2> points) {
	generate_delaunay_mesh(points);
}

void FacePainter::addTriangle() {
	
	
	
}

void FacePainter::generate_delaunay_mesh(std::vector<ci::vec2> points) {
	Area a(points);
	vec2 c = a.getCenter();
	a.expand(a.getWidth() / 2, a.getHeight() / 2);
	cv::Rect bounds = toOcv(a);
	cv::Subdiv2D subdiv(bounds);

	for each(auto p in points) {
		subdiv.insert(cv::Point2f(p.x, p.y));		
	}

	vector<cv::Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<cv::Point> pt(3);

	auto vbo_pos = _face_mesh->mapAttrib3f(geom::Attrib::POSITION, true);
	auto tex_pos = _face_mesh->mapAttrib2f(geom::Attrib::TEX_COORD_0, true);
	_tri_count = 0;

	for (size_t i = 0; i < triangleList.size(); i++)
	{
		cv::Vec6f t = triangleList[i];
		pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

		// Draw rectangles completely inside the image.
		if (bounds.contains(pt[0]) && bounds.contains(pt[1]) && bounds.contains(pt[2]))
		{
			vec3 p1 = vec3(pt[0].x, pt[0].y, 0);
			vec3 p2 = vec3(pt[1].x, pt[1].y, 0);
			vec3 p3 = vec3(pt[2].x, pt[2].y, 0);

			*vbo_pos = p1;
			++vbo_pos;
			*tex_pos = vec2(pt[0].x/1920.0, 1.0- pt[0].y/1080.0);
			++tex_pos;

			*vbo_pos = p2;
			++vbo_pos;
			*tex_pos = vec2(pt[1].x / 1920.0, 1.0 - pt[1].y / 1080.0);
			++tex_pos;

			*vbo_pos = p3;
			++vbo_pos;
			*tex_pos = vec2(pt[2].x / 1920.0, 1.0 - pt[2].y / 1080.0);
			++tex_pos;

			++_tri_count;
		}
	}

	vbo_pos.unmap();
	tex_pos.unmap();



}




ci::vec2 FacePainter::center(std::vector<ci::vec2>) {
	return vec2();
}

std::vector<ci::vec2> FacePainter::normalize_points(std::vector<ci::vec2>) {
	std::vector<ci::vec2> r;
	return r;
}

void FacePainter::draw(gl::Texture2dRef tex) {
	

	{
		gl::pushMatrices();
		_fbo->bindFramebuffer();
			gl::ScopedViewport vp(_fbo->getSize());
			gl::setMatricesWindow(_fbo->getSize());
		
			gl::ScopedGlslProg glslScope(gl::getStockShader(gl::ShaderDef().texture()));
			tex->bind();
			gl::draw(_face_mesh, 0, _tri_count * 3);
			tex->unbind();


			_fbo->unbindFramebuffer();
		gl::popMatrices();
	}

	gl::draw(_fbo->getColorTexture());

}