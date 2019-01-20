#include "FacialComponents.h"

//the following are the different components of the 68 point openCV facial landmark detector:
//00-16  Jaw line
//17-21  Left eyebrow
//22-26  Right eyebrow
//27-30  Nose bridge
//30-35  Lower nose
//36-41  Left eye
//42-47  Right Eye
//48-59  Outer lip
//60-67  Inner lip

VectorSubset jawRange          = { 00, 16 };
VectorSubset leftEyeRange      = { 36, 41 };
VectorSubset rightEyeRange     = { 42, 47 };
VectorSubset leftEyebrowRange  = { 17, 21 };
VectorSubset rightEyebrowRange = { 22, 26 };
VectorSubset noseRange         = { 27, 35 };
VectorSubset mouthRange        = { 48, 59 };


std::vector< ci::Rectf(*)(std::vector<ci::vec2>) > FacialComponents::update_functions() {
	std::vector< ci::Rectf(*)(std::vector<ci::vec2>) > out;
	out.push_back(&left_eye);
	out.push_back(&right_eye);
	out.push_back(&mouth);
	return out;
}


ci::Rectf FacialComponents::left_half_dest(std::vector<ci::vec2> points) {
	ci::Area a(points);
	a.expand(-a.getWidth() / 3, -a.getHeight() / 5.0);
	a.offset(ci::ivec2(a.getWidth()/2, - a.getHeight()/4));
	return ci::Rectf(a);
}

ci::Rectf FacialComponents::right_half_dest(std::vector<ci::vec2> points) {
	ci::Area a(points);
	a.expand(-a.getWidth() / 3, -a.getHeight() / 5.0);
	a.offset(ci::ivec2(-a.getWidth() / 2, -a.getHeight() / 4));
	return ci::Rectf(a);
}

ci::Rectf FacialComponents::left_eye(std::vector<ci::vec2> points) {
	return FacialComponents::bounds_of_point_list_subset(points, leftEyeRange, ci::vec2(0.5,0));
}

ci::Rectf FacialComponents::right_eye(std::vector<ci::vec2> points) {
	return FacialComponents::bounds_of_point_list_subset(points, rightEyeRange, ci::vec2(0.5, 0));
}

ci::Rectf FacialComponents::nose(std::vector<ci::vec2> points) {
	return FacialComponents::bounds_of_point_list_subset(points, noseRange, ci::vec2(0, 0));
}

ci::Rectf FacialComponents::mouth(std::vector<ci::vec2> points) {
	return FacialComponents::bounds_of_point_list_subset(points, mouthRange, ci::vec2(0, 0));
}

std::vector<ci::vec2>  FacialComponents::left_eye_points(std::vector<ci::vec2> points) {
	return FacialComponents::point_list_subset(points, leftEyeRange);
}

std::vector<ci::vec2>  FacialComponents::right_eye_points(std::vector<ci::vec2> points) {
	return FacialComponents::point_list_subset(points, rightEyeRange);
}

std::vector<ci::vec2>  FacialComponents::jaw_points(std::vector<ci::vec2> points) {
	return FacialComponents::point_list_subset(points, jawRange);
}

std::vector<ci::vec2>  FacialComponents::eye_and_jaw_points(std::vector<ci::vec2> points) {
	std::vector<VectorSubset> ranges;
	ranges.push_back(leftEyeRange);
	ranges.push_back(rightEyeRange);
	ranges.push_back(jawRange);
	return FacialComponents::point_list_subsets(points, ranges);
}

std::vector<ci::vec2> FacialComponents::vertical_orientation_points(std::vector<ci::vec2> points) {
	std::vector<ci::vec2> out;
	out.push_back(points[jawRange.start_index + 8]);
	out.push_back(points[noseRange.start_index]);
	return out;
}

std::vector<ci::vec2> FacialComponents::horizontal_orientation_points(std::vector<ci::vec2> points) {
	std::vector<ci::vec2> out;
	out.push_back(points[leftEyeRange.start_index]);
	out.push_back(points[rightEyeRange.start_index + 3]);
	return out;
}

std::vector<ci::Rectf> FacialComponents::eyes(std::vector <std::vector<ci::vec2>> faces) {
	std::vector<ci::Rectf> out;
	for each (std::vector<ci::vec2> points in faces) {
		std::vector<ci::Rectf> face_rects = eyes(points);
		out.insert(out.end(), face_rects.begin(), face_rects.end());
	}
	return out;
}

std::vector<ci::Rectf> FacialComponents::eyes(std::vector<ci::vec2> points) {
	std::vector<ci::Rectf> out;
	out.push_back(FacialComponents::left_eye(points));
	out.push_back(FacialComponents::right_eye(points));
	return out;
}

