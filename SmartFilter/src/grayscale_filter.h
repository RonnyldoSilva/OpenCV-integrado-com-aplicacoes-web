#ifndef GRAYSCALEFILTER_H
#define GRAYSCALEFILTER_H

#include "abstract_filter.h"

class GrayscaleFilter : public AbstractFilter {
public:
	cv::Mat filter(const cv::Mat &input);
};

#endif // GRAYSCALEFILTER_H
