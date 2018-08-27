#ifndef EDGEFILTER_H
#define EDGEFILTER_H

#include "abstract_filter.h"

class EdgeFilter : public AbstractFilter {
public:
	cv::Mat filter(const cv::Mat &input);
};

#endif // EDGEFILTER_H
