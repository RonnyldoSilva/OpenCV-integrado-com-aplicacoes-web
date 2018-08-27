#ifndef CARTOONFILTER_H
#define CARTOONFILTER_H

#define NUM_COLORS 8

#include "abstract_filter.h"

class CartoonFilter : public AbstractFilter {
public:
	cv::Mat filter(const cv::Mat &input);
};

#endif // CARTOONFILTER_H
