#pragma once
#define _USE_MATH_DEFINES
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/VboMesh.h"

using namespace ci;
using namespace OpenMesh;
using namespace std;

namespace OpenMesh {
	template<>
	struct vector_traits<ci::vec3>
	{
		typedef vec3 vector_type;
		typedef float value_type;
		static const size_t size_ = sizeof(vec3);
		static size_t size() { return size_; }
	};
}

namespace OpenMesh {
	template<>
	struct vector_traits<ci::vec2>
	{
		typedef vec2 vector_type;
		typedef float value_type;
		static const size_t size_ = sizeof(vec2);
		static size_t size() { return size_; }
	};
}

struct CinderOMeshTraits : public OpenMesh::DefaultTraits
{
	typedef vec3    Point; // use double-values points
	typedef vec3    Normal;
	typedef vec2    TexCoord2D;
	typedef ColorA  Color;
};

typedef OpenMesh::TriMesh_ArrayKernelT<CinderOMeshTraits>  OMeshCinder;

class OMeshVBO {
public:
	OMeshVBO(int max_faces);
	OMeshCinder::VertexHandle add_point(vec3 point);
	OMeshCinder::FaceHandle   add_face(vector<OMeshCinder::VertexHandle> vertices);
	OMeshCinder::FaceHandle   add_face(OMeshCinder::VertexHandle v1, OMeshCinder::VertexHandle v2, OMeshCinder::VertexHandle v3);
	OMeshCinder::VertexHandle add_mirror_face(vec3 pt, OMeshCinder::HalfedgeHandle edge);
	OMeshCinder::HalfedgeHandle get_boundary();
	vec3 midpoint(OMeshCinder::HalfedgeHandle edge);
	vec3 out_vector(OMeshCinder::HalfedgeHandle edge, float distance);
	void set_tex_coord(vec2 tex_coord, OMeshCinder::VertexHandle h);
	void grow(float amt) {
		OMeshCinder::HalfedgeHandle boundry = get_boundary();
		vec3 new_pt = midpoint(boundry);
		new_pt += out_vector(boundry, amt);
		add_mirror_face(new_pt, boundry);
	}


	//ArrayKernel::FaceHandle   add_face_to_closest_boundary(vec3 pt);

	gl::VboMeshRef            mesh();
	void					  update_vbo();
	int size() { return _vertex_count; };

private:
	gl::VboMeshRef _vbo_mesh;
	OMeshCinder _o_mesh;
	int _vertex_count = 0;
};