#ifndef RETROFILTER_H
#define RETROFILTER_H

#include "abstract_filter.h"

class RetroFilter : public AbstractFilter {
public:
	cv::Mat filter(const cv::Mat &input);
};

#endif // RETROFILTER_H
