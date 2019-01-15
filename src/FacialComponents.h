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


#pragma once
#include "cinder/Vector.h"
#include "cinder/Rect.h"

struct VectorSubset {
	int start_index;
	int end_index;
};

class FacialComponents {
public:

	FacialComponents(int buffer_size = 30) :_buffer_size(buffer_size) {};

	static ci::Rectf left_eye(std::vector<ci::vec2> points);
	static ci::Rectf right_eye(std::vector<ci::vec2> points);
	static ci::Rectf nose(std::vector<ci::vec2> points);
	static ci::Rectf mouth(std::vector<ci::vec2> points);

	static std::vector<ci::vec2> left_eye_points(std::vector<ci::vec2> points);
	static std::vector<ci::vec2> right_eye_points(std::vector<ci::vec2> points);
	static std::vector<ci::vec2> jaw_points(std::vector<ci::vec2> points);
	static std::vector<ci::vec2>  eye_and_jaw_points(std::vector<ci::vec2> points);
	static std::vector<ci::Rectf> face_parts(std::vector<ci::vec2> points);
	static std::vector<ci::vec2>  face_part_centroids(std::vector<ci::vec2> points);

	static std::vector<ci::vec2> vertical_orientation_points(std::vector<ci::vec2> points);
	static std::vector<ci::vec2> horizontal_orientation_points(std::vector<ci::vec2> points);

	static std::vector<ci::Rectf> face_parts(std::vector <std::vector<ci::vec2>> faces);
	static std::vector<ci::Rectf> eyes(std::vector<std::vector<ci::vec2>> faces);
	static std::vector<ci::Rectf> FacialComponents::eyes(std::vector<ci::vec2> points);
	static std::vector<ci::vec2>  face_part_centroids(std::vector<std::vector<ci::vec2>> faces);
	std::vector<ci::Rectf> averaged_face_parts(std::vector <std::vector<ci::vec2>> faces);
	std::vector<ci::vec2> FacialComponents::averaged_centroids(std::vector <std::vector<ci::vec2>> faces);

	
	static std::vector<ci::vec2> point_list_subset(std::vector<ci::vec2> points, VectorSubset range);
	static std::vector<ci::vec2> point_list_subsets(std::vector<ci::vec2> points, std::vector<VectorSubset> ranges);

	static ci::Rectf bounds_of_point_list_subset(std::vector<ci::vec2> points, VectorSubset range, ci::vec2 expand = ci::vec2(0, 0));
	ci::Rectf FacialComponents::bounds_of_point_list_subsets(std::vector<ci::vec2> points, std::vector<VectorSubset> ranges, ci::vec2 expand);

private:
	std::vector < std::vector< std::vector<ci::Rectf > > > _running_averages;
	int _buffer_size, _frame_count;
};

