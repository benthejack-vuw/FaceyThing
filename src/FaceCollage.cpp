#include "FaceCollage.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

using namespace ci;

FaceCollage::FaceCollage(int extra_parts) : _extra_part_count(extra_parts) {
	randSeed(std::chrono::system_clock::now().time_since_epoch().count());
	for (int i = 0; i < 100000; ++i) {
		_randomized_indices.push_back(randInt(100000));
	}
}

void FaceCollage::set_extra_boxes_count(int part_count) {
	_extra_part_count = part_count;
}

void FaceCollage::set_rotation_amount(float rotation_amount) {
	_rotation_amount = rotation_amount;
}

void FaceCollage::draw(gl::Texture2dRef tex, std::vector<ci::Rectf> faces, std::vector<std::vector<ci::vec2>> points) {
	std::vector<ci::Rectf> parts = _components.averaged_face_parts(points);
	std::vector<ci::vec2>  main_locations = _components.averaged_centroids(points);


	gl::color(ColorA(1, 1, 1, 1));
	for (int i = 0; i < faces.size(); ++i) {

		for (int j = 0; j < _extra_part_count; ++j) {
			Area part = Area(parts[_randomized_indices[i*j+(i+j)] % (int)parts.size()]);
			float d = (_randomized_indices[i*j + (i + j)] / 100000.0+0.3)*faces[i].getWidth()/2.0;
			float t = (_randomized_indices[i*j + (i + j)*2] / 100000.0)*6.2831852;
			vec2 loc = faces[i].getCenter() + vec2(cos(t)*d, sin(t)*d);
			Area dest = Area(loc.x - part.getWidth() / 2.0, loc.y - part.getHeight() / 2.0, loc.x + part.getWidth() / 2.0, loc.y + part.getHeight() / 2.0);
			
			float e = _randomized_indices[i+j] / 100000.0;
			dest.expand(dest.getWidth()*e*0.3, dest.getHeight()*e*0.3);


			gl::pushMatrices();
			gl::translate(dest.getCenter());
			gl::rotate(3.1415926 * _rotation_amount * (0.5-(_randomized_indices[(i*j+i+j*3 * 100) % _randomized_indices.size()] / 100000.0)));
			gl::translate(-dest.getCenter());
			gl::draw(tex, part, Rectf(dest));
			gl::popMatrices();
		}

	}


	for (int i = 0; i < main_locations.size(); ++i) {
		Area part = Area(parts[_randomized_indices[i] % (int)parts.size()]);
		Area dest = Area(main_locations[i].x - part.getWidth()/2.0, main_locations[i].y - part.getHeight() / 2.0, main_locations[i].x + part.getWidth() / 2.0, main_locations[i].y + part.getHeight() / 2.0);
		dest.expand(dest.getWidth()*0.5, dest.getHeight()*0.5);
		
		gl::pushMatrices();
		gl::translate(dest.getCenter());
		gl::rotate(3.1415926 *_rotation_amount* (0.5-(_randomized_indices[(i*100)%_randomized_indices.size()]/100000.0)));
		gl::translate(-dest.getCenter());
		gl::draw(tex, part, Rectf(dest));
		gl::popMatrices();
	}
}