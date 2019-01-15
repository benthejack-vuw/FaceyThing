#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "CinderOpenCV.h"
#include "OMeshVBO.h"

class EyePopper{
public:
	EyePopper();
	void draw(gl::Texture2dRef tex, Rectf eye);
	void update();

private:
	std::shared_ptr<OMeshVBO> _oMesh;
	std::vector<int> _randomized_indices;
	ci::gl::FboRef   _eye_texture;
	int _frame;
};