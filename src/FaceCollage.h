#include "CinderOpenCV.h"
#include "FaceTracker.h"
#include "FacialComponents.h"
#include "cinder/Rect.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "CollageItem.h"

#define CollageItemRef std::shared_ptr<CollageItem> 

class FaceCollage{
public:
	FaceCollage(TrackedFace face, int part_count, float rotation_multiplier, int smooth_level);
	void update(TrackedFace this_face, const std::vector<TrackedFace> &all_faces);
	void draw(ci::gl::Texture2dRef tex, int element_count = -1);

private:
	CollageItemRef last_element();
	void add_random_element(float rotation_multiplier, float smooth_level);
	void add_static_element(float rotation_multiplier, float smooth_level, UpdateFunction update);

	void FaceCollage::set_update_func(CollageItemRef item);

	std::vector<CollageItemRef> _elements;
	FacialComponents _components;
	ci::Rectf _bounds;
};
