#include "FaceCollage.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

using namespace ci;

FaceCollage::FaceCollage(TrackedFace face, int part_count, float rotation_multiplier, int smooth_level){
	//this is a list of function pointers from facial components - the pointers are randomly assigned to each element

	std::vector< ci::Rectf(*)(std::vector<ci::vec2>) > dests;
	dests.push_back(&FacialComponents::left_half_dest);
	dests.push_back(&FacialComponents::right_half_dest);

	add_static_element(rotation_multiplier, smooth_level, dests.at(randInt(0,dests.size())));
	//add_static_element(rotation_multiplier, smooth_level, &FacialComponents::right_eye);
	//add_static_element(rotation_multiplier, smooth_level, &FacialComponents::mouth);

	//for (int i = 0; i < part_count; ++i) {
		//add_random_element(rotation_multiplier, smooth_level);
	//}
}

std::shared_ptr<CollageItem> FaceCollage::last_element() {
	return _elements.at(_elements.size() - 1);
}

void FaceCollage::add_static_element(float rotation_multiplier, float smooth_level, UpdateFunction update) {
	CollageItemStatic * cis = new CollageItemStatic(randFloat(3.14159 * rotation_multiplier), smooth_level);
	cis->position_update_function = update;
	CollageItemRef s(cis);
	_elements.push_back(s);
	set_update_func(last_element());
}

void FaceCollage::add_random_element(float rotation_multiplier, float smooth_level) {
	vec2 p(cosf(randFloat(0, 6.2831852)), sinf(randFloat(0, 6.2831852)));
	CollageItemRandom * cir = new CollageItemRandom(p, rotation_multiplier, smooth_level);
	_elements.push_back(CollageItemRef(cir));
	set_update_func(last_element());
}

void FaceCollage::set_update_func(CollageItemRef item) {
	std::vector< ci::Rectf(*)(std::vector<ci::vec2>) > update_functions = _components.update_functions();
	last_element()->update_function = update_functions.at(randInt(0, update_functions.size()));
}

void FaceCollage::update(TrackedFace this_face, const std::vector<TrackedFace> &all_faces){
	_bounds = this_face.bounds;
	for (int i = 0; i < _elements.size(); ++i) {
		_elements.at(i)->update(this_face, all_faces);
	}

}

void FaceCollage::draw(ci::gl::Texture2dRef tex, int element_count) {
	int draw_to = element_count >= 0 ? element_count : _elements.size();
	for (int i = 0; i < draw_to; ++i) {
		_elements.at(i)->draw(tex);
	}
}
