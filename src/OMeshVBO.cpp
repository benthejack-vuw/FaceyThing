#include "OMeshVBO.h"
#include "cinder/Log.h"

OMeshVBO::OMeshVBO(int max_faces) {

	CI_LOG_D("adding thingies");
	_o_mesh.request_vertex_colors();
	_o_mesh.request_vertex_texcoords2D();
	_o_mesh.request_vertex_normals();
	CI_LOG_D("added thingies");

	vector<gl::VboMesh::Layout> bufferLayout = {
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::COLOR, 4),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::TEX_COORD_0, 2),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::NORMAL, 3)
	};

	_vbo_mesh = gl::VboMesh::create(max_faces*3, GL_TRIANGLES, bufferLayout);
	vector<ColorA> colors   (max_faces, ColorA(1.0, 1.0, 1.0, 1.0));
	vector<vec2> tex_coords(max_faces, vec2(0.0, 0.0));
	vector<vec3> normals(max_faces, vec3(0.0, 0.0, 1.0));

	_vbo_mesh->bufferAttrib(geom::Attrib::COLOR, colors.size() * sizeof(ColorAf), colors.data());
	_vbo_mesh->bufferAttrib(geom::Attrib::TEX_COORD_0, tex_coords.size() * sizeof(vec2), tex_coords.data());
	_vbo_mesh->bufferAttrib(geom::Attrib::NORMAL, normals.size() * sizeof(vec3), normals.data());

}

OMeshCinder::VertexHandle OMeshVBO::add_point(vec3 point) {
	CI_LOG_D("add vertex");
	return _o_mesh.add_vertex(point);
}

OMeshCinder::FaceHandle   OMeshVBO::add_face(vector<OMeshCinder::VertexHandle> vertices) {
	return _o_mesh.add_face(vertices);
}

OMeshCinder::FaceHandle   OMeshVBO::add_face(OMeshCinder::VertexHandle v1, OMeshCinder::VertexHandle v2, OMeshCinder::VertexHandle v3) {
	CI_LOG_D("add face");
	return _o_mesh.add_face(v1, v2, v3);
}

OMeshCinder::VertexHandle   OMeshVBO::add_mirror_face(vec3 pt, OMeshCinder::HalfedgeHandle edge) {
	ArrayKernel::VertexHandle new_h    = _o_mesh.add_vertex(pt);
	ArrayKernel::VertexHandle to_h     = _o_mesh.to_vertex_handle(edge);
	ArrayKernel::VertexHandle from_h   = _o_mesh.from_vertex_handle(edge);
	_o_mesh.add_face(to_h, new_h, from_h);

	OMeshCinder::HalfedgeLoopIter itr = _o_mesh.hl_begin(_o_mesh.opposite_halfedge_handle(edge));
	itr++;
	vec2 tex_coord = _o_mesh.texcoord2D(_o_mesh.to_vertex_handle(itr.handle()));
	CI_LOG_D("NEW_TEX_POINT:" <<itr.is_valid()<< "    :    "<< tex_coord);
	_o_mesh.set_texcoord2D(new_h, tex_coord);

	return new_h;
}

void OMeshVBO::set_tex_coord(vec2 tex_coord, OMeshCinder::VertexHandle h) {
	_o_mesh.set_texcoord2D(h, tex_coord);
}

OMeshCinder::HalfedgeHandle OMeshVBO::get_boundary() {
	OMeshCinder::HalfedgeIter itr = _o_mesh.halfedges_begin();
	while (itr != _o_mesh.halfedges_end()) {
		if (_o_mesh.is_boundary(itr.handle())) {
			return itr.handle();
		}
		++itr;
	}
	return itr.handle();
}

vec3 OMeshVBO::midpoint(ArrayKernel::HalfedgeHandle edge) {
	vec3 to     = _o_mesh.point(_o_mesh.to_vertex_handle(edge));
	vec3 from   = _o_mesh.point(_o_mesh.from_vertex_handle(edge));
	return vec3(lerp(from.x, to.x, 0.5), lerp(from.y, to.y, 0.5), lerp(from.z, to.z, 0.5));
}

vec3 OMeshVBO::out_vector(ArrayKernel::HalfedgeHandle edge, float distance) {	
	vec3 to   = _o_mesh.point(_o_mesh.to_vertex_handle(edge));
	vec3 from = _o_mesh.point(_o_mesh.from_vertex_handle(edge));
	vec3 dir = from - to;
	float angle = atan2(dir.y, dir.x)-(3.14159/2.0);
	CI_LOG_D(angle);

	return vec3(cos(angle)*distance, sin(angle)*distance, 0);
}


gl::VboMeshRef            OMeshVBO::mesh() {
	return _vbo_mesh;
}

void OMeshVBO::update_vbo() {
	_vertex_count = 0;
	auto vbo_pos = _vbo_mesh->mapAttrib3f(geom::Attrib::POSITION, true);
	auto tex_pos = _vbo_mesh->mapAttrib2f(geom::Attrib::TEX_COORD_0, true);

	for (OMeshCinder::FaceIter f_it = _o_mesh.faces_begin(); f_it != _o_mesh.faces_end(); ++f_it) {
		CI_LOG_D("FACE");

		for (OMeshCinder::FaceVertexIter vtx = _o_mesh.fv_begin(f_it.handle()); vtx != _o_mesh.fv_end(f_it.handle()); ++vtx) {				
				*vbo_pos = _o_mesh.point(*vtx);
				++vbo_pos;
				*tex_pos = _o_mesh.texcoord2D(*vtx);
				CI_LOG_D(*tex_pos);
				++tex_pos;

				++_vertex_count;
			}
	}

	vbo_pos.unmap();
	tex_pos.unmap();
}
