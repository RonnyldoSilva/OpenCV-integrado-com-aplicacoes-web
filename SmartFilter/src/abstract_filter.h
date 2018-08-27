#ifndef ABSTRACTFILTER_H
#define ABSTRACTFILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class AbstractFilter {
public:
	virtual cv::Mat filter(const cv::Mat &input) = 0;
};

#endif // ABSTRACTFILTER_H
