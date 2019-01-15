#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "CinderOpenCV.h"
#include "FacialComponents.h"

class FaceCollage{
public:
	FaceCollage(int extra_parts = 4);

	void set_extra_boxes_count(int part_count);
	void set_rotation_amount(float rotation_amount);

	void draw(ci::gl::Texture2dRef tex, std::vector<ci::Rectf> faces, std::vector<std::vector<ci::vec2>> points);
private:
	std::vector<int> _randomized_indices;
	FacialComponents _components;
	int   _extra_part_count;
	float _rotation_amount;
};