std::vector<ci::Rectf> FacialComponents::face_parts(std::vector<ci::vec2> points) {
	std::vector<ci::Rectf> out;
	if (points.size() == 68) {
		out.push_back(FacialComponents::left_eye(points));
		out.push_back(FacialComponents::right_eye(points));
		//out.push_back(FacialComponents::nose(points));
		out.push_back(FacialComponents::mouth(points));
	}
	return out;
}


std::vector<ci::vec2> FacialComponents::face_part_centroids(std::vector<ci::vec2> points) {
	std::vector<ci::vec2> out;
	for each(ci::Rectf r in face_parts(points)) {
		out.push_back(r.getCenter());
	}
	return out;
}



std::vector<ci::Rectf> FacialComponents::face_parts(std::vector <std::vector<ci::vec2>> faces) {
	std::vector<ci::Rectf> out;
	for each (std::vector<ci::vec2> points in faces){
		std::vector<ci::Rectf> face_rects = face_parts(points);
		out.insert(out.end(), face_rects.begin(), face_rects.end());
	}
	return out;
}

std::vector<ci::vec2> FacialComponents::face_part_centroids(std::vector <std::vector<ci::vec2>> faces) {
	std::vector<ci::vec2> out;
	for each (std::vector<ci::vec2> points in faces) {
		std::vector<ci::vec2> face_centroids = face_part_centroids(points);
		out.insert(out.end(), face_centroids.begin(), face_centroids.end());
	}
	return out;
}


std::vector<ci::vec2> FacialComponents::point_list_subset(std::vector<ci::vec2> points, VectorSubset range) {
	std::vector<ci::vec2> subset(points.begin() + range.start_index, points.begin() + range.end_index);
	return subset;
}

std::vector<ci::vec2> FacialComponents::point_list_subsets(std::vector<ci::vec2> points, std::vector<VectorSubset> ranges) {
	std::vector<ci::vec2> subset;
	for each(VectorSubset range in ranges) {
		subset.insert(subset.end(), points.begin() + range.start_index, points.begin() + range.end_index);
	}
	return subset;
}

ci::Rectf FacialComponents::bounds_of_point_list_subset(std::vector<ci::vec2> points, VectorSubset range, ci::vec2 expand) {
	ci::Area a(point_list_subset(points, range));
	a.expand(a.getWidth()*expand.x, a.getHeight()*expand.y);
	return ci::Rectf(a);
}

ci::Rectf FacialComponents::bounds_of_point_list_subsets(std::vector<ci::vec2> points, std::vector<VectorSubset> ranges, ci::vec2 expand) {
	ci::Area a(point_list_subsets(points, ranges));
	a.expand(a.getWidth()*expand.x, a.getHeight()*expand.y);
	return ci::Rectf(a);
}


std::vector<ci::vec2> FacialComponents::averaged_centroids(std::vector <std::vector<ci::vec2>> faces) {
	std::vector<ci::vec2> out;

	std::vector<ci::Rectf> rects = averaged_face_parts(faces);
	for each (ci::Rectf r in rects) {
		out.push_back(r.getCenter());
	}
	return out;
}

std::vector<ci::Rectf>  FacialComponents::averaged_face_parts(std::vector <std::vector<ci::vec2>> faces) {

	std::vector<ci::Rectf> out;
	try {
		for (int i = 0; i < faces.size(); ++i) {

			if (_running_averages.size() <= i) {
				_running_averages.push_back(std::vector <std::vector<ci::Rectf>>());
			}

			std::vector<ci::Rectf> face_rects = face_parts(faces.at(i));
			for (int j = 0; j < face_rects.size(); ++j) {

				if (_running_averages.at(i).size() <= j) {
					_running_averages.at(i).push_back(std::vector<ci::Rectf>());
				}

				if (_running_averages.at(i).at(j).size() < _buffer_size) {
					_running_averages.at(i).at(j).push_back(face_rects.at(j));
				}
				else {
					_running_averages.at(i).at(j).at(_frame_count%_buffer_size) = face_rects.at(j);
				}

				ci::Rectf average(0, 0, 0, 0);
				for (int k = 0; k < _running_averages.at(i).at(j).size(); k++) {
					average.x1 += _running_averages.at(i).at(j).at(k).x1;
					average.y1 += _running_averages.at(i).at(j).at(k).y1;
					average.x2 += _running_averages.at(i).at(j).at(k).x2;
					average.y2 += _running_averages.at(i).at(j).at(k).y2;
				}

				average.x1 /= (_running_averages.at(i).at(j).size() + 0.0);
				average.y1 /= (_running_averages.at(i).at(j).size() + 0.0);
				average.x2 /= (_running_averages.at(i).at(j).size() + 0.0);
				average.y2 /= (_running_averages.at(i).at(j).size() + 0.0);
				out.push_back(average);
			}
		}


		_frame_count++;
	}
	catch (const std::out_of_range& e) {}
	return out;
}