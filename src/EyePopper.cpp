#include "EyePopper.h"
#include "FacialComponents.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"

using namespace ci::app;
using namespace ci;

EyePopper::EyePopper(){

	_eye_texture = gl::Fbo::create(300, 150, false, false, false);
	_oMesh = std::shared_ptr<OMeshVBO>(new OMeshVBO(300));

	OMeshCinder::VertexHandle v1 = _oMesh->add_point(vec3(cos(3.14159 * 2 * (1.0 / 3.0)), sin(3.14159 * 2 * (1.0 / 3.0)), -1));
	_oMesh->set_tex_coord(vec2(0, 0), v1);

	OMeshCinder::VertexHandle v2 = _oMesh->add_point(vec3(cos(3.14159 * 2 * (2.0 / 3.0)), sin(3.14159 * 2 * (2.0 / 3.0)), -1));
	_oMesh->set_tex_coord(vec2(1, 0), v2);

	OMeshCinder::VertexHandle v3 = _oMesh->add_point(vec3(cos(3.14159 * 2 * (3.0 / 3.0)), sin(3.14159 * 2 * (3.0 / 3.0)), -1));
	_oMesh->set_tex_coord(vec2(0.5, 1), v3);

	_oMesh->add_face(v1, v2, v3);
}

void EyePopper::update() {
	if (_frame % 25 == 0 && _frame < 25*15) {
		_oMesh->grow(randFloat(1,1.5));
		_oMesh->update_vbo();
	}
	_frame++;
}

void EyePopper::draw(gl::Texture2dRef tex, Rectf eye) {

	{
		gl::pushMatrices();
		_eye_texture->bindFramebuffer();
		gl::ScopedViewport vp(_eye_texture->getSize());
		gl::setMatricesWindow(_eye_texture->getSize());
		Area a(eye);
		a.expand(a.getWidth() / 2.0, a.getHeight() / 2.0);
		Rectf r = Rectf(0, 0, 300, 150);
		gl::draw(tex, a, r);
		_eye_texture->unbindFramebuffer();
		gl::popMatrices();
	}

	gl::pushMatrices();
	gl::ScopedGlslProg glslScope(gl::getStockShader(gl::ShaderDef().texture()));
	_eye_texture->bindTexture();
	gl::translate(eye.getCenter());
	gl::scale(70, 70);
	gl::draw(_oMesh->mesh(), 0, _oMesh->size());
	_eye_texture->unbindTexture();
	gl::popMatrices();

	//gl::draw(_eye_texture->getColorTexture(), Area(Rectf(vec2(0, 0), _eye_texture->getSize())), Rectf(vec2(0, 0), _eye_texture->getSize()));

}
