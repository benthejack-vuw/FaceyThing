#include "FaceTracker.h"
#include "FaceCollage.h"
#include "FacePainter.h"
#include "FaceMesh.h"
#include "cinder/Rect.h"
#include "cinder/gl/Texture.h"

class FaceyThing {
public:
	static std::vector<int> stages;
	static ci::Rectf camera_bounds; //CAMERA frame not window frame

	FaceyThing(int face_index);
	void setup_collage(TrackedFace &face, int part_count, float rotation_multiplier, int smooth_level);
	void setup_paint_mesh(ci::vec2 camera_resolution, float fade_speed, float max_fade);

	int  index();
	void update(TrackedFace &face, std::vector<TrackedFace> &all_faces);
	void draw_mesh_to(std::shared_ptr<FacePainter> painter, ci::gl::Texture2dRef texture);
	void draw_mesh(ci::gl::Texture2dRef texture);
	void draw_collage(ci::gl::Texture2dRef texture);
	void draw_detection(ci::gl::Texture2dRef texture);
	void draw_backbar();



	bool stage_1();
	bool stage_2();
	bool stage_3();
	int stage();

	TrackedFace face();

	bool marked_for_deletion;
private:

	float _fade, _max_fade;
	float _fade_speed;
	int _face_index;
	TrackedFace _face;
	std::shared_ptr<FaceCollage> _collage;
	std::shared_ptr<FaceMesh>    _painter_mesh;
};