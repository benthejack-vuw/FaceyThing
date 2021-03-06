#include "opencv2/face.hpp"

class LandmarkDetector {
public:
	LandmarkDetector(std::string model_path);
	std::vector<std::vector<cv::Point2f>> detect(const cv::Mat &frame, std::vector<cv::Rect> regions);
private:
	cv::Ptr<cv::face::Facemark> _facemark;
};