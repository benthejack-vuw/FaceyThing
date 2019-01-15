#include "opencv2/face.hpp"

class LandmarkDetector {
public:
	LandmarkDetector(std::string model_path);
	void detect(const cv::Mat &frame, std::vector<cv::Rect> regions);
	std::vector<std::vector<cv::Point2f>> landmarks();

private:
	cv::Ptr<cv::face::Facemark> _facemark;
	std::vector<std::vector<cv::Point2f>> _landmarks;
